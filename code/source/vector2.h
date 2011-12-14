/*===========================================================================
   Two Component Vector Template Class

   Dominik Deak
  ===========================================================================*/

#ifndef ___VECTOR_2_H___
#define ___VECTOR_2_H___


/*---------------------------------------------------------------------------
   Header files
  ---------------------------------------------------------------------------*/
#include "common.h"
#include "math.h"


//Namespaces
NAMESPACE_BEGIN(NAMESPACE_PROJECT)


/*---------------------------------------------------------------------------
  Definitions
  ---------------------------------------------------------------------------*/

//Unary operator
#define VECTOR_2_OPERATOR_UNARY(_op_) \
   inline vector2<TYPE> operator _op_ (void) const \
      { \
      return vector2(_op_(X), _op_(Y)); \
      }

//Binary operator, uses a constant operand
#define VECTOR_2_OPERATOR_k(_op_) \
   inline vector2<TYPE> operator _op_ (const TYPE k) const \
      { \
      return vector2(X _op_ k, Y _op_ k); \
      }

//Binary operator, uses a vector operand
#define VECTOR_2_OPERATOR(_op_) \
   inline vector2<TYPE> operator _op_ (const vector2<TYPE> &V) const \
      { \
      return vector2(X _op_ V.X, Y _op_ V.Y); \
      }


//Binary assignment operator, uses a constant operand
#define VECTOR_2_OPERATOR_ASSIGN_k(_op_) \
   inline vector2<TYPE> &operator _op_ (const TYPE k) \
      { \
      X _op_ k; \
      Y _op_ k; \
      return *this; \
      }

//Binary assignment operator, uses a vector operand
#define VECTOR_2_OPERATOR_ASSIGN(_op_) \
   inline vector2<TYPE> &operator _op_ (const vector2<TYPE> &V) \
      { \
      X _op_ V.X; \
      Y _op_ V.Y; \
      return *this; \
      }


/*---------------------------------------------------------------------------
  The vector2 union structure, suitable for built-in/intrinsic types.
  ---------------------------------------------------------------------------*/
template <typename TYPE> class vector2
   {
   //---- Member data ----
   public:

   union {TYPE X, U, C0;};
   union {TYPE Y, V, C1;};

   //---- Methods ----
   inline vector2(void) {}
   inline ~vector2(void) {}

   inline vector2(const TYPE k);
   inline vector2(const TYPE x, const TYPE y);
   inline vector2(const vector2<TYPE> &V);

   inline void Set(const TYPE x, const TYPE y);

   //Operators for integers and floating point numbers
   VECTOR_2_OPERATOR_k(+)
   VECTOR_2_OPERATOR(+)
   VECTOR_2_OPERATOR_ASSIGN_k(+=)
   VECTOR_2_OPERATOR_ASSIGN(+=)

   VECTOR_2_OPERATOR_UNARY(-)
   VECTOR_2_OPERATOR_k(-)
   VECTOR_2_OPERATOR(-)
   VECTOR_2_OPERATOR_ASSIGN_k(-=)
   VECTOR_2_OPERATOR_ASSIGN(-=)

   VECTOR_2_OPERATOR_k(*)
   VECTOR_2_OPERATOR(*)
   VECTOR_2_OPERATOR_ASSIGN_k(*=)
   VECTOR_2_OPERATOR_ASSIGN(*=)

   VECTOR_2_OPERATOR_k(/)
   VECTOR_2_OPERATOR(/)
   VECTOR_2_OPERATOR_ASSIGN_k(/=)
   VECTOR_2_OPERATOR_ASSIGN(/=)

   inline vector2<TYPE> operator % (const TYPE k) const;
   inline vector2<TYPE> operator % (const vector2<TYPE> &V) const;
   inline vector2<TYPE> &operator %= (const TYPE k);
   inline vector2<TYPE> &operator %= (const vector2<TYPE> &V);

   //Operators for integers
   VECTOR_2_OPERATOR_UNARY(~)

   VECTOR_2_OPERATOR_k(<<)
   VECTOR_2_OPERATOR(<<)
   VECTOR_2_OPERATOR_ASSIGN_k(<<=)
   VECTOR_2_OPERATOR_ASSIGN(<<=)

   VECTOR_2_OPERATOR_k(>>)
   VECTOR_2_OPERATOR(>>)
   VECTOR_2_OPERATOR_ASSIGN_k(>>=)
   VECTOR_2_OPERATOR_ASSIGN(>>=)

   VECTOR_2_OPERATOR_k(&)
   VECTOR_2_OPERATOR(&)
   VECTOR_2_OPERATOR_ASSIGN_k(&=)
   VECTOR_2_OPERATOR_ASSIGN(&=)

   VECTOR_2_OPERATOR_k(|)
   VECTOR_2_OPERATOR(|)
   VECTOR_2_OPERATOR_ASSIGN_k(|=)
   VECTOR_2_OPERATOR_ASSIGN(|=)

   VECTOR_2_OPERATOR_k(^)
   VECTOR_2_OPERATOR(^)
   VECTOR_2_OPERATOR_ASSIGN_k(^=)
   VECTOR_2_OPERATOR_ASSIGN(^=)

   //Operators for integers and floating point numbers
   inline vector2<TYPE> Abs(void) const;
   inline vector2<TYPE> Min(const vector2<TYPE> &V) const;
   inline vector2<TYPE> Max(const vector2<TYPE> &V) const;
   inline vector2<TYPE> Clamp(const TYPE Min, const TYPE Max) const;
   inline vector2<TYPE> Clamp(const vector2<TYPE> &Min, const vector2<TYPE> &Max) const;
   inline vector2<TYPE> Shuffle(const uiter I0, const uiter I1) const;

   //Operators for floating point numbers
   inline vector2<TYPE> Rcp(void) const;
   inline TYPE MagSqr(void) const;
   inline TYPE Mag(void) const;
   inline vector2<TYPE> Unit(void) const;
   inline TYPE Dot(const vector2<TYPE> &V) const;
   inline TYPE Cross(const vector2<TYPE> &V) const;
   };


