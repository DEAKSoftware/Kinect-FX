/*===========================================================================
   OpenGL Compatible 4 x 4 Matrix Operations

   Dominik Deak

   All matrices are in the column major format, as illustrated below. The 
   coordinate system used by OpenGL is illustrated on the left, where the 
   positive direction of X axis points to the left, the Y axis points 
   upwards, and the Z axis towards the viewpoint. This is also called the 
   "right handed" coordinate system.

     +y       [C00, C04, C08, C0C] - X Components of axis vectors
      |__ +x  [C01, C05, C09, C0D] - Y Components of axis vectors
      /       [C02, C06, C0A, C0E] - Z Components of axis vectors
    +z        [C03, C07, C0B, C0F] - Homogeneous (W) components of axis vectors
                |    |    |    |
                |    |    |    |____ Translation component
                |    |    |_________ X axis vector (left)
                |    |______________ Y axis vector (up)
                |___________________ Z axis vector (forward)

   Element m15 is the homogeneous coordinate, generally used for 
   projection transformations. Axis vectors in a column major matrix can 
   accessed in the following way:

   Vx{C00, C01, C02, C03} = {E[0][0], E[0][1], E[0][2], E[0][3]};
   Vy{C04, C05, C06, C07} = {E[1][0], E[1][1], E[1][2], E[1][3]};
   Vz{C08, C09, C0A, C0B} = {E[2][0], E[2][1], E[2][2], E[2][3]};
   Vt{C0C, C0D, C0E, C0F} = {E[3][0], E[3][1], E[3][2], E[3][3]};

   The linear order of matrix elements in memory is as follows:

   C00, C01, C02, C03;  C04, C05, C06, C07;  C08, C09, C0A, C0B;  
   C0C, C0D, C0E, C0F.

   This is where confusion can arise, as the mathematical notation of the
   matrix appears to be the transpose in actual memory. General rule, each
   element in a column major matrix is accessed via: offs = col*4 + row,
   where "col" is the horizontal index and "row" is the vertical index.
  ===========================================================================*/

#ifndef ___MATRIX_H___
#define ___MATRIX_H___


/*---------------------------------------------------------------------------
   Header files
  ---------------------------------------------------------------------------*/
#include "common.h"
#include "math.h"
#include "vector.h"


//Namespaces
NAMESPACE_BEGIN(NAMESPACE_PROJECT)


/*---------------------------------------------------------------------------
  Definitions
  ---------------------------------------------------------------------------*/

//Unary operator
#define MATRIX_OPERATOR_UNARY(_op_) \
   inline matrix16<TYPE> operator _op_ (void) const \
      { \
      return matrix16(_op_(C[0x00]), _op_(C[0x01]), _op_(C[0x02]), _op_(C[0x03]), \
                      _op_(C[0x04]), _op_(C[0x05]), _op_(C[0x06]), _op_(C[0x07]), \
                      _op_(C[0x08]), _op_(C[0x09]), _op_(C[0x0A]), _op_(C[0x0B]), \
                      _op_(C[0x0C]), _op_(C[0x0D]), _op_(C[0x0E]), _op_(C[0x0F])); \
      }

//Binary operator, uses a constant operand
#define MATRIX_OPERATOR_k(_op_) \
   inline matrix16<TYPE> operator _op_ (const TYPE k) const \
      { \
      return matrix16(C[0x00] _op_ k, C[0x01] _op_ k, C[0x02] _op_ k, C[0x03] _op_ k, \
                      C[0x04] _op_ k, C[0x05] _op_ k, C[0x06] _op_ k, C[0x07] _op_ k, \
                      C[0x08] _op_ k, C[0x09] _op_ k, C[0x0A] _op_ k, C[0x0B] _op_ k, \
                      C[0x0C] _op_ k, C[0x0D] _op_ k, C[0x0E] _op_ k, C[0x0F] _op_ k); \
      }

