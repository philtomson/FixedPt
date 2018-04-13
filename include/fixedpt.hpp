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
#include <cstdint>
#include <iostream>
#include <algorithm>
#include <math.h>
#include <bitset>

namespace FP {
//set SAT to true if you want values to saturate instead of overflow
static bool SAT = true; 

//if it resolves to this TypeForSize_ type then you'll get a compiler error:
template<std::size_t N, typename=void> 
struct TypeForSize_;

template<std::size_t N>
struct TypeForSize_<N, std::enable_if_t< (N > 0 && N < 9)>>
{
   using type = uint8_t;
};

template<std::size_t N>
struct TypeForSize_<N, std::enable_if_t< (N > 8 && N < 17 )>>
{
   using type = uint16_t;
};

template<std::size_t N>
struct TypeForSize_<N, std::enable_if_t< (N > 16 && N < 33 )>>
{
   using type = uint32_t;
};

template<std::size_t N>
struct TypeForSize_<N, std::enable_if_t< (N > 32 && N < 65 )>>
{
   using type = uint64_t;
};

template< uint8_t WWIDTH, uint8_t FRACWIDTH>
struct FixedPt {
      using val_t = typename TypeForSize_<WWIDTH+FRACWIDTH>::type;

      using FP = FixedPt< WWIDTH, FRACWIDTH>;
      constexpr static const int MAX_VAL = (1 << (WWIDTH+FRACWIDTH))-1;
      val_t val    : (WWIDTH + FRACWIDTH);

      template<typename FT>
      val_t float2fixed(const FT& n){
         return static_cast<val_t>(round(n * pow(2.0, FRACWIDTH)));  
      }

      std::bitset<WWIDTH+FRACWIDTH> to_bitset(){
         return std::bitset<WWIDTH+FRACWIDTH>(val);
      }

      uint8_t wwidth(){
         return WWIDTH;
      }

      uint8_t fracwidth(){
         return FRACWIDTH;
      }

      int max_val() {
         constexpr const int max_val = (1<<(WWIDTH+FRACWIDTH))-1;
         return (max_val);
      }

      //default c'tor
      FixedPt() : val(0) { }

      //move c'tor
      FixedPt(val_t&& v) : val(v) { }

      // c'tor from double
      FixedPt(const double& n) {
         uint64_t whole = static_cast<uint64_t>(n);
         float frac  = n - whole;
         val = float2fixed(n);
      }

      // c'tor from float
      FixedPt(const float& n) {
         uint64_t whole = static_cast<uint64_t>(n);
         val = float2fixed<float>(n);
      }

      // copy c'tor
      FixedPt(const FP& other){
         val = other.val;
      }

      // move c'tor
      FixedPt(FP&& o){
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
      void add_(FixedPt& n) {
         int res = static_cast<int>(val + n.val);
         if(SAT && (res > max_val())) 
            val = max_val();
         else
            val = val + n.val; 
      }

      auto add(FixedPt& n) {
         auto res = val + n.val;
         if(SAT && (res > max_val())) 
            res = max_val();
         return FixedPt< WWIDTH, FRACWIDTH>(res); 
      }
};

// infix + for FixedPts of same width
template<uint8_t WWID, uint8_t FWID> 
auto operator+(FixedPt<WWID,FWID> a, FixedPt<WWID,FWID> b)
{
   return FixedPt<WWID,FWID>(a.val + b.val);
}

// infix + for FixedPts of differing widths
template<uint8_t AWWID, uint8_t AFWID, uint8_t BWWID, uint8_t BFWID> 
auto operator+(FixedPt<AWWID,AFWID> a, FixedPt<BWWID,BFWID> b) -> FixedPt<std::max(AWWID,BWWID), 
                                                                          std::max(AFWID,BFWID)>
{
   return FixedPt<std::max(AWWID,BWWID),std::max(AFWID,BFWID)>(a.val + b.val);
}

// infix - for FixedPts of same width
template<uint8_t WWID, uint8_t FWID> 
auto operator-(FixedPt<WWID,FWID> a, FixedPt<WWID,FWID> b)
{
   return FixedPt<WWID,FWID>(a.val - b.val);
}

// infix + for FixedPts of differing widths
template<uint8_t AWWID, uint8_t AFWID, uint8_t BWWID, uint8_t BFWID> 
auto operator-(FixedPt<AWWID,AFWID> a, FixedPt<BWWID,BFWID> b) -> FixedPt<std::max(AWWID,BWWID), 
                                                                          std::max(AFWID,BFWID)>
{
   return FixedPt<std::max(AWWID,BWWID),std::max(AFWID,BFWID)>(a.val - b.val);
}

// infix * for FixedPts of same width
template<uint8_t WWID, uint8_t FWID> 
auto operator*(FixedPt<WWID,FWID> a, FixedPt<WWID,FWID> b)
{
   return FixedPt<WWID,FWID>(a.val * b.val);
}

// infix * for FixedPts of differing widths
template<uint8_t AWWID, uint8_t AFWID, uint8_t BWWID, uint8_t BFWID> 
auto operator*(FixedPt<AWWID,AFWID> a, FixedPt<BWWID,BFWID> b) -> FixedPt<std::max(AWWID,BWWID), 
                                                                          std::max(AFWID,BFWID)>
{
   return FixedPt<std::max(AWWID,BWWID),std::max(AFWID,BFWID)>(a.val * b.val);
}

// infix / for FixedPts of same width
template<uint8_t WWID, uint8_t FWID> 
auto operator/(FixedPt<WWID,FWID> a, FixedPt<WWID,FWID> b)
{
   return FixedPt<WWID,FWID>(a.val / b.val);
}

// infix / for FixedPts of differing widths
template<uint8_t AWWID, uint8_t AFWID, uint8_t BWWID, uint8_t BFWID> 
auto operator/(FixedPt<AWWID,AFWID> a, FixedPt<BWWID,BFWID> b) -> FixedPt<std::max(AWWID,BWWID), 
                                                                          std::max(AFWID,BFWID)>
{
   return FixedPt<std::max(AWWID,BWWID),std::max(AFWID,BFWID)>(a.val / b.val);
}

} //namespace FP