/*---------------------------------------------------------------------------
   Copy constant constructor.
  ---------------------------------------------------------------------------*/
template <typename TYPE>
inline vector2<TYPE>::vector2(const TYPE k) : 
   X(k), Y(k)
   {}
   
/*---------------------------------------------------------------------------
   Constructor that copies individual elements.
  ---------------------------------------------------------------------------*/
template <typename TYPE>
inline vector2<TYPE>::vector2(const TYPE x, const TYPE y) : 
   X(x), Y(y)
   {}

/*---------------------------------------------------------------------------
   Copy constructor.
  ---------------------------------------------------------------------------*/
template <typename TYPE>
inline vector2<TYPE>::vector2(const vector2<TYPE> &V) :
   X(V.X), Y(V.Y)
   {}

/*---------------------------------------------------------------------------
   Method for intializing members.
  ---------------------------------------------------------------------------*/
template <typename TYPE>
inline void vector2<TYPE>::Set(const TYPE x, const TYPE y)
   {
   X = x;
   Y = y;
   }

/*---------------------------------------------------------------------------
   Modulo arithmetic.
  ---------------------------------------------------------------------------*/
template <typename TYPE>
inline vector2<TYPE> vector2<TYPE>::operator % (const TYPE k) const
   {
   return vector2
      (
      Math::Mod(X, k), 
      Math::Mod(Y, k)
      );
   }

template <typename TYPE>
inline vector2<TYPE> vector2<TYPE>::operator % (const vector2<TYPE> &V) const
   {
   return vector2
      (
      Math::Mod(X, V.X), 
      Math::Mod(Y, V.Y)
      );
   }

template <typename TYPE>
inline vector2<TYPE> &vector2<TYPE>::operator %= (const TYPE k)
   {
   X = Math::Mod(X, k); 
   Y = Math::Mod(Y, k);
   return *this;
   }

template <typename TYPE>
inline vector2<TYPE> &vector2<TYPE>::operator %= (const vector2<TYPE> &V)
   {
   X = Math::Mod(X, V.X); 
   Y = Math::Mod(Y, V.Y);
   return *this;
   }

/*---------------------------------------------------------------------------
   Returns absolute value.
  ---------------------------------------------------------------------------*/