//Binary operator, uses a matrix operand
#define MATRIX_OPERATOR(_op_) \
   inline matrix16<TYPE> operator _op_ (const matrix16<TYPE> &M) const \
      { \
      return matrix16(C[0x00] _op_ M.C[0x00], C[0x01] _op_ M.C[0x01], C[0x02] _op_ M.C[0x02], C[0x03] _op_ M.C[0x03], \
                      C[0x04] _op_ M.C[0x04], C[0x05] _op_ M.C[0x05], C[0x06] _op_ M.C[0x06], C[0x07] _op_ M.C[0x07], \
                      C[0x08] _op_ M.C[0x08], C[0x09] _op_ M.C[0x09], C[0x0A] _op_ M.C[0x0A], C[0x0B] _op_ M.C[0x0B], \
                      C[0x0C] _op_ M.C[0x0C], C[0x0D] _op_ M.C[0x0D], C[0x0E] _op_ M.C[0x0E], C[0x0F] _op_ M.C[0x0F]); \
      }


//Binary assignment operator, uses a constant operand
#define MATRIX_OPERATOR_ASSIGN_k(_op_) \
   inline matrix16<TYPE> &operator _op_ (const TYPE k) \
      { \
      C[0x00] _op_ k; C[0x01] _op_ k; C[0x02] _op_ k; C[0x03] _op_ k; \
      C[0x04] _op_ k; C[0x05] _op_ k; C[0x06] _op_ k; C[0x07] _op_ k; \
      C[0x08] _op_ k; C[0x09] _op_ k; C[0x0A] _op_ k; C[0x0B] _op_ k; \
      C[0x0C] _op_ k; C[0x0D] _op_ k; C[0x0E] _op_ k; C[0x0F] _op_ k; \
      return *this; \
      }

//Binary assignment operator, uses a matrix operand
#define MATRIX_OPERATOR_ASSIGN(_op_) \
   inline matrix16<TYPE> &operator _op_ (const matrix16<TYPE> &M) \
      { \
      C[0x00] _op_ M.C[0x00]; C[0x01] _op_ M.C[0x01]; C[0x02] _op_ M.C[0x02]; C[0x03] _op_ M.C[0x03]; \
      C[0x04] _op_ M.C[0x04]; C[0x05] _op_ M.C[0x05]; C[0x06] _op_ M.C[0x06]; C[0x07] _op_ M.C[0x07]; \
      C[0x08] _op_ M.C[0x08]; C[0x09] _op_ M.C[0x09]; C[0x0A] _op_ M.C[0x0A]; C[0x0B] _op_ M.C[0x0B]; \
      C[0x0C] _op_ M.C[0x0C]; C[0x0D] _op_ M.C[0x0D]; C[0x0E] _op_ M.C[0x0E]; C[0x0F] _op_ M.C[0x0F]; \
      return *this; \
      }


/*---------------------------------------------------------------------------
  4 x 4 matrix structure, suitable for built-in/intrinsic types.
  ---------------------------------------------------------------------------*/
