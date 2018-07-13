/*********************************************************************************
 * fixedpt.cpp
 * Author: Phil Tomson
 * Date:   4/9/2018
 * Description:
 * Defines a FixedPt struct for fixed point operations.
 * Uses several of C++11/14/17 features
 *
 * Advantages: 
 * * time efficient as it uses bit fields to
 *   set up the value so that you only do ops on the number
 *   of bits you specify (no need for shifts & masks in math operations.
 * * Space efficient as it determines the underlying type based on 
 *   the number of bits you specify (up to 64 bits).
 *
 * Limitations: 
 * * FixedPts can only have up to 64 bits (uint64_t)
 * * Each size FixedPt is a different type(FixedPt<WWID,FWID> ) this makes it
 *   difficult to define infix math ops on different sized FixedPts (could
 *   add a common base class so operations could be defined, but that would
 *   add overhead of virtual functions (the vptr).
 *

 *
 * TODO: 
 * * Everything is unsigned right now; add signed ops/val types
 * * Define more math ops on FixedPts
 * * For saturation use macros to keep things efficient (could add a max_val member 
 *   that keeps track of the maximum value a FixedPt has seen.
 * * Need to add conversion functions/casts to convert to different sizes
 */
#ifndef FIXEDPT_HPP_INCLUDED
#define FIXEDPT_HPP_INCLUDED
#include <cstdint>
#include <iostream>
#include <algorithm>
#include <math.h>
#include <bitset>

namespace FPMath {
//set SAT to true if you want values to saturate instead of overflow
static bool SAT = true; 

//if it resolves to this TypeForSize_ type then you'll get a compiler error:
template<std::size_t N, bool Signed=false, typename=void, typename=void> 
struct TypeForSize_;

template<std::size_t N, bool Signed>
struct TypeForSize_<N, Signed, std::enable_if_t< (N > 0 && N < 9)>, std::enable_if_t<!Signed>>
{
   using type = uint8_t;
};

template<std::size_t N, bool Signed>
struct TypeForSize_<N, Signed, std::enable_if_t< (N > 0 && N < 9)>, std::enable_if_t<Signed>>
{
   using type = int8_t;
};

template<std::size_t N, bool Signed>
struct TypeForSize_<N, Signed, std::enable_if_t< (N > 8 && N < 17 )>, std::enable_if_t<!Signed>>
{
   using type = uint16_t;
};

template<std::size_t N, bool Signed>
struct TypeForSize_<N, Signed, std::enable_if_t< (N > 8 && N < 17 )>, std::enable_if_t<Signed>>
{
   using type = int16_t;
};

template<std::size_t N, bool Signed>
struct TypeForSize_<N, Signed, std::enable_if_t< (N > 16 && N < 33 )>, std::enable_if_t<!Signed>>
{
   using type = uint32_t;
};

template<std::size_t N, bool Signed>
struct TypeForSize_<N, Signed, std::enable_if_t< (N > 16 && N < 33 )>, std::enable_if_t<Signed>>
{
   using type = int32_t;
};

template<std::size_t N, bool Signed>
struct TypeForSize_<N, Signed, std::enable_if_t< (N > 32 && N < 65 )>, std::enable_if_t<!Signed>>
{
   using type = uint64_t;
};

template<std::size_t N, bool Signed>
struct TypeForSize_<N, Signed, std::enable_if_t< (N > 32 && N < 65 )>, std::enable_if_t<Signed>>
{
   using type = int64_t;
};

auto make_mask(int width) {
   return ( (1 << width) - 1);
}


template< uint8_t WWIDTH, uint8_t FRACWIDTH, bool Signed=false>
struct FixedPt {
      
      using val_t = typename TypeForSize_<WWIDTH+FRACWIDTH, Signed>::type;