template <typename TYPE>
inline vector2<TYPE> vector2<TYPE>::Abs(void) const
   {
   return vector2(Math::Abs(X), Math::Abs(Y));
   }

/*---------------------------------------------------------------------------
   Returns the minimum values between this vector and the one specified.
  ---------------------------------------------------------------------------*/
template <typename TYPE>
inline vector2<TYPE> vector2<TYPE>::Min(const vector2<TYPE> &V) const
   {
   return vector2
      (
      Math::Min(X, V.X), 
      Math::Min(Y, V.Y)
      );
   }

/*---------------------------------------------------------------------------
   Returns the maximum values between this vector and the one specified.
  ---------------------------------------------------------------------------*/
template <typename TYPE>
inline vector2<TYPE> vector2<TYPE>::Max(const vector2<TYPE> &V) const
   {
   return vector2
      (
      Math::Max(X, V.X), 
      Math::Max(Y, V.Y)
      );
   }

/*---------------------------------------------------------------------------
   Clamps the values of this vector to the specified range.
  ---------------------------------------------------------------------------*/
template <typename TYPE>
inline vector2<TYPE> vector2<TYPE>::Clamp(const TYPE Min, const TYPE Max) const
   {
   return vector2
      (
      Math::Clamp(X, Min, Max), 
      Math::Clamp(Y, Min, Max)
      );
   }

/*---------------------------------------------------------------------------
   Clamps the values of this vector to the specified component range of the
   Min and Max vectors.
  ---------------------------------------------------------------------------*/
template <typename TYPE>
inline vector2<TYPE> vector2<TYPE>::Clamp(const vector2<TYPE> &Min, const vector2<TYPE> &Max) const
   {
   return vector2
      (
      Math::Clamp(X, Min.X, Max.X), 
      Math::Clamp(Y, Min.Y, Max.Y)
      );
   }

/*---------------------------------------------------------------------------
   Shuffles vector components according to index specifier I0, and I1.
   Valid range for each index specifier is [0, 1]. For example, if 
   V = [4, 5], then V.Shuffle(1, 0) will return [5, 4].
  ---------------------------------------------------------------------------*/
template <typename TYPE>
inline vector2<TYPE> vector2<TYPE>::Shuffle(const uiter I0, const uiter I1) const
   {
   const TYPE C[2] = {C0, C1}; 
   
   return vector2
      (
      C[I0 & 0x01], 
      C[I1 & 0x01]
      );
   }

/*---------------------------------------------------------------------------
   Computes the reciprocal of this vector.
  ---------------------------------------------------------------------------*/
template <typename TYPE>
inline vector2<TYPE> vector2<TYPE>::Rcp(void) const
   {
   return vector2(1 / X, 1 / Y);
   }

/*---------------------------------------------------------------------------
   Computes the magnitude squared.
  ---------------------------------------------------------------------------*/
template <typename TYPE>
inline TYPE vector2<TYPE>::MagSqr(void) const
   {
   return sqr(X) + sqr(Y);
   }

/*---------------------------------------------------------------------------
   Computes the magnitude.
  ---------------------------------------------------------------------------*/
template <typename TYPE>
inline TYPE vector2<TYPE>::Mag(void) const
   {
   return sqrt(sqr(X) + sqr(Y));
   }

/*---------------------------------------------------------------------------
   Computes the unit vector.
  ---------------------------------------------------------------------------*/
template <typename TYPE>
inline vector2<TYPE> vector2<TYPE>::Unit(void) const
   {
   TYPE m = this->Mag();
   return Math::Zero(m) ? 0 : *this * (1 / m);
   }

/*---------------------------------------------------------------------------
   Computes the dot product.
  ---------------------------------------------------------------------------*/
template <typename TYPE>
inline TYPE vector2<TYPE>::Dot(const vector2<TYPE> &V) const
   {
   return X*V.X + Y*V.Y;
   }

/*---------------------------------------------------------------------------
   Computes the cross product.
  ---------------------------------------------------------------------------*/
template <typename TYPE>
inline TYPE vector2<TYPE>::Cross(const vector2<TYPE> &V) const
   {
   return X*V.Y - Y*V.X;
   }


//Close namespaces
NAMESPACE_END(NAMESPACE_PROJECT)


//==== End of file ===========================================================
#endif