template <typename TYPE> union matrix16
   {
   //---- Member data -----
   public:

   TYPE C[16];                                     //Cell access
   TYPE E[4][4];                                   //Element access

   //---- Methods -----
   inline matrix16(void) {}
   inline ~matrix16(void) {}

   inline matrix16(const TYPE k);
   inline matrix16(const matrix16<TYPE> &M);
   inline matrix16
      (
      const TYPE c00, const TYPE c01, const TYPE c02, const TYPE c03, 
      const TYPE c04, const TYPE c05, const TYPE c06, const TYPE c07, 
      const TYPE c08, const TYPE c09, const TYPE c0A, const TYPE c0B, 
      const TYPE c0C, const TYPE c0D, const TYPE c0E, const TYPE c0F
      );

   MATRIX_OPERATOR_k(+)
   MATRIX_OPERATOR(+)
   MATRIX_OPERATOR_ASSIGN_k(+=)
   MATRIX_OPERATOR_ASSIGN(+=)

   MATRIX_OPERATOR_UNARY(-)
   MATRIX_OPERATOR_k(-)
   MATRIX_OPERATOR(-)
   MATRIX_OPERATOR_ASSIGN_k(-=)
   MATRIX_OPERATOR_ASSIGN(-=)

   MATRIX_OPERATOR_k(*)
   inline matrix16<TYPE> operator * (const matrix16<TYPE> &M) const;
   MATRIX_OPERATOR_ASSIGN_k(*=)
   inline matrix16<TYPE> &operator *= (const matrix16<TYPE> &M);
   inline vector4<TYPE> operator * (const vector4<TYPE> &V) const;

   inline matrix16<TYPE> Identity(void) const;
   inline matrix16<TYPE> Transpose(void) const;
   inline TYPE Determinant(void) const;
   inline matrix16<TYPE> Inverse(void) const;
   inline matrix16<TYPE> Normal(void) const;

   inline matrix16<TYPE> Scale(const TYPE X, const TYPE Y, const TYPE Z) const;
   inline matrix16<TYPE> Scale(const vector4<TYPE> &V) const;
   inline matrix16<TYPE> Translate(const TYPE X, const TYPE Y, const TYPE Z) const;
   inline matrix16<TYPE> Translate(const vector4<TYPE> &V) const;
   inline matrix16<TYPE> Rotate(const TYPE A, const TYPE X, const TYPE Y, const TYPE Z) const;
   inline matrix16<TYPE> Rotate(TYPE A, const vector4<TYPE> &V) const;

   inline matrix16<TYPE> Frustum(const TYPE Left, const TYPE Right, const TYPE Bottom, const TYPE Top, const TYPE Near, const TYPE Far) const;
   inline matrix16<TYPE> Perspective(const TYPE FOV, const TYPE Aspect, const TYPE Near, const TYPE Far) const;
   inline matrix16<TYPE> Ortho(const TYPE Left, const TYPE Right, const TYPE Bottom, const TYPE Top, const TYPE Near, const TYPE Far) const;

   static inline vector4<TYPE> Project(const matrix16<TYPE> &MV, const matrix16<TYPE> &MP, const vector4<TYPE> &ViewPort, const vector2<TYPE> &Depth, const vector4<TYPE> &V);
   };

/*---------------------------------------------------------------------------
   Copy constant constructor.
  ---------------------------------------------------------------------------*/
template <typename TYPE>
inline matrix16<TYPE>::matrix16(const TYPE k)
   {
   C[0x00] = k; C[0x01] = k; C[0x02] = k; C[0x03] = k;
   C[0x04] = k; C[0x05] = k; C[0x06] = k; C[0x07] = k;
   C[0x08] = k; C[0x09] = k; C[0x0A] = k; C[0x0B] = k;
   C[0x0C] = k; C[0x0D] = k; C[0x0E] = k; C[0x0F] = k;
   }

/*---------------------------------------------------------------------------
   Constructor that copies individual elements.
  ---------------------------------------------------------------------------*/
template <typename TYPE>
inline matrix16<TYPE>::matrix16
   (
   const TYPE c00, const TYPE c01, const TYPE c02, const TYPE c03, 
   const TYPE c04, const TYPE c05, const TYPE c06, const TYPE c07, 
   const TYPE c08, const TYPE c09, const TYPE c0A, const TYPE c0B, 
   const TYPE c0C, const TYPE c0D, const TYPE c0E, const TYPE c0F
   )
   {
   C[0x00] = c00; C[0x01] = c01; C[0x02] = c02; C[0x03] = c03;
   C[0x04] = c04; C[0x05] = c05; C[0x06] = c06; C[0x07] = c07;
   C[0x08] = c08; C[0x09] = c09; C[0x0A] = c0A; C[0x0B] = c0B;
   C[0x0C] = c0C; C[0x0D] = c0D; C[0x0E] = c0E; C[0x0F] = c0F;
   }

/*---------------------------------------------------------------------------
   Copy constructor.
  ---------------------------------------------------------------------------*/
template <typename TYPE>
inline matrix16<TYPE>::matrix16(const matrix16<TYPE> &M)
   {
   C[0x00] = M.C[0x00]; C[0x01] = M.C[0x01]; C[0x02] = M.C[0x02]; C[0x03] = M.C[0x03];
   C[0x04] = M.C[0x04]; C[0x05] = M.C[0x05]; C[0x06] = M.C[0x06]; C[0x07] = M.C[0x07];
   C[0x08] = M.C[0x08]; C[0x09] = M.C[0x09]; C[0x0A] = M.C[0x0A]; C[0x0B] = M.C[0x0B];
   C[0x0C] = M.C[0x0C]; C[0x0D] = M.C[0x0D]; C[0x0E] = M.C[0x0E]; C[0x0F] = M.C[0x0F];
   }

