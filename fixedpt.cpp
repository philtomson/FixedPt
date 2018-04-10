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
 * Compile with: C++17 required:
 * * clang: clang++-5.0 -o fixedpt fixedpt.cpp -std=c++1z
 * * g++: g++-7 -o fixedpt fixedpt.cpp -std=c++1z
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

int main(){
   constexpr uint8_t a_wid  = 5;
   constexpr uint8_t b_wid  = 4;
   constexpr uint8_t a_frac = 3;
   constexpr uint8_t b_frac = 4;
   //FixedPt<uint8_t, 5, 3> a{.whole=8, .frac=7};
   FixedPt< a_wid, a_frac> a(8.25);
   FixedPt< a_wid, a_frac> aa(8.25);
   std::cout << "size of a: " << sizeof a << std::endl;
   std::cout << "max value for a: " << a.max_val() << std::endl;
   float fn = 2.5f;
   FixedPt< b_wid, b_frac> b(fn);
   std::cout << "b bits: " << b.to_bitset() << std::endl;
   auto c = a.add(a);
   std::cout << "c bits; " << c.to_bitset() << std::endl;
   std::cout << "sizeof c: " << sizeof c << std::endl;
   /* 
   for(uint8_t i = 1; i < 8; ++i){
      FixedPt<(8-i), i> some(8.25);
      std::cout << "some.bits: " << some.to_bitset() << std::endl;
   }
   */
   
   std::cout << "a bits: " << a.to_bitset() << std::endl;
   for(int i = 0; i < 10; ++i){
      a.add_(a);
      std::cout << "a bits: " << a.to_bitset() << std::endl;
   }
   a = aa.add(aa);
   std::cout << "after assignment: a bits: " << a.to_bitset() << std::endl;
   aa = 4.4;
   aa = (4.4 + 1.1);
   std::cout << "aa bits; " << aa.to_bitset() << std::endl;

   FixedPt<17,16> zz(1.5);
   std::cout << "sizeof zz: " << sizeof zz << std::endl;

}
