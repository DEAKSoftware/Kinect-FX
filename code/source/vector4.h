/*===========================================================================
   Four Component Vector Template Class

   Dominik Deak
  ===========================================================================*/

#ifndef ___VECTOR_4_H___
#define ___VECTOR_4_H___


/*---------------------------------------------------------------------------
   Header files
  ---------------------------------------------------------------------------*/
#include "common.h"
#include "math.h"


//Namespaces
NAMESPACE_BEGIN(NAMESPACE_PROJECT)


/*---------------------------------------------------------------------------
  3D operator macros. NOTE: All vector4 template classes will use 3D 
  operators by default, which ignores the homogeneous coordinate W. 
  Therefore, the initialisation of the homogeneous coordinate must be handled
  externally. If four component computations are required, use named operators 
  instead.
  ---------------------------------------------------------------------------*/

//Unary operator
#define VECTOR_3_OPERATOR_UNARY(_op_) \
   inline vector4<TYPE> operator _op_ (void) const \
      { \
      return vector4(_op_(X), _op_(Y), _op_(Z), W); \
      }

//Binary operator, uses a constant operand
#define VECTOR_3_OPERATOR_k(_op_) \
   inline vector4<TYPE> operator _op_ (const TYPE k) const \
      { \
      return vector4(X _op_ k, Y _op_ k, Z _op_ k, W); \
      }

//Binary operator, uses a vector operand
#define VECTOR_3_OPERATOR(_op_) \
   inline vector4<TYPE> operator _op_ (const vector4<TYPE> &V) const \
      { \
      return vector4(X _op_ V.X, Y _op_ V.Y, Z _op_ V.Z, W); \
      }

//Binary assignment operator, uses a constant operand
#define VECTOR_3_OPERATOR_ASSIGN_k(_op_) \
   inline vector4<TYPE> &operator _op_ (const TYPE k) \
      { \
      X _op_ k; \
      Y _op_ k; \
      Z _op_ k; \
      return *this; \
      }

//Binary assignment operator, uses a vector operand
#define VECTOR_3_OPERATOR_ASSIGN(_op_) \
   inline vector4<TYPE> &operator _op_ (const vector4<TYPE> &V) \
      { \
      X _op_ V.X; \
      Y _op_ V.Y; \
      Z _op_ V.Z; \
      return *this; \
      }


/*---------------------------------------------------------------------------
  Named operator macros, used for four-component computations. NOTE: Named
  operators do not use self assignment. For example, the expression V.Add4(N) 
  has no effect, because the returned value is not stored anywhere. To remedy 
  this, use the expression V = V.Add4(N) instead.
  ---------------------------------------------------------------------------*/

//Unary operator
#define VECTOR_4_OPERATOR_UNARY(_NAME_, _op_) \
   inline vector4<TYPE> _NAME_ (void) const \
      { \
      return vector4(_op_(X), _op_(Y), _op_(Z), _op_(W)); \
      }

//Binary operator, uses a constant operand
#define VECTOR_4_OPERATOR_k(_NAME_, _op_) \
   inline vector4<TYPE> _NAME_ (const TYPE k) const \
      { \
      return vector4(X _op_ k, Y _op_ k, Z _op_ k, W _op_ k); \
      }

//Binary operator, uses a vector operand
#define VECTOR_4_OPERATOR(_NAME_, _op_) \
   inline vector4<TYPE> _NAME_ (const vector4<TYPE> &V) const \
      { \
      return vector4(X _op_ V.X, Y _op_ V.Y, Z _op_ V.Z, W _op_ V.W); \
      }


/*---------------------------------------------------------------------------
  The vector4 union structure, suitable for built-in/intrinsic types.
  ---------------------------------------------------------------------------*/
