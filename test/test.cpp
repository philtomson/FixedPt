/*
 * Compile with: C++17 required: (assuming you're in the test directory)
 * * clang: clang++-5.0 -I../include -o test test.cpp -std=c++1z
 * * g++: g++-7 -I../include -o test test.cpp -std=c++1z
 */
#include <iostream>
#include <fixedpt.hpp>
#include <bitset>
#include <assert.h>
#include <cstdint>
#include <typeinfo>
using namespace FPMath;
//template <typename T> std::string type_name();
int main(){
   constexpr uint8_t a_wid  = 5;
   constexpr uint8_t b_wid  = 4;
   constexpr uint8_t a_frac = 3;
   constexpr uint8_t b_frac = 4;
   //FixedPt<uint8_t, 5, 3> a{.whole=8, .frac=7};
   FixedPt< a_wid, a_frac> a(8.25);
   std::cout << "a.to_f(): " << a.to_f() << std::endl;
   std::cout << "double(a): " << double(a) << std::endl;
   assert(float(a) == 8.25) ;
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
   
   std::cout << "a bits: " << a.to_bitstring() << std::endl;
   std::cout << "b bits: " << b.to_bitstring() << std::endl;
   std::cout << "cc2 bits " << cc2.to_bitstring()  << std::endl;

   auto c = a.add(a);
   std::cout << "c bits; " << c.to_bitstring() << std::endl;
   std::cout << "sizeof c: " << sizeof c << std::endl;
   auto diff = a - aa; // should be 0
   std::cout << "diff  bits " << diff.to_bitstring()  << std::endl;
   auto diff2 = aa - a; // should be 0
   std::cout << "diff2 bits " << diff2.to_bitstring()  << std::endl;
   auto s1 = FixedPt<6,5>(2.5);
   std::cout << "s1 bits " << s1.to_bitstring()  << std::endl;
   auto s2 = FixedPt<6,5>(3.5);
   std::cout << "s2 bits " << s2.to_bitstring()  << std::endl;
   auto diff3 = (s1-s2);
   std::cout << "diff3 bits " << diff3.to_bitstring()  << std::endl;
   auto addend = (s2 + diff3);
   std::cout << "addend bits " << addend.to_bitstring()  << std::endl;
   assert(addend.val == 0x7ff);
   auto div = aa/aaa; // should be 1
   std::cout << "div  bits " << div.to_bitstring()  << std::endl;
   assert(float(div) == 1.0);
   assert(div.to_bitstring() == "00001.000");
   auto div2= (FixedPt<a_wid,a_frac>(1.0)/FixedPt<a_wid,a_frac>(2.0));
   std::cout << "div2 bits " << div2.to_bitstring()  << std::endl;
   auto div3= (FixedPt<a_wid,a_frac>(7.0)/FixedPt<b_wid,b_frac>(2.0));
   std::cout << "div3 bits " << div3.to_bitstring()  << std::endl;

   auto mult = aa*FixedPt<a_wid,a_frac>(2.0);  // should be 16.5
   std::cout << "mult  bits " << mult.to_bitstring()  << std::endl;
   assert(float(mult) == 16.5);
   assert(int(mult)   == 16);
   assert(int(mult)   == 16);
   assert(uint8_t(mult) == 0x10);

   auto mult2= (FixedPt<a_wid,a_frac>(1.25)*FixedPt<a_wid,a_frac>(2.0));
   std::cout << "mult2 bits " << mult2.to_bitstring()  << std::endl;
   assert(float(mult2) == 2.5);
   assert(int(mult2)   == 2);
   assert(mult2.to_bitstring() == "00010.100");

   auto mult3= (FixedPt<a_wid,a_frac>(7.0)*FixedPt<b_wid,b_frac>(2.0));
   std::cout << "mult3 bits " << mult3.to_bitstring()  << std::endl;
   assert(float(mult3) == 14.0);
   assert(mult3.to_bitstring() == "01110.0000");

   auto mult3s= (FixedPt<4,4,true>(7.0)*FixedPt<4,4,true>(2.0));
   std::cout << "mult3s bits " << mult3s.to_bitstring()  << std::endl;
   std::cout << "mult3s to float: " << float(mult3s)  << std::endl;
   assert(float(mult3s) == 7.9375);
   assert(mult3s.to_bitstring() == "0111.1111");

   auto mult3as= (FixedPt<5,4,true>(7.0)*FixedPt<5,4,true>(2.0));
   std::cout << "mult3as bits " << mult3as.to_bitstring()  << std::endl;
   std::cout << "mult3as to float: " << float(mult3as)  << std::endl;
   assert(float(mult3as) == 14.0);
   assert(mult3as.to_bitstring() == "01110.0000");
   

   auto mult4 = (FixedPt<5, 3>(1.5)*FixedPt<5,3>(1.5));
   std::cout << "mult4 bits " << mult4.to_bitstring() << std::endl;
   assert(float(mult4) == 2.25);
   assert(mult4.to_bitstring() == "00010.010");
   assert(int(mult4) == 2);

   auto mult5 = (FixedPt<3, 3>(4.25)*FixedPt<3,3>(4.0));
   std::cout << "mult5 bits " << mult5.to_bitstring() << " (should saturate)"<< std::endl;
   assert(mult5.to_bitstring() == "111.111");

   //test multiplication with different sizes
   auto mult6 = (FixedPt<2,4>(2.25)*FixedPt<5,3>(1.5));
   std::cout << "mult6 bits " << mult6.to_bitstring() << std::endl;
   assert(float(mult6) == 3.375);
   assert(mult6.to_bitstring() == "00011.0110");

   //test multiplication with different sizes & saturation 
   auto mult6a = (FixedPt<3,4>(6.25)*FixedPt<2,3>(2.5));
   std::cout << "mult6a bits " << mult6a.to_bitstring() << std::endl;
   std::cout << "float(mult6a): " << float(mult6a) << std::endl;
   assert(float(mult6a) == 7.9375);
   assert(mult6a.to_bitstring() == "111.1111");

   //test multiplication with different sizes same signedness
   auto mult6b = (FixedPt<3,4,true>(6.00)*FixedPt<2,3,true>(2.0));
   std::cout << "mult6b bits " << mult6b.to_bitstring() << std::endl;
   std::cout << "float(mult6b): " << float(mult6b) << std::endl;
   assert(float(mult6b) == 3.9375);
   assert(mult6b.to_bitstring() == "011.1111");

   FixedPt<3,3> maxout;
   //maxout = maxout.max_val();
   std::cout << "maxout.max_val() is: "<< std::hex << unsigned(maxout.max_val()) << std::endl;

   std::cout << "aa  bits " << aa.to_bitstring()  << std::endl;
   auto two = FixedPt<a_wid, a_frac>(2);
   std::cout << "two  bits " << two.to_bitstring()  << std::endl;
   
   std::cout << "a bits: " << a.to_bitstring() << std::endl;
   for(int i = 0; i < 3; ++i){
      a.add_(a);
      std::cout << "a bits: " << a.to_bitstring() << std::endl;
   }
   a = aa.add(aa);
   std::cout << "after assignment: a bits: " << a.to_bitstring() << std::endl;
   aa = 4.4;
   aa = (4.4 + 1.1);
   std::cout << "aa bits; " << aa.to_bitstring() << std::endl;



   //FixedPt<17,16> zz(1.5);
   FixedPt<17,16,true> zz(8589934591.25); //just over max value
   assert(int64_t(zz)    == 65535);
   assert(int(zz)        == 65535);
   assert(zz.get_whole() == 65535);
   std::cout << "zz.get_frac(): " << std::hex << zz.get_frac() << std::endl;
   assert(zz.get_frac() == 65535);

   using zz_type = typename TypeForSize_<17+16,true>::type;
   assert(zz_type(zz) == 65535);
   //Or alternatively:
   assert((FixedPt<17,16,true>::val_t)(zz) == 65535);
   //yet another way to do it:['
   assert((decltype(zz.val))(zz) == 65535);



   std::cout << "sizeof zz: " << sizeof zz << std::endl;
   std::cout << "zz bits: " << zz.to_bitstring() << std::endl;
   assert(zz.to_bitstring() == "01111111111111111.1111111111111111");
   std::cout << "sizeof(zz): " << sizeof(zz) << std::endl;
   auto zzmax = zz.max_val();
   auto shift_amt = (uint64_t(1) << (17+16))-1;
   std::cout <<  "zz.max_val() is: " << std::dec << (zzmax) << std::endl;
   

}
