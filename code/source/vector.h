/*===========================================================================
   Vector Classes

   Dominik Deak
  ===========================================================================*/

#ifndef ___VECTOR_H___
#define ___VECTOR_H___


/*---------------------------------------------------------------------------
   Header files
  ---------------------------------------------------------------------------*/
#include "common.h"
#include "vector2.h"
#include "vector4.h"


//Namespaces
NAMESPACE_BEGIN(NAMESPACE_PROJECT)


/*---------------------------------------------------------------------------
   Type definitions
  ---------------------------------------------------------------------------*/
typedef vector2<bool>   vector2b;
typedef vector2<int8>   vector2i8;
typedef vector2<int16>  vector2i16;
typedef vector2<int32>  vector2i32;
typedef vector2<int64>  vector2i64;
typedef vector2<uint8>  vector2u8;
typedef vector2<uint16> vector2u16;
typedef vector2<uint32> vector2u32;
typedef vector2<uint64> vector2u64;
typedef vector2<char>   vector2c;
typedef vector2<int>    vector2i;
typedef vector2<uint>   vector2u;
typedef vector2<float>  vector2f;
typedef vector2<double> vector2d;

typedef vector4<bool>   vector4b;
typedef vector4<int8>   vector4i8;
typedef vector4<int16>  vector4i16;
typedef vector4<int32>  vector4i32;
typedef vector4<int64>  vector4i64;
typedef vector4<uint8>  vector4u8;
typedef vector4<uint16> vector4u16;
typedef vector4<uint32> vector4u32;
typedef vector4<uint64> vector4u64;
typedef vector4<char>   vector4c;
typedef vector4<int>    vector4i;
typedef vector4<uint>   vector4u;
typedef vector4<float>  vector4f;
typedef vector4<double> vector4d;


/*---------------------------------------------------------------------------
   Casting definitions
  ---------------------------------------------------------------------------*/
#define cast_vector2(TYPE, V) vector2<TYPE>((TYPE)(V).X, (TYPE)(V).Y)
#define cast_vector4(TYPE, V) vector4<TYPE>((TYPE)(V).X, (TYPE)(V).Y, (TYPE)(V).Z, (TYPE)(V).W)

#define cast_vector2_to_4(TYPE, V) vector4<TYPE>((TYPE)(V).X, (TYPE)(V).Y, (TYPE)0, (TYPE)0)
#define cast_vector4_to_2(TYPE, V) vector2<TYPE>((TYPE)(V).X, (TYPE)(V).Y)


//Close namespaces
NAMESPACE_END(NAMESPACE_PROJECT)


//==== End of file ===========================================================
#endif
