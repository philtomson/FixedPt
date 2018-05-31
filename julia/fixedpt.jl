# The start of a native Julia FixedPt implementation based on the c++ FixedPt 
# Wrapping the C++ for use in Julia proved more difficult than anticipated
# So using this as a Julia-learning exercise

insert_bp(str, loc) = str[1:end-loc]*"."*str[end-loc+1:end]

val_t(total_wid, s::Bool) =
                if    (0  < total_wid < 9 )
                    s ? Int8  : UInt8
                 elseif(8  < total_wid < 17)
                    s ? Int16 : UInt16
                 elseif(16 < total_wid < 33)
                    s ? Int32 : UInt32
                 elseif(32 < total_wid < 65)
                    s ? Int64 : UInt64
                 elseif(64 < total_wid < 129)
                    s ? Int128 : UInt128
                 else
                    UInt64
                 end

function float2fp(n::Float64, fwidth::Int64, targetType)
   convert(targetType, round(n*2^fwidth))
end

mutable struct FixedPt{W,F,T, Signed} 
   val::T
   wwid
   fwid
   function FixedPt{W,F,Signed}(val) where {W,F,Signed} 
      total_wid = W+F
      val_type = val_t(total_wid,Signed)
      new{W,F,val_type,Signed}(val,W,F)
   end
   FixedPt{W,F}(val::Float64) where {W,F} = FixedPt{W,F,false}(float2fp(val, F, val_t(W+F,false)))
   FixedPt{W,F,Signed}(val::Float64) where {W,F,Signed} = FixedPt{W,F,Signed}(float2fp(val, F, val_t(W+F,Signed)))
   FixedPt{W,F}(val) where {W,F} = FixedPt{W,F,false}(val)
 end

 to_binstr(fp::FixedPt{W,F,T,Signed}) where {W,F,T,Signed} = insert_bp(bin(fp.val,(W+F)),F)


