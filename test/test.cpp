/*
 * Compile with: C++17 required: (assuming you're in the test directory)
 * * clang: clang++-5.0 -I../include -o test test.cpp -std=c++1z
 * * g++: g++-7 -I../include -o test test.cpp -std=c++1z
 */
#include <iostream>
#include <fixedpt.hpp>
#include <bitset>
using namespace FP;
int main(){
   constexpr uint8_t a_wid  = 5;
   constexpr uint8_t b_wid  = 4;
   constexpr uint8_t a_frac = 3;
   constexpr uint8_t b_frac = 4;
   //FixedPt<uint8_t, 5, 3> a{.whole=8, .frac=7};
   FixedPt< a_wid, a_frac> a(8.25);
   FixedPt< a_wid, a_frac> aa(8.25);
   auto cc = a + aa;
   std::cout << "size of a: " << sizeof a << std::endl;
   std::cout << "max value for a: " << a.max_val() << std::endl;
   float fn = 2.5f;
   FixedPt< b_wid, b_frac> b(fn);
   auto cc2 = a + b;
   std::cout << "cc2.wwidth() " << int(cc2.wwidth()) << " cc2.fracwidth() " << int(cc2.fracwidth()) << std::endl;
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
