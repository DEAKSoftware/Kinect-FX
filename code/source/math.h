/*===========================================================================
   Math Functions

   Dominik Deak
  ===========================================================================*/

#ifndef ___MATH_H___
#define ___MATH_H___


/*---------------------------------------------------------------------------
   Header files
  ---------------------------------------------------------------------------*/
#include "common.h"


//Namespaces
NAMESPACE_BEGIN(NAMESPACE_PROJECT)
NAMESPACE_BEGIN(Math)


/*---------------------------------------------------------------------------
   Constants
  ---------------------------------------------------------------------------*/
const double pi = asin((double)1.0) * 2.0;
const double rcp_pi = 1.0 / pi;

const float feps = FLT_EPSILON;
const float fmax = FLT_MAX;
const double deps = DBL_EPSILON;
const double dmax = DBL_MAX;


/*---------------------------------------------------------------------------
   Macros
  ---------------------------------------------------------------------------*/
#define sqr(X) ((X)*(X))

/*---------------------------------------------------------------------------
   Tests whether the value is either zero, or less than epsilon.
  ---------------------------------------------------------------------------*/
template <typename TYPE> 
inline bool Zero(const TYPE X)
   {
   return X == 0; //Treat as integer
   }

inline bool Zero(const float X)
   {
   return fabs(X) <= feps;
   }

inline bool Zero(const double X)
   {
   return fabs(X) <= deps;
   }

/*---------------------------------------------------------------------------
   Returns the absolute value. This function seems a bit redundant; however,
   due to the g++ compiler invoking the wrong overloaded version of abs( )
   for float types, a custom version of abs( ) is needed.
  ---------------------------------------------------------------------------*/
template <typename TYPE>
inline bool Abs(const TYPE X)
   {
   return abs(X);
   }

inline float Abs(const float X)
   {
   return fabs(X);
   }

inline double Abs(const double X)
   {
   return fabs(X);
   }

/*---------------------------------------------------------------------------
   Modulo aritmetic.
  ---------------------------------------------------------------------------*/
template <typename TYPE> 
inline TYPE Mod(const TYPE A, const TYPE B)
   {
   return A % B;  //Assumes integer type
   }

inline float Mod(const float A, const float B)
   {
   return fmod(A, B);
   }

inline double Mod(const double A, const double B)
   {
   return fmod(A, B);
   }

/*---------------------------------------------------------------------------
   Find reciprocal, but clamp to maximum if zero. Only availeble to float
   and double types.
  ---------------------------------------------------------------------------*/
inline float Reciprocal(const float X, const float Max)
   {
   return Zero(X) ? (X < 0.0f ? -Max : Max) : 1.0f / X;
   }

inline double Reciprocal(const double X, const double Max)
   {
   return Zero(X) ? (X < 0.0 ? -Max : Max) : 1.0 / X;
   }

/*---------------------------------------------------------------------------
   Min/max functions.
  ---------------------------------------------------------------------------*/
template <typename TYPE> 
inline TYPE Min(const TYPE A, const TYPE B)
   {
   return A < B ? A : B;
   }

template <typename TYPE> 
inline TYPE Max(const TYPE A, const TYPE B)
   {
   return A > B ? A : B;
   }

template <typename TYPE> 
inline TYPE Clamp(const TYPE X, const TYPE Min, const TYPE Max)
   {
   return X > Max ? Max : (X < Min ? Min : X);
   }

/*---------------------------------------------------------------------------
   Swap template function.
  ---------------------------------------------------------------------------*/
template <typename TYPE> 
inline void Swap(TYPE &A, TYPE &B)
   {
   TYPE T = A; 
   A = B; 
   B = T;
   } 

/*---------------------------------------------------------------------------
   Angle conversions.
  ---------------------------------------------------------------------------*/
template <typename TYPE> 
inline TYPE RadToDeg(const TYPE Rad)
   {
   return (TYPE)((float)Rad * (float)rcp_pi * 180.0f);
   }

inline float RadToDeg(const float Rad)
   {
   return Rad * (float)rcp_pi * 180.0f;
   }

inline double RadToDeg(const double Rad)
   {
   return Rad * rcp_pi * 180.0;
   }

template <typename TYPE> 
inline TYPE DegToRad(const TYPE Deg)
   {
   return (TYPE)((float)Deg * (float)pi * (1.0f / 180.0f));
   }

inline float DegToRad(const float Deg)
   {
   return Deg * (float)pi * (1.0f / 180.0f);
   }

inline double DegToRad(const double Deg)
   {
   return Deg * pi * (1.0 / 180.0);
   }

/*---------------------------------------------------------------------------
   Returns true if the current machine is little endian.
  ---------------------------------------------------------------------------*/
inline bool MachineLittleEndian(void)
   {
   union {int I; char C[sizeof(int)];} Test;
   Test.I = 0x000F;
   return (Test.C[0] == 0x0F);
   }

/*---------------------------------------------------------------------------
   Swaps the endian-ness of a value.
  ---------------------------------------------------------------------------*/
template <typename TYPE> 
inline TYPE SwapEndian(const TYPE X)
   {
   union {TYPE X; uint8 C[sizeof(TYPE)];} Temp;

   Temp.X = X;

   const usize S = sizeof(TYPE);

   for (usize I = 0; I < (S >> 1); I++)
      {
      register uint8 T = Temp.C[S-I-1];
      Temp.C[S-I-1] = Temp.C[I];
      Temp.C[I] = T;
      }

   return Temp.X;
   }

/*---------------------------------------------------------------------------
   Returns the number of bytes needed to contain certain number of bits. 
  ---------------------------------------------------------------------------*/
inline usize ByteSize(const usize Bits)
   {
   usize Size = Bits >> 3;
   usize Test = Size << 3;
   return Bits != Test ? Size + 1 : Size;
   }


//Close namespaces
NAMESPACE_END(Math)
NAMESPACE_END(NAMESPACE_PROJECT)


//==== End of file ===========================================================
#endif
