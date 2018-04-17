/*
 * Compile with: C++17 required: (assuming you're in the test directory)
 * * clang: clang++-5.0 -I../include -o test test.cpp -std=c++1z
 * * g++: g++-7 -I../include -o test test.cpp -std=c++1z
 */
#include <iostream>
#include <fixedpt.hpp>
#include <bitset>
#include <assert.h>
using namespace FP;
int main(){
   constexpr uint8_t a_wid  = 5;
   constexpr uint8_t b_wid  = 4;
   constexpr uint8_t a_frac = 3;
   constexpr uint8_t b_frac = 4;
   //FixedPt<uint8_t, 5, 3> a{.whole=8, .frac=7};
   FixedPt< a_wid, a_frac> a(8.25);
   std::cout << "a.to_f(): " << a.to_f() << std::endl;
   assert(a.to_f() == 8.25) ;
   FixedPt< a_wid, a_frac> aa(8.25);
   FixedPt< a_wid, a_frac> aaa(a);
   auto cc = a + aa;
   std::cout << "size of a: " << sizeof a << std::endl;
   assert((sizeof a) == 1);
   std::cout << "max value for a: " << a.max_val() << std::endl;
   float fn = 2.5f;
   FixedPt< b_wid, b_frac> b(fn);
   assert(b.to_f() == 2.5);
   auto cc2 = a + b;
   std::cout << "cc2.wwidth() " << int(cc2.wwidth()) << " cc2.fracwidth() " << int(cc2.fracwidth()) << std::endl;
   assert(cc2.wwidth() == std::max(a.wwidth(), b.wwidth()));
   assert(cc2.fracwidth() == std::max(a.fracwidth(), b.fracwidth()));
   auto comp = std::bitset<std::max(a_wid, b_wid)+
                           std::max(a_frac, b_frac)>{std::string("010101100")};
   assert(cc2.to_bitset() == std::bitset<std::max(a_wid, b_wid)+
                                         std::max(a_frac, b_frac)>{std::string("010101100")});
   
   std::cout << "a bits: " << a.to_bitset() << std::endl;
   std::cout << "b bits: " << b.to_bitset() << std::endl;
   std::cout << "cc2 bits " << cc2.to_bitset()  << std::endl;

   auto c = a.add(a);
   std::cout << "c bits; " << c.to_bitset() << std::endl;
   std::cout << "sizeof c: " << sizeof c << std::endl;
   auto diff = a - aa; // should be 0
   std::cout << "diff  bits " << diff.to_bitset()  << std::endl;
   auto diff2 = aa - a; // should be 0
   std::cout << "diff2 bits " << diff2.to_bitset()  << std::endl;
   auto div = aa/aaa; // should be 1
   std::cout << "div  bits " << div.to_bitset()  << std::endl;
   auto div2= (FixedPt<a_wid,a_frac>(1.0)/FixedPt<a_wid,a_frac>(2.0));
   std::cout << "div2 bits " << div2.to_bitset()  << std::endl;
   auto div3= (FixedPt<a_wid,a_frac>(7.0)/FixedPt<b_wid,b_frac>(2.0));
   std::cout << "div3 bits " << div3.to_bitset()  << std::endl;

   auto mult = aa*FixedPt<a_wid,a_frac>(2.0);  // should be 16.5
   std::cout << "mult  bits " << mult.to_bitset()  << std::endl;
   auto mult2= (FixedPt<a_wid,a_frac>(1.25)*FixedPt<a_wid,a_frac>(2.0));
   std::cout << "mult2 bits " << mult2.to_bitset()  << std::endl;
   auto mult3= (FixedPt<a_wid,a_frac>(7.0)*FixedPt<b_wid,b_frac>(2.0));
   std::cout << "mult3 bits " << mult3.to_bitset()  << std::endl;

   std::cout << "aa  bits " << aa.to_bitset()  << std::endl;
   auto two = FixedPt<a_wid, a_frac>(2);
   std::cout << "two  bits " << two.to_bitset()  << std::endl;
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