      using FP = FixedPt< WWIDTH, FRACWIDTH>;
      constexpr static const int 
         MAX_VAL = Signed? (val_t(1) << (WWIDTH+FRACWIDTH-1))-1 :
                           (val_t(1) << (WWIDTH+FRACWIDTH))-1;
      val_t val    : (WWIDTH + FRACWIDTH);

      template<typename FT>
      val_t float2fixed(const FT& n){
         val_t ret_val; 
         if(SAT && n > max_val())
            ret_val = max_val();
         else
            ret_val = static_cast<val_t>(round(n * pow(2.0, FRACWIDTH))); 
         return ret_val;
      }

      std::bitset<WWIDTH+FRACWIDTH> to_bitset(){
         return std::bitset<WWIDTH+FRACWIDTH>(val);
      }

      std::string to_bitstring(){
         std::bitset<WWIDTH+FRACWIDTH> bits  = this->to_bitset();
         std::string bitstr = bits.to_string();
         return bitstr.insert(WWIDTH, ".");
      }

      uint8_t wwidth(){
         return WWIDTH;
      }

      uint8_t fracwidth(){
         return FRACWIDTH;
      }

      val_t max_val() {
         //constexpr const int max_val = (1<<(WWIDTH+FRACWIDTH))-1;

         constexpr const val_t max_val = 
            Signed ?  (val_t(1) << (WWIDTH+FRACWIDTH-1))-1 :
                      (val_t(1) << (WWIDTH+FRACWIDTH  ))-1;
                                               
         //std::cout << std::dec << "WWIDTH: " << unsigned(WWIDTH) << " FRACWIDTH " << unsigned(FRACWIDTH) << std::endl << std::flush; 
         /*
         val_t max_val = Signed?(val_t(1) << (WWIDTH+FRACWIDTH-1))-1 :
                                (val_t(1) << (WWIDTH+FRACWIDTH))-1;
                                */
         return (max_val);
      }

      double to_double(){
         double fractional = 0.0;
         auto v = val;
         for(int i = -(FRACWIDTH); i != 0; ++i){
            if(v & 0x1) {
               fractional += pow(2.0, i);
            }
            v = v >> 1;
         }
         // v should now hold only the whole part of the value
         // fractional should now hold the fractional part
         return double(v)+fractional;
      }

      float to_f(){
         return float(this->to_double());
      }

      //get the whole part of the number as an integral type:
      auto get_whole()  { return val_t(val >> FRACWIDTH); }
      //get the fractional part of the number as an integral type:
      auto get_frac()   { return (make_mask(FRACWIDTH) & val);}
      operator float()  { return this->to_f(); }
      operator double() { return this->to_double(); }
      operator int()    { return val >> FRACWIDTH; }
      operator val_t()  { return val_t(val >> FRACWIDTH); }


      //default c'tor
      FixedPt() : val(0) { }

      //move c'tor
      FixedPt(const val_t&& v) : val(v) { }

      // c'tor from double
      FixedPt(const double& n) {
         //uint64_t whole = static_cast<uint64_t>(n);
         //float frac  = n - whole;
         val = float2fixed(n);
      }

      // c'tor from float
      FixedPt(const float& n) {
         //uint64_t whole = static_cast<uint64_t>(n);
         val = float2fixed<float>(n);
      }

      /* TODO
      // c'tor from bit string
      FixedPt(const std::string& s){

      }
      */

      // copy c'tor both FixedPt's are same size
      FixedPt(const FP& other){
         val = other.val;
      }

      // move c'tor
      FixedPt(const FP&& o){
         val = o.val;
      }

      // assignment operator
      FP& operator=(const FP& other){
         val = other.val;
         return *this;
      }

      // move assignment operator
      FP& operator=(const FP&& other){
         std::cout << "Move assignment operator called!" << std::endl;
         if(this != &other)
            val = other.val;
         return *this;
      }

      FP& operator=(const double& n) {
         std::cout << "assignment from double operator called!" << std::endl;
         val = float2fixed(n);
         return *this;
      }