/*---------------------------------------------------------------------------
   Matrix multiply. Note that matrix multiplications do not operate in 
   parallel.
  ---------------------------------------------------------------------------*/
template <typename TYPE>
inline matrix16<TYPE> matrix16<TYPE>::operator * (const matrix16<TYPE> &M) const
   {
   matrix16<TYPE> T;
   
   for (uiter I = 0; I < 4; I++)
      {
      T.E[0][I] = E[0][I] * M.E[0][0] + E[1][I] * M.E[0][1] + E[2][I] * M.E[0][2] + E[3][I] * M.E[0][3];
      T.E[1][I] = E[0][I] * M.E[1][0] + E[1][I] * M.E[1][1] + E[2][I] * M.E[1][2] + E[3][I] * M.E[1][3];
      T.E[2][I] = E[0][I] * M.E[2][0] + E[1][I] * M.E[2][1] + E[2][I] * M.E[2][2] + E[3][I] * M.E[2][3];
      T.E[3][I] = E[0][I] * M.E[3][0] + E[1][I] * M.E[3][1] + E[2][I] * M.E[3][2] + E[3][I] * M.E[3][3];
      }
   
   return T;
   }

/*---------------------------------------------------------------------------
   Matrix assignment multiply. Note that matrix multiplications do not 
   operate in parallel.
  ---------------------------------------------------------------------------*/
template <typename TYPE>
inline matrix16<TYPE> &matrix16<TYPE>::operator *= (const matrix16<TYPE> &M)
   {
   matrix16<TYPE> T;
   
   for (uiter I = 0; I < 4; I++)
      {
      T.E[0][I] = E[0][I] * M.E[0][0] + E[1][I] * M.E[0][1] + E[2][I] * M.E[0][2] + E[3][I] * M.E[0][3];
      T.E[1][I] = E[0][I] * M.E[1][0] + E[1][I] * M.E[1][1] + E[2][I] * M.E[1][2] + E[3][I] * M.E[1][3];
      T.E[2][I] = E[0][I] * M.E[2][0] + E[1][I] * M.E[2][1] + E[2][I] * M.E[2][2] + E[3][I] * M.E[2][3];
      T.E[3][I] = E[0][I] * M.E[3][0] + E[1][I] * M.E[3][1] + E[2][I] * M.E[3][2] + E[3][I] * M.E[3][3];
      }
   
   *this = T;

   return *this;
   }

/*---------------------------------------------------------------------------
   Matrix multiply with a vector. Used for transformation.
  ---------------------------------------------------------------------------*/
template <typename TYPE>
inline vector4<TYPE> matrix16<TYPE>::operator * (const vector4<TYPE> &V) const
   {
   return vector4<TYPE>
      (
      V.X * E[0][0] + V.Y * E[1][0] + V.Z * E[2][0] + V.W * E[3][0],
      V.X * E[0][1] + V.Y * E[1][1] + V.Z * E[2][1] + V.W * E[3][1],
      V.X * E[0][2] + V.Y * E[1][2] + V.Z * E[2][2] + V.W * E[3][2],
      V.X * E[0][3] + V.Y * E[1][3] + V.Z * E[2][3] + V.W * E[3][3]
      );
   }

/*---------------------------------------------------------------------------
   Sets current matrix into an identity matrix.
  ---------------------------------------------------------------------------*/
template <typename TYPE>
inline matrix16<TYPE> matrix16<TYPE>::Identity(void) const
   {
   return matrix16(1, 0, 0, 0,  0, 1, 0, 0,  0, 0, 1, 0,  0, 0, 0, 1);
   }

/*---------------------------------------------------------------------------
   Return the transpose.
  ---------------------------------------------------------------------------*/
template <typename TYPE>
inline matrix16<TYPE> matrix16<TYPE>::Transpose(void) const
   {
   return matrix16
      (
      C[0x00], C[0x04], C[0x08], C[0x0C],
      C[0x01], C[0x05], C[0x09], C[0x0D],
      C[0x02], C[0x06], C[0x0A], C[0x0E],
      C[0x03], C[0x07], C[0x0B], C[0x0F]
      );
   }