template <typename TYPE> class vector4
   {
   //---- Member data ----
   public:

   union {TYPE X, U, R, C0;};
   union {TYPE Y, V, G, C1;};
   union {TYPE Z, S, B, C2;};
   union {TYPE W, T, A, C3;};

   //---- Methods ----
   inline vector4(void) {}
   inline ~vector4(void) {}

   inline vector4(const TYPE k);
   inline vector4(const TYPE x, const TYPE y, const TYPE z, const TYPE w);
   inline vector4(const vector4<TYPE> &V);

   inline void Set2(const TYPE x, const TYPE y);
   inline void Set3(const TYPE x, const TYPE y, const TYPE z);
   inline void Set4(const TYPE x, const TYPE y, const TYPE z, const TYPE w);

   //Operators for integers and floating point numbers
   VECTOR_3_OPERATOR_k(+)
   VECTOR_3_OPERATOR(+)
   VECTOR_3_OPERATOR_ASSIGN_k(+=)
   VECTOR_3_OPERATOR_ASSIGN(+=)
   VECTOR_4_OPERATOR_k(Add4, +)
   VECTOR_4_OPERATOR(Add4, +)

   VECTOR_3_OPERATOR_UNARY(-)
   VECTOR_4_OPERATOR_UNARY(Neg4, -)

   VECTOR_3_OPERATOR_k(-)
   VECTOR_3_OPERATOR(-)
   VECTOR_3_OPERATOR_ASSIGN_k(-=)
   VECTOR_3_OPERATOR_ASSIGN(-=)
   VECTOR_4_OPERATOR_k(Sub4, -)
   VECTOR_4_OPERATOR(Sub4, -)

   VECTOR_3_OPERATOR_k(*)
   VECTOR_3_OPERATOR(*)
   VECTOR_3_OPERATOR_ASSIGN_k(*=)
   VECTOR_3_OPERATOR_ASSIGN(*=)
   VECTOR_4_OPERATOR_k(Mul4, *)
   VECTOR_4_OPERATOR(Mul4, *)

   VECTOR_3_OPERATOR_k(/)
   VECTOR_3_OPERATOR(/)
   VECTOR_3_OPERATOR_ASSIGN_k(/=)
   VECTOR_3_OPERATOR_ASSIGN(/=)
   VECTOR_4_OPERATOR_k(Div4, /)
   VECTOR_4_OPERATOR(Div4, /)

   inline vector4<TYPE> operator % (const TYPE k) const;
   inline vector4<TYPE> operator % (const vector4<TYPE> &V) const;
   inline vector4<TYPE> &operator %= (const TYPE k);
   inline vector4<TYPE> &operator %= (const vector4<TYPE> &V);
   inline vector4<TYPE> Mod4(const TYPE k) const;
   inline vector4<TYPE> Mod4(const vector4<TYPE> &V) const;
   inline vector4<TYPE> &Mod4(const TYPE k);
   inline vector4<TYPE> &Mod4(const vector4<TYPE> &V);

   //Operators for integers
   VECTOR_3_OPERATOR_UNARY(~)
   VECTOR_4_OPERATOR_UNARY(Not4, ~)

   VECTOR_3_OPERATOR_k(<<)
   VECTOR_3_OPERATOR(<<)
   VECTOR_3_OPERATOR_ASSIGN_k(<<=)
   VECTOR_3_OPERATOR_ASSIGN(<<=)
   VECTOR_4_OPERATOR_k(Shl4, <<)
   VECTOR_4_OPERATOR(Shl4, <<)

   VECTOR_3_OPERATOR_k(>>)
   VECTOR_3_OPERATOR(>>)
   VECTOR_3_OPERATOR_ASSIGN_k(>>=)
   VECTOR_3_OPERATOR_ASSIGN(>>=)
   VECTOR_4_OPERATOR_k(Shr4, >>)
   VECTOR_4_OPERATOR(Shr4, >>)

   VECTOR_3_OPERATOR_k(&)
   VECTOR_3_OPERATOR(&)
   VECTOR_3_OPERATOR_ASSIGN_k(&=)
   VECTOR_3_OPERATOR_ASSIGN(&=)
   VECTOR_4_OPERATOR_k(And4, &)
   VECTOR_4_OPERATOR(And4, &)

   VECTOR_3_OPERATOR_k(|)
   VECTOR_3_OPERATOR(|)
   VECTOR_3_OPERATOR_ASSIGN_k(|=)
   VECTOR_3_OPERATOR_ASSIGN(|=)
   VECTOR_4_OPERATOR_k(Or4, |)
   VECTOR_4_OPERATOR(Or4, |)

   VECTOR_3_OPERATOR_k(^)
   VECTOR_3_OPERATOR(^)
   VECTOR_3_OPERATOR_ASSIGN_k(^=)
   VECTOR_3_OPERATOR_ASSIGN(^=)
   VECTOR_4_OPERATOR_k(Xor4, ^)
   VECTOR_4_OPERATOR(Xor4, ^)

   //Operators for integers and floating point numbers
   inline vector4<TYPE> Abs3(void) const;
   inline vector4<TYPE> Abs4(void) const;
   inline vector4<TYPE> Min3(const vector4<TYPE> &V) const;
   inline vector4<TYPE> Min4(const vector4<TYPE> &V) const;
   inline vector4<TYPE> Max3(const vector4<TYPE> &V) const;
   inline vector4<TYPE> Max4(const vector4<TYPE> &V) const;
   inline vector4<TYPE> Clamp3(const TYPE Min, const TYPE Max) const;
   inline vector4<TYPE> Clamp4(const TYPE Min, const TYPE Max) const;
   inline vector4<TYPE> Clamp3(const vector4<TYPE> &Min, const vector4<TYPE> &Max) const;
   inline vector4<TYPE> Clamp4(const vector4<TYPE> &Min, const vector4<TYPE> &Max) const;
   inline vector4<TYPE> Shuffle(const uiter I0, const uiter I1, const uiter I2, const uiter I3) const;

   //Operators for floating point numbers
   inline vector4<TYPE> Rcp3(void) const;
   inline vector4<TYPE> Rcp4(void) const;
   inline TYPE MagSqr3(void) const;
   inline TYPE MagSqr4(void) const;
   inline TYPE Mag3(void) const;
   inline TYPE Mag4(void) const;
   inline vector4<TYPE> Unit3(void) const;
   inline vector4<TYPE> Unit4(void) const;
   inline TYPE Dot3(const vector4<TYPE> &V) const;
   inline TYPE Dot4(const vector4<TYPE> &V) const;
   inline vector4<TYPE> Cross3(const vector4<TYPE> &V) const;
   };