      FP& operator=(const double&& n) {
         std::cout << "Move assignment from double operator called!" << std::endl;
         val = float2fixed(n);
         return *this;
      }

      FP& operator=(const float& n) {
         val = float2fixed<float>(n);
         return *this;
      }

      //destructive add:
      void add_(const FixedPt& n) {
         int res = static_cast<int>(val + n.val);
         if(SAT && (res > max_val())) 
            val = max_val();
         else
            val = val + n.val; 
      }

      auto add(const FixedPt& n) {
         auto res = val + n.val;
         if(SAT && (res > max_val())) 
            res = max_val();
         return FixedPt< WWIDTH, FRACWIDTH>(res); 
      }
};

// infix + for FixedPts of same width and both unsigned
template<uint8_t WWID, uint8_t FWID> 
auto operator+(FixedPt<WWID,FWID> a, FixedPt<WWID,FWID> b)
{
   auto sum = FixedPt<WWID,FWID>(a.val + b.val);
   if(SAT && ((sum.val < a.val) || (sum.val < b.val))) {
      sum.val = sum.max_val();
   }
   return sum;
}

// infix + for FixedPts of same width and both signed
template<uint8_t WWID, uint8_t FWID> 
auto operator+(FixedPt<WWID,FWID,true> a, FixedPt<WWID,FWID,true> b)
{
   auto sum = FixedPt<WWID,FWID,true>(a.val + b.val);
   if(SAT && ((sum.val < a.val) || (sum.val < b.val))) {
      sum.val = sum.max_val();
   }
   return sum;
}

// infix - for FixedPts of same width and both unsigned
template<uint8_t WWID, uint8_t FWID> 
auto operator-(FixedPt<WWID,FWID> a, FixedPt<WWID,FWID> b)
{
   return FixedPt<WWID,FWID>(a.val - b.val);
}

// infix - for FixedPts of same width and both signed
template<uint8_t WWID, uint8_t FWID> 
auto operator-(FixedPt<WWID,FWID,true> a, FixedPt<WWID,FWID,true> b)
{
   return FixedPt<WWID,FWID,true>(a.val - b.val);
}

// infix + for FixedPts of differing widths and both unsigned
template<uint8_t AWWID, uint8_t AFWID, uint8_t BWWID, uint8_t BFWID> 
auto operator+(FixedPt<AWWID,AFWID> a, FixedPt<BWWID,BFWID> b) -> FixedPt<std::max(AWWID,BWWID), 
                                                                          std::max(AFWID,BFWID)>
{
   auto larger_frac_val  = (a.fracwidth() >= b.fracwidth())? a.val : b.val;
   auto larger_frac_wid  = (a.fracwidth() >= b.fracwidth())? a.fracwidth() : b.fracwidth();
   auto smaller_frac_val = (a.fracwidth() <  b.fracwidth())? a.val : b.val;
   auto smaller_frac_wid = (a.fracwidth() <  b.fracwidth())? a.fracwidth() : b.fracwidth();
   
   auto shift_by = larger_frac_wid - smaller_frac_wid;
   auto sum      = FixedPt<std::max(AWWID,BWWID),std::max(AFWID,BFWID)>
                       (larger_frac_val + (smaller_frac_val << shift_by));
   if(SAT && ((sum.val < a.val) || (sum.val < b.val))) 
      sum.val = sum.max_val();
   return sum;
}

// infix * for FixedPts of same width and both unsigned
template<uint8_t WWID, uint8_t FWID> 
auto operator*(FixedPt<WWID,FWID> a, FixedPt<WWID,FWID> b)
{
   //This will be problematic if 2*(WWID+FWID) > 64!
   auto prod  = FixedPt<2*WWID,2*FWID>(a.val * b.val);
   auto prod2 = FixedPt<WWID, FWID>(prod.val >> FWID);
   if(SAT && (prod.val >> (WWID+2*FWID)) > 0){
      prod2.val = prod2.max_val();
   }
   return prod2;
}
// infix * for FixedPts of same width and same signedness
template<uint8_t WWID, uint8_t FWID, bool SIGNED> 
auto operator*(FixedPt<WWID,FWID,SIGNED> a, FixedPt<WWID,FWID,SIGNED> b)
{
   //This will be problematic if 2*(WWID+FWID) > 64!
   auto prod  = FixedPt<2*WWID,2*FWID,SIGNED>(a.val * b.val);
   auto prod2 = FixedPt<WWID, FWID,SIGNED>(prod.val >> FWID);
   if(SIGNED && ((a.val > 0 && b.val > 0) || (a.val < 0 && b.val < 0))){ 
      //make sure the sign bit is cleared in this case:
      prod2.val = prod2.val & (prod2.max_val() );
   }
   auto adjustment = SIGNED ? 1 : 0;
   if(SAT && (prod.val >> (WWID+2*FWID-adjustment)) > 0){
      prod2.val = prod2.max_val();
   }
   return prod2;
}

// infix * for FixedPts of differing widths
template<uint8_t AWWID, uint8_t AFWID, uint8_t BWWID, uint8_t BFWID> 
auto operator*(FixedPt<AWWID,AFWID> a, FixedPt<BWWID,BFWID> b) -> FixedPt<std::max(AWWID,BWWID), 
                                                                          std::max(AFWID,BFWID)>
{
   auto larger_frac_val  = (a.fracwidth() >= b.fracwidth())? a.val : b.val;
   auto larger_frac_wid  = (a.fracwidth() >= b.fracwidth())? a.fracwidth() : b.fracwidth();
   auto smaller_frac_val = (a.fracwidth() <  b.fracwidth())? a.val : b.val;
   auto smaller_frac_wid = (a.fracwidth() <  b.fracwidth())? a.fracwidth() : b.fracwidth();
   const auto max_wwid = std::max(AWWID, BWWID);
   const auto max_fwid = std::max(AFWID, BFWID);
   
   auto shift_by     = larger_frac_wid - smaller_frac_wid;
   //This will be problematic if 2*(max_wwid+max_fwid) > 64!
   auto prod  = FixedPt<max_wwid*2,max_fwid*2>
                       (larger_frac_val * (smaller_frac_val << shift_by));
   auto prod2 = FixedPt<max_wwid, max_fwid>(prod.val >> max_fwid);
   
   if(SAT && (prod.val >> (max_wwid+2*max_fwid)) > 0) {
      prod2.val = prod2.max_val();
   }
  
   return prod2;
}

// infix * for FixedPts of differing widths, same signedness
template<uint8_t AWWID, uint8_t AFWID, uint8_t BWWID, uint8_t BFWID, bool SIGNED> 
auto operator*(FixedPt<AWWID,AFWID,SIGNED> a, FixedPt<BWWID,BFWID,SIGNED> b)-> 
                                                                 FixedPt<std::max(AWWID,BWWID), 
                                                                         std::max(AFWID,BFWID),
                                                                         SIGNED>
{
   auto larger_frac_val  = (a.fracwidth() >= b.fracwidth())? a.val : b.val;
   auto larger_frac_wid  = (a.fracwidth() >= b.fracwidth())? a.fracwidth() : b.fracwidth();
   auto smaller_frac_val = (a.fracwidth() <  b.fracwidth())? a.val : b.val;
   auto smaller_frac_wid = (a.fracwidth() <  b.fracwidth())? a.fracwidth() : b.fracwidth();
   const auto max_wwid = std::max(AWWID, BWWID);
   const auto max_fwid = std::max(AFWID, BFWID);
   
   auto shift_by     = larger_frac_wid - smaller_frac_wid;
   //This will be problematic if 2*(max_wwid+max_fwid) > 64!
   auto prod  = FixedPt<max_wwid*2,max_fwid*2, SIGNED>
                       (larger_frac_val * (smaller_frac_val << shift_by));
   auto prod2 = FixedPt<max_wwid, max_fwid, SIGNED>(prod.val >> max_fwid);
   auto adjustment = SIGNED ? 1 : 0;
   if(SAT && (prod.val >> (max_wwid+2*max_fwid-adjustment)) > 0) {
      prod2.val = prod2.max_val();
   }

   if(SIGNED && ((a.val > 0 && b.val > 0) || (a.val < 0 && b.val < 0))){ 
      //make sure the sign bit is cleared in this case:
      prod2.val = prod2.val & (prod2.max_val() );
   }
  
   return prod2;
}


// infix - for FixedPts of differing widths - non-commutative op
template<uint8_t AWWID, uint8_t AFWID, uint8_t BWWID, uint8_t BFWID> 
auto operator-(FixedPt<AWWID,AFWID> a, FixedPt<BWWID,BFWID> b) -> FixedPt<std::max(AWWID,BWWID), 
                                                                          std::max(AFWID,BFWID)>
{
   auto larger_frac_val  = (a.fracwidth() >= b.fracwidth())? a.val : b.val;
   auto larger_frac_wid  = (a.fracwidth() >= b.fracwidth())? a.fracwidth() : b.fracwidth();
   auto smaller_frac_val = (a.fracwidth() <  b.fracwidth())? a.val : b.val;
   auto smaller_frac_wid = (a.fracwidth() <  b.fracwidth())? a.fracwidth() : b.fracwidth();
   
   auto shift_by = larger_frac_wid - smaller_frac_wid;
   auto diff = (a.val == smaller_frac_val) ? ((a.val << shift_by) - b.val) :
                                              (a.val - (b.val << shift_by));
   return FixedPt<std::max(AWWID,BWWID),std::max(AFWID,BFWID)>(diff);
}

// infix / for FixedPts of differing widths - non-commutative op
template<uint8_t AWWID, uint8_t AFWID, uint8_t BWWID, uint8_t BFWID> 
auto operator/(FixedPt<AWWID,AFWID> a, FixedPt<BWWID,BFWID> b) -> FixedPt<std::max(AWWID,BWWID), 
                                                                          std::max(AFWID,BFWID)>
{
   auto larger_frac_val  = (a.fracwidth() >= b.fracwidth())? a.val : b.val;
   auto larger_frac_wid  = (a.fracwidth() >= b.fracwidth())? a.fracwidth() : b.fracwidth();
   auto smaller_frac_val = (a.fracwidth() <  b.fracwidth())? a.val : b.val;
   auto smaller_frac_wid = (a.fracwidth() <  b.fracwidth())? a.fracwidth() : b.fracwidth();
   
   auto shift_by = larger_frac_wid - smaller_frac_wid;
   auto div = (a.val == smaller_frac_val)? 
        int((float(a.val<<shift_by)/float(b.val))*(1 << std::max(AFWID,BFWID))):
        int((float(a.val)/float(b.val<<shift_by))*(1 << std::max(AFWID,BFWID)));
   return FixedPt<std::max(AWWID,BWWID),std::max(AFWID,BFWID)>(div);
}



// infix / for FixedPts of same width both unsigned
template<uint8_t WWID, uint8_t FWID> 
auto operator/(FixedPt<WWID,FWID> a, FixedPt<WWID,FWID> b)
{
   return FixedPt<WWID,FWID>(int((float(a.val)/float(b.val))*(1 << FWID)));
}

// infix / for FixedPts of same width both signed
template<uint8_t WWID, uint8_t FWID> 
auto operator/(FixedPt<WWID,FWID,true> a, FixedPt<WWID,FWID,true> b)
{
   return FixedPt<WWID,FWID,true>(int((float(a.val)/float(b.val))*(1 << FWID)));
}

} //namespace FP
#endif //FIXEDPT_HPP_INCLUDED