/*---------------------------------------------------------------------------
   Finds the determinant of the matrix.
   http://www.geometrictools.com/LibFoundation/Mathematics/Wm4Matrix4.inl
  ---------------------------------------------------------------------------*/
template <typename TYPE>
inline TYPE matrix16<TYPE>::Determinant(void) const
   {
   TYPE A0 = C[0x00]*C[0x05] - C[0x01]*C[0x04];
   TYPE A1 = C[0x00]*C[0x06] - C[0x02]*C[0x04];
   TYPE A2 = C[0x00]*C[0x07] - C[0x03]*C[0x04];
   TYPE A3 = C[0x01]*C[0x06] - C[0x02]*C[0x05];
   TYPE A4 = C[0x01]*C[0x07] - C[0x03]*C[0x05];
   TYPE A5 = C[0x02]*C[0x07] - C[0x03]*C[0x06];
   TYPE B0 = C[0x08]*C[0x0D] - C[0x09]*C[0x0C];
   TYPE B1 = C[0x08]*C[0x0E] - C[0x0A]*C[0x0C];
   TYPE B2 = C[0x08]*C[0x0F] - C[0x0B]*C[0x0C];
   TYPE B3 = C[0x09]*C[0x0E] - C[0x0A]*C[0x0D];
   TYPE B4 = C[0x09]*C[0x0F] - C[0x0B]*C[0x0D];
   TYPE B5 = C[0x0A]*C[0x0F] - C[0x0B]*C[0x0E];

   return A0*B5 - A1*B4 + A2*B3 + A3*B2 - A4*B1 + A5*B0;
   }

/*---------------------------------------------------------------------------
   Finds the inverse of a matrix. Note, this operation is applicable to 
   floating point types only.
   http://www.geometrictools.com/Documentation/LaplaceExpansionTheorem.pdf
   http://www.geometrictools.com/LibFoundation/Mathematics/Wm4Matrix4.inl
  ---------------------------------------------------------------------------*/
template <typename TYPE>
inline matrix16<TYPE> matrix16<TYPE>::Inverse(void) const
   {
   //Determinant
   TYPE A0 = C[0x00]*C[0x05] - C[0x01]*C[0x04];
   TYPE A1 = C[0x00]*C[0x06] - C[0x02]*C[0x04];
   TYPE A2 = C[0x00]*C[0x07] - C[0x03]*C[0x04];
   TYPE A3 = C[0x01]*C[0x06] - C[0x02]*C[0x05];
   TYPE A4 = C[0x01]*C[0x07] - C[0x03]*C[0x05];
   TYPE A5 = C[0x02]*C[0x07] - C[0x03]*C[0x06];
   TYPE B0 = C[0x08]*C[0x0D] - C[0x09]*C[0x0C];
   TYPE B1 = C[0x08]*C[0x0E] - C[0x0A]*C[0x0C];
   TYPE B2 = C[0x08]*C[0x0F] - C[0x0B]*C[0x0C];
   TYPE B3 = C[0x09]*C[0x0E] - C[0x0A]*C[0x0D];
   TYPE B4 = C[0x09]*C[0x0F] - C[0x0B]*C[0x0D];
   TYPE B5 = C[0x0A]*C[0x0F] - C[0x0B]*C[0x0E];
   
   TYPE D = A0*B5 - A1*B4 + A2*B3 + A3*B2 - A4*B1 + A5*B0;
   if (Math::Zero(D)) {return 0;}

   matrix16<TYPE> M
      (
       C[0x05]*B5 - C[0x06]*B4 + C[0x07]*B3,
      -C[0x01]*B5 + C[0x02]*B4 - C[0x03]*B3,
       C[0x0D]*A5 - C[0x0E]*A4 + C[0x0F]*A3,
      -C[0x09]*A5 + C[0x0A]*A4 - C[0x0B]*A3,
      -C[0x04]*B5 + C[0x06]*B2 - C[0x07]*B1,
       C[0x00]*B5 - C[0x02]*B2 + C[0x03]*B1,
      -C[0x0C]*A5 + C[0x0E]*A2 - C[0x0F]*A1,
       C[0x08]*A5 - C[0x0A]*A2 + C[0x0B]*A1,
       C[0x04]*B4 - C[0x05]*B2 + C[0x07]*B0,
      -C[0x00]*B4 + C[0x01]*B2 - C[0x03]*B0,
       C[0x0C]*A4 - C[0x0D]*A2 + C[0x0F]*A0,
      -C[0x08]*A4 + C[0x09]*A2 - C[0x0B]*A0,
      -C[0x04]*B3 + C[0x05]*B1 - C[0x06]*B0,
       C[0x00]*B3 - C[0x01]*B1 + C[0x02]*B0,
      -C[0x0C]*A3 + C[0x0D]*A1 - C[0x0E]*A0,
       C[0x08]*A3 - C[0x09]*A1 + C[0x0A]*A0
      );

   return M * (1 / D);
   }