/*---------------------------------------------------------------------------
   Copy constant constructor.
  ---------------------------------------------------------------------------*/
template <typename TYPE>
inline vector4<TYPE>::vector4(const TYPE k) : 
   X(k), Y(k), Z(k), W(k) 
   {}
   
/*---------------------------------------------------------------------------
   Constructor that copies individual elements.
  ---------------------------------------------------------------------------*/
template <typename TYPE>
inline vector4<TYPE>::vector4(const TYPE x, const TYPE y, const TYPE z, const TYPE w) : 
   X(x), Y(y), Z(z), W(w) 
   {}

/*---------------------------------------------------------------------------
   Copy constructor.
  ---------------------------------------------------------------------------*/
template <typename TYPE>
inline vector4<TYPE>::vector4(const vector4<TYPE> &V) :
   X(V.X), Y(V.Y), Z(V.Z), W(V.W) 
   {}

/*---------------------------------------------------------------------------
   Methods for intializing members.
  ---------------------------------------------------------------------------*/
template <typename TYPE>
inline void vector4<TYPE>::Set2(const TYPE x, const TYPE y)
   {
   X = x;
   Y = y;
   }

template <typename TYPE>
inline void vector4<TYPE>::Set3(const TYPE x, const TYPE y, const TYPE z)
   {
   X = x;
   Y = y;
   Z = z;
   }

template <typename TYPE>
inline void vector4<TYPE>::Set4(const TYPE x, const TYPE y, const TYPE z, const TYPE w)
   {
   X = x;
   Y = y;
   Z = z;
   W = w;
   }

/*---------------------------------------------------------------------------
   Modulo arithmetic, 3D version.
  ---------------------------------------------------------------------------*/
template <typename TYPE>
inline vector4<TYPE> vector4<TYPE>::operator % (const TYPE k) const
   {
   return vector4
      (
      Math::Mod(X, k), 
      Math::Mod(Y, k), 
      Math::Mod(Z, k),
      W
      );
   }

template <typename TYPE>
inline vector4<TYPE> vector4<TYPE>::operator % (const vector4<TYPE> &V) const
   {
   return vector4
      (
      Math::Mod(X, V.X), 
      Math::Mod(Y, V.Y), 
      Math::Mod(Z, V.Z),
      W
      );
   }

template <typename TYPE>
inline vector4<TYPE> &vector4<TYPE>::operator %= (const TYPE k)
   {
   X = Math::Mod(X, k); 
   Y = Math::Mod(Y, k); 
   Z = Math::Mod(Z, k); 
   return *this;
   }

template <typename TYPE>
inline vector4<TYPE> &vector4<TYPE>::operator %= (const vector4<TYPE> &V)
   {
   X = Math::Mod(X, V.X); 
   Y = Math::Mod(Y, V.Y); 
   Z = Math::Mod(Z, V.Z); 
   return *this;
   }

/*---------------------------------------------------------------------------
   Modulo arithmetic, 4D version.
  ---------------------------------------------------------------------------*/
template <typename TYPE>
inline vector4<TYPE> vector4<TYPE>::Mod4(const TYPE k) const
   {
   return vector4
      (
      Math::Mod(X, k), 
      Math::Mod(Y, k), 
      Math::Mod(Z, k), 
      Math::Mod(W, k)
      );
   }

