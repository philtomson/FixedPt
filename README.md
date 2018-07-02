# FixedPt: A minimalistic, efficient (hopefully), header-only Fixed Point C++ library

This started out as a code-doodle, a vehicle for playing with modern C++ features. 
I wanted a fast, efficient, minimalistic Fixed Point math package (NOTE: It's not
all here yet!) that I could use in determining precision needed in nerual network
implementations - the goal being to easily plug in fixed point operations in place
of floating point operations and then determine how many bits of precision are 
required to get reasonable results out of the network.

I've designed fixed point math packages in Ruby and Julia in the past and there tends 
to be a lot of shifting and masking because you've only got certain fixed-sized integer 
types to work with. Recently while pondering the problem, I realized that perhaps
using C++'s bitfields along with templates would be a way around all of those
extra operations: once you have your floating point numbers converted to a fixed point 
defined using bitfields you can just do math on them as normal without all of those
extra operations for each operation. And so this code was developed. This is a proof
of concept and a work in progress - not all operations are supported yet.


## Advantages: 

   * Time efficient as it uses bit fields to
     set up the value so that you only do ops on the number
     of bits you specify (no need for shifts & masks in math operations).

   * Space efficient as it determines the underlying type based on 
     the number of bits you specify (up to 64 bits).
  
## Limitations: 

   * FixedPts can only have up to 64 bits (uint64_t) (for my usage this isn't
     really a limitation, but YMMV)

   * Each size FixedPt is a different type(FixedPt<WWID,FWID> ) this makes it
     difficult to define infix math ops on different sized FixedPts (could
     add a common base class so operations could be defined, but that would
     add overhead of virtual functions (the vptr).
  
## Usage

     See test.cpp in the tests directory.
  
##  TODO: 

   * Everything is unsigned right now; add signed ops/val types.

   * For saturation use macros to keep things efficient (could add a max_val member 
     that keeps track of the maximum value a FixedPt has seen).

   * Need to add conversion functions/casts to convert to different sizes.

   * wrappers for Julia using Cxx.jl? : As it turns out this is problematic. C++ does not allow non-const references to bitfields and Cxx.jl needs to take a reference to the val member of the FixedPt struct in order to pass values back to the Julia side. Hence I've started working on a separate FixedPt.jl implementation.
     
## Gotchyas:

   * The FixedPt struct contains a bit field member (val). Therefore you cannot take a pointer or non-const reference to this bitfield.