/*---------------------------------------------------------------------------
   Creates a normal trasformation matrix from a the current model view matrix.
   Note, this operation is applicable to floating point types only.
   http://www.songho.ca/opengl/gl_normaltransform.html
  ---------------------------------------------------------------------------*/
template <typename TYPE>
inline matrix16<TYPE> matrix16<TYPE>::Normal(void) const
   {
   return Inverse().Transpose();
   }

/*---------------------------------------------------------------------------
   Applies scaling operation on the current matrix.
  ---------------------------------------------------------------------------*/
template <typename TYPE>
inline matrix16<TYPE> matrix16<TYPE>::Scale(const TYPE X, const TYPE Y, const TYPE Z) const
   {
   matrix16<TYPE> M = *this;
   
   M.E[0][0] *= X;
   M.E[0][1] *= X;
   M.E[0][2] *= X;
   M.E[0][3] *= X;

   M.E[1][0] *= Y;
   M.E[1][1] *= Y;
   M.E[1][2] *= Y;
   M.E[1][3] *= Y;

   M.E[2][0] *= Z;
   M.E[2][1] *= Z;
   M.E[2][2] *= Z;
   M.E[2][3] *= Z;

   return M;
   }

template <typename TYPE>
inline matrix16<TYPE> matrix16<TYPE>::Scale(const vector4<TYPE> &V) const
   {
   return Scale(V.X, V.Y, V.Z);
   }

/*---------------------------------------------------------------------------
   Applies translation operation on the current matrix.
  ---------------------------------------------------------------------------*/
template <typename TYPE>
inline matrix16<TYPE> matrix16<TYPE>::Translate(const TYPE X, const TYPE Y, const TYPE Z) const
   {
   matrix16<TYPE> M = *this;

   M.E[3][0] += (M.E[0][0] * X + M.E[1][0] * Y + M.E[2][0] * Z);
   M.E[3][1] += (M.E[0][1] * X + M.E[1][1] * Y + M.E[2][1] * Z);
   M.E[3][2] += (M.E[0][2] * X + M.E[1][2] * Y + M.E[2][2] * Z);
   M.E[3][3] += (M.E[0][3] * X + M.E[1][3] * Y + M.E[2][3] * Z);

   return M;
   }

template <typename TYPE>
inline matrix16<TYPE> matrix16<TYPE>::Translate(const vector4<TYPE> &V) const
   {
   return Translate(V.X, V.Y, V.Z);
   }

/*---------------------------------------------------------------------------
   Applies rotation operation on the current matrix. Note, this operation is 
   applicable to floating point types only.
  ---------------------------------------------------------------------------*/