template <typename TYPE>
inline vector4<TYPE> vector4<TYPE>::Mod4(const vector4<TYPE> &V) const
   {
   return vector4
      (
      Math::Mod(X, V.X), 
      Math::Mod(Y, V.Y), 
      Math::Mod(Z, V.Z), 
      Math::Mod(W, V.W)
      );
   }

template <typename TYPE>
inline vector4<TYPE> &vector4<TYPE>::Mod4(const TYPE k)
   {
   X = Math::Mod(X, k); 
   Y = Math::Mod(Y, k); 
   Z = Math::Mod(Z, k); 
   W = Math::Mod(W, k);
   return *this;
   }

template <typename TYPE>
inline vector4<TYPE> &vector4<TYPE>::Mod4(const vector4<TYPE> &V)
   {
   X = Math::Mod(X, V.X); 
   Y = Math::Mod(Y, V.Y); 
   Z = Math::Mod(Z, V.Z); 
   W = Math::Mod(W, V.W);
   return *this;
   }

/*---------------------------------------------------------------------------
   Returns absolute value.
  ---------------------------------------------------------------------------*/
//3D version
template <typename TYPE>
inline vector4<TYPE> vector4<TYPE>::Abs3(void) const
   {
   return vector4(Math::Abs(X), Math::Abs(Y), Math::Abs(Z), W);
   }

//4D version
template <typename TYPE>
inline vector4<TYPE> vector4<TYPE>::Abs4(void) const
   {
   return vector4(Math::Abs(X), Math::Abs(Y), Math::Abs(Z), Math::Abs(W));
   }

/*---------------------------------------------------------------------------
   Returns the minimum values between this vector and the one specified.
  ---------------------------------------------------------------------------*/
//3D version
template <typename TYPE>
inline vector4<TYPE> vector4<TYPE>::Min3(const vector4<TYPE> &V) const
   {
   return vector4
      (
      Math::Min(X, V.X), 
      Math::Min(Y, V.Y), 
      Math::Min(Z, V.Z),
      W
      );
   }

//4D version
template <typename TYPE>
inline vector4<TYPE> vector4<TYPE>::Min4(const vector4<TYPE> &V) const
   {
   return vector4
      (
      Math::Min(X, V.X), 
      Math::Min(Y, V.Y), 
      Math::Min(Z, V.Z), 
      Math::Min(W, V.W)
      );
   }

/*---------------------------------------------------------------------------
   Returns the maximum values between this vector and the one specified.
  ---------------------------------------------------------------------------*/
//3D version
template <typename TYPE>
inline vector4<TYPE> vector4<TYPE>::Max3(const vector4<TYPE> &V) const
   {
   return vector4
      (
      Math::Max(X, V.X), 
      Math::Max(Y, V.Y), 
      Math::Max(Z, V.Z),
      W
      );
   }

//4D version
template <typename TYPE>
inline vector4<TYPE> vector4<TYPE>::Max4(const vector4<TYPE> &V) const
   {
   return vector4
      (
      Math::Max(X, V.X), 
      Math::Max(Y, V.Y), 
      Math::Max(Z, V.Z), 
      Math::Max(W, V.W)
      );
   }

/*---------------------------------------------------------------------------
   Clamps the values of this vector to the specified range.
  ---------------------------------------------------------------------------*/
//3D version
template <typename TYPE>
inline vector4<TYPE> vector4<TYPE>::Clamp3(const TYPE Min, const TYPE Max) const
   {
   return vector4
      (
      Math::Clamp(X, Min, Max), 
      Math::Clamp(Y, Min, Max), 
      Math::Clamp(Z, Min, Max),
      W
      );
   }

//4D version
template <typename TYPE>
inline vector4<TYPE> vector4<TYPE>::Clamp4(const TYPE Min, const TYPE Max) const
   {
   return vector4
      (
      Math::Clamp(X, Min, Max), 
      Math::Clamp(Y, Min, Max), 
      Math::Clamp(Z, Min, Max), 
      Math::Clamp(W, Min, Max)
      );
   }

/*---------------------------------------------------------------------------
   Clamps the values of this vector to the specified component range of the
   Min and Max vectors.
  ---------------------------------------------------------------------------*/
//3D version
template <typename TYPE>
inline vector4<TYPE> vector4<TYPE>::Clamp3(const vector4<TYPE> &Min, const vector4<TYPE> &Max) const
   {
   return vector4
      (
      Math::Clamp(X, Min.X, Max.X), 
      Math::Clamp(Y, Min.Y, Max.Y), 
      Math::Clamp(Z, Min.Z, Max.Z),
      W
      );
   }