template <typename TYPE>
inline matrix16<TYPE> matrix16<TYPE>::Rotate(const TYPE A, const TYPE X, const TYPE Y, const TYPE Z) const
   {
   TYPE m = sqr(X) + sqr(Y) + sqr(Z);
   if (Math::Zero(m)) {return *this;}

   m = 1 / sqrt(m);

   const TYPE a = Math::DegToRad(A);
   const TYPE s = sin(a);
   const TYPE c = cos(a);

   const TYPE x = X * m;
   const TYPE y = Y * m;
   const TYPE z = Z * m;

   const TYPE x2 = sqr(x);
   const TYPE y2 = sqr(y);
   const TYPE z2 = sqr(z);
   const TYPE xy = x * y;
   const TYPE yz = y * z;
   const TYPE zx = z * x;
   const TYPE xs = x * s;
   const TYPE ys = y * s;
   const TYPE zs = z * s;
   const TYPE ci = 1 - c;

   matrix16<TYPE> M;
   M.E[0][0] = ci * x2 + c;
   M.E[1][0] = ci * xy - zs;
   M.E[2][0] = ci * zx + ys;
   M.E[3][0] = 0;

   M.E[0][1] = ci * xy + zs;
   M.E[1][1] = ci * y2 + c;
   M.E[2][1] = ci * yz - xs;
   M.E[3][1] = 0;

   M.E[0][2] = ci * zx - ys;
   M.E[1][2] = ci * yz + xs;
   M.E[2][2] = ci * z2 + c;
   M.E[3][2] = 0;

   M.E[0][3] = 0;
   M.E[1][3] = 0;
   M.E[2][3] = 0;
   M.E[3][3] = 1;

   return *this * M;
   }

template <typename TYPE>
inline matrix16<TYPE> matrix16<TYPE>::Rotate(const TYPE A, const vector4<TYPE> &V) const
   {
   return Rotate(A, V.X, V.Y, V.Z);
   }

/*---------------------------------------------------------------------------
   Creates a view frustum projection matrix. Note, this operation is 
   applicable to floating point types only.
  ---------------------------------------------------------------------------*/
template <typename TYPE>
inline matrix16<TYPE> matrix16<TYPE>::Frustum(const TYPE Left, const TYPE Right, const TYPE Bottom, const TYPE Top, const TYPE Near, const TYPE Far) const
   {
   const TYPE dX = Right - Left;
   const TYPE dY = Top - Bottom;
   const TYPE dZ = Far - Near;

   if (Near <= 0 || Far <= 0 || dX <= 0 || dY <= 0 || dZ <= 0) {return *this;}

   const TYPE rcp_dX = 1 / dX;
   const TYPE rcp_dY = 1 / dY;
   const TYPE rcp_dZ = 1 / dZ;

   matrix16<TYPE> M;
   M.E[0][0] = 2 * Near * rcp_dX;
   M.E[0][1] = 0;
   M.E[0][2] = 0;
   M.E[0][3] = 0;

   M.E[1][1] = 2 * Near * rcp_dY;
   M.E[1][0] = 0;
   M.E[1][2] = 0;
   M.E[1][3] = 0;

   M.E[2][0] = (Right + Left) * rcp_dX;
   M.E[2][1] = (Top + Bottom) * rcp_dY;
   M.E[2][2] = -(Near + Far) * rcp_dZ;
   M.E[2][3] = -1;

   M.E[3][2] = -2 * Near * Far * rcp_dZ;
   M.E[3][0] = 0;
   M.E[3][1] = 0;
   M.E[3][3] = 0;

   return *this * M;
   }

/*---------------------------------------------------------------------------
   Creates a perspective projection matrix. Note, this operation is 
   applicable to floating point types only.
  ---------------------------------------------------------------------------*/
template <typename TYPE>
inline matrix16<TYPE> matrix16<TYPE>::Perspective(const TYPE FOV, const TYPE Aspect, const TYPE Near, const TYPE Far) const
   {
   vector2<TYPE> F;

   F.Y = tan(FOV / 360 * (TYPE)Math::pi) * Near;
   F.X = F.Y * Aspect;

   return Frustum(-F.X, F.X, -F.Y, F.Y, Near, Far);
   }

/*---------------------------------------------------------------------------
   Creates an orthographic projection matrix. Note, this operation is 
   applicable to floating point types only.
  ---------------------------------------------------------------------------*/