//4D version
template <typename TYPE>
inline vector4<TYPE> vector4<TYPE>::Clamp4(const vector4<TYPE> &Min, const vector4<TYPE> &Max) const
   {
   return vector4
      (
      Math::Clamp(X, Min.X, Max.X), 
      Math::Clamp(Y, Min.Y, Max.Y), 
      Math::Clamp(Z, Min.Z, Max.Z), 
      Math::Clamp(W, Min.W, Max.W)
      );
   }

/*---------------------------------------------------------------------------
   Shuffles vector components according to index specifier I0, I1, I2 and I3.
   Valid range for each index specifier is [0, 3]. For example, if 
   V = [4, 5, 6, 7], then V.Shuffle(0, 3, 2, 1) will return [4, 7, 6, 5].
  ---------------------------------------------------------------------------*/
template <typename TYPE>
inline vector4<TYPE> vector4<TYPE>::Shuffle(const uiter I0, const uiter I1, const uiter I2, const uiter I3) const
   {
   const TYPE C[4] = {C0, C1, C2, C3}; 

   return vector4
      (
      C[I0 & 0x03], 
      C[I1 & 0x03], 
      C[I2 & 0x03], 
      C[I3 & 0x03]
      );
   }

/*---------------------------------------------------------------------------
   Computes the reciprocal of this vector.
  ---------------------------------------------------------------------------*/
//3D version
template <typename TYPE>
inline vector4<TYPE> vector4<TYPE>::Rcp3(void) const
   {
   return vector4(1 / X, 1 / Y, 1 / Z, W);
   }

//4D version
template <typename TYPE>
inline vector4<TYPE> vector4<TYPE>::Rcp4(void) const
   {
   return vector4(1 / X, 1 / Y, 1 / Z, 1 / W);
   }

/*---------------------------------------------------------------------------
   Computes the vector magnitude squared.
  ---------------------------------------------------------------------------*/
//3D version
template <typename TYPE>
inline TYPE vector4<TYPE>::MagSqr3(void) const
   {
   return sqr(X) + sqr(Y) + sqr(Z);
   }

//4D version
template <typename TYPE>
inline TYPE vector4<TYPE>::MagSqr4(void) const
   {
   return sqr(X) + sqr(Y) + sqr(Z) + sqr(W);
   }

/*---------------------------------------------------------------------------
   Computes the vector magnitude.
  ---------------------------------------------------------------------------*/
//3D version
template <typename TYPE>
inline TYPE vector4<TYPE>::Mag3(void) const
   {
   return sqrt(sqr(X) + sqr(Y) + sqr(Z));
   }

//4D version
template <typename TYPE>
inline TYPE vector4<TYPE>::Mag4(void) const
   {
   return sqrt(sqr(X) + sqr(Y) + sqr(Z) + sqr(W));
   }

/*---------------------------------------------------------------------------
   Computes the unit vector.
  ---------------------------------------------------------------------------*/
//3D version
template <typename TYPE>
inline vector4<TYPE> vector4<TYPE>::Unit3(void) const
   {
   TYPE m = this->Mag3();
   return Math::Zero(m) ? 0 : *this * (1 / m);
   }

//4D version
template <typename TYPE>
inline vector4<TYPE> vector4<TYPE>::Unit4(void) const
   {
   TYPE m = this->Mag4();
   return Math::Zero(m) ? 0 : Mul4(1 / m);
   }

/*---------------------------------------------------------------------------
   Computes the dot product.
  ---------------------------------------------------------------------------*/
//3D version
template <typename TYPE>
inline TYPE vector4<TYPE>::Dot3(const vector4<TYPE> &V) const
   {
   return X*V.X + Y*V.Y + Z*V.Z;
   }

//4D version
template <typename TYPE>
inline TYPE vector4<TYPE>::Dot4(const vector4<TYPE> &V) const
   {
   return X*V.X + Y*V.Y + Z*V.Z + W*V.W;
   }

/*---------------------------------------------------------------------------
   Computes the cross product.
  ---------------------------------------------------------------------------*/
template <typename TYPE>
inline vector4<TYPE> vector4<TYPE>::Cross3(const vector4<TYPE> &V) const
   {
   return vector4
      (
      Y*V.Z - Z*V.Y,
      Z*V.X - X*V.Z,
      X*V.Y - Y*V.X,
      W
      );
   }


//Close namespaces
NAMESPACE_END(NAMESPACE_PROJECT)


//==== End of file ===========================================================
#endif