template <typename TYPE>
inline matrix16<TYPE> matrix16<TYPE>::Ortho(const TYPE Left, const TYPE Right, const TYPE Bottom, const TYPE Top, const TYPE Near, const TYPE Far) const
   {
   const TYPE dX = Right - Left;
   const TYPE dY = Top - Bottom;
   const TYPE dZ = Far - Near;

   if (dX <= 0 || dY <= 0 || dZ <= 0) {return *this;}

   const TYPE rcp_dX = 1 / dX;
   const TYPE rcp_dY = 1 / dY;
   const TYPE rcp_dZ = 1 / dZ;

   matrix16<TYPE> M;
   M = M.Identity();

   M.E[0][0] =  2 * rcp_dX;
   M.E[1][1] =  2 * rcp_dY;
   M.E[2][2] = -2 * rcp_dZ;
   M.E[3][0] = -(Right + Left) * rcp_dX;
   M.E[3][1] = -(Top + Bottom) * rcp_dY;
   M.E[3][2] = -(Far + Near) * rcp_dZ;

   return *this * M;
   }

/*---------------------------------------------------------------------------
   Performs a 3D projection of a point V, using the model view matrix, the
   projection matrix and the view port. Similar in functionality as the
   function gluProject( ). The code is based on the Mesa source. Note, this 
   operation is applicable to floating point types only.
  
   MV       : Model view matrix.
   MP       : Projection matrix.
   ViewPort : Vector that defines the viewport.
   Depth    : Depth buffer range, generally obtained using GL_DEPTH_RANGE.
   V        : Point or vector to transform.
  ---------------------------------------------------------------------------*/
template <typename TYPE>
inline vector4<TYPE> matrix16<TYPE>::Project(const matrix16<TYPE> &MV, const matrix16<TYPE> &MP, const vector4<TYPE> &ViewPort, const vector2<TYPE> &Depth, const vector4<TYPE> &V)
   {
   vector4<TYPE> A, B;

   A.X = V.X;
   A.Y = V.Y;
   A.Z = V.Z;
   A.W = 1;

   B = MV * A;
   A = MP * B;
   
   if (!Math::Zero(A.W)) {A.W = 1 / A.W;}
   
   //Map X, Y and Z to range [-0.5, 0.5]
   A.W *= (TYPE)0.5;
   A.X *= A.W;
   A.Y *= A.W;
   A.Z *= A.W;

   //Map X and Y to [0.0, 1.0] range, Z to depth range, then map X, Y to view port
   B.X = ViewPort.C[0] + (A.X + (TYPE)0.5) * ViewPort.C[2];
   B.Y = ViewPort.C[1] + (A.Y + (TYPE)0.5) * ViewPort.C[3];
   B.Z = A.Z * (Depth.C[1] - Depth.C[0]) + (Depth.C[1] + Depth.C[0]) * (TYPE)0.5;
   B.W = 1;
   
   return B;
   }


/*---------------------------------------------------------------------------
   Type definitions
  ---------------------------------------------------------------------------*/
typedef matrix16<int8>   matrix16i8;
typedef matrix16<int16>  matrix16i16;
typedef matrix16<int32>  matrix16i32;
typedef matrix16<int64>  matrix16i64;
typedef matrix16<uint8>  matrix16u8;
typedef matrix16<uint16> matrix16u16;
typedef matrix16<uint32> matrix16u32;
typedef matrix16<uint64> matrix16u64;
typedef matrix16<char>   matrix16c;
typedef matrix16<int>    matrix16i;
typedef matrix16<uint>   matrix16u;
typedef matrix16<float>  matrix16f;
typedef matrix16<double> matrix16d;


/*---------------------------------------------------------------------------
   Casting definitions
  ---------------------------------------------------------------------------*/
#define cast_matrix16(TYPE, M) \
   matrix16<TYPE>((TYPE)(M).C[0x00], (TYPE)(M).C[0x01], (TYPE)(M).C[0x02], (TYPE)(M).C[0x03], \
                  (TYPE)(M).C[0x04], (TYPE)(M).C[0x05], (TYPE)(M).C[0x06], (TYPE)(M).C[0x07], \
                  (TYPE)(M).C[0x08], (TYPE)(M).C[0x09], (TYPE)(M).C[0x0A], (TYPE)(M).C[0x0B], \
                  (TYPE)(M).C[0x0C], (TYPE)(M).C[0x0D], (TYPE)(M).C[0x0E], (TYPE)(M).C[0x0F])


//Close namespaces
NAMESPACE_END(NAMESPACE_PROJECT)


//==== End of file ===========================================================
#endif
