/*===========================================================================
   Vertex Elementary Type for Mesh Geometry

   Dominik Deak
  ===========================================================================*/

#ifndef ___VERTEX_H___
#define ___VERTEX_H___


/*---------------------------------------------------------------------------
   Header files
  ---------------------------------------------------------------------------*/
#include "common.h"
#include "debug.h"
#include "vector.h"


//Namespaces
NAMESPACE_BEGIN(NAMESPACE_PROJECT)


/*---------------------------------------------------------------------------
  Vertex structure
  ---------------------------------------------------------------------------*/
template <typename TYPE, usize COUNT> struct Vertex
   {
   //---- Member data -----
   public:

   vector4<TYPE> Coord;                            //Vertex coordinate
   vector4<TYPE> Norm;                             //Vertex normal
   vector2<TYPE> Tex[COUNT];                       //Texture coordinates
   uint RefCount;                                  //Keeps count on the number of external elements referencing this vertex

   //---- Methods ----
   inline Vertex(void) {Clear();}
   inline ~Vertex(void) {}

   inline void Clear(void);
   inline void TexCoords(TYPE U = 0, TYPE V = 0);
   inline static usize TexCount(void) {return COUNT;}

   inline static usize OffsetCoord(void);
   inline static usize OffsetNorm(void);
   inline static usize OffsetTex(uiter I);
   inline static usize OffsetRefCount(void);
   };


/*---------------------------------------------------------------------------
   Returns the offset in bytes for various member structures.
  ---------------------------------------------------------------------------*/
//Vertex coordinate
template <typename TYPE, usize COUNT> 
inline usize Vertex<TYPE, COUNT>::OffsetCoord(void)
   {
   Vertex<TYPE, COUNT> Vert;
   return reinterpret_cast<uint8*>(&Vert.Coord.C0) - reinterpret_cast<uint8*>(&Vert);
   }

//Vertex normal
template <typename TYPE, usize COUNT> 
inline usize Vertex<TYPE, COUNT>::OffsetNorm(void)
   {
   Vertex<TYPE, COUNT> Vert;
   return reinterpret_cast<uint8*>(&Vert.Norm.C0) - reinterpret_cast<uint8*>(&Vert);
   }

//Texture coordinate
template <typename TYPE, usize COUNT> 
inline usize Vertex<TYPE, COUNT>::OffsetTex(uiter I)
   {
   if (I >= COUNT) {throw dexception("Texture coordinate index is out of range.");}
   Vertex<TYPE, COUNT> Vert;
   return (reinterpret_cast<uint8*>(&Vert.Tex[0].C0) - reinterpret_cast<uint8*>(&Vert)) + sizeof(Vert.Tex[0]) * I;
   }

//Reference counter
template <typename TYPE, usize COUNT> 
inline usize Vertex<TYPE, COUNT>::OffsetRefCount(void)
   {
   Vertex<TYPE, COUNT> Vert;
   return reinterpret_cast<uint8*>(&Vert.RefCount) - reinterpret_cast<uint8*>(&Vert);
   }

/*---------------------------------------------------------------------------
   Clears structure to default values.
  ---------------------------------------------------------------------------*/
template <typename TYPE, usize COUNT> 
inline void Vertex<TYPE, COUNT>::Clear(void)
   {
   Coord.X = 0;
   Coord.Y = 0;
   Coord.Z = 0;
   Coord.W = 1;

   Norm.X = 0;
   Norm.Y = 0;
   Norm.Z = 1;
   Norm.W = 0;
   
   TexCoords();
   
   RefCount = 0;
   }

/*---------------------------------------------------------------------------
   Set all texture coordinates to a specific value.
  ---------------------------------------------------------------------------*/
template <typename TYPE, usize COUNT> 
inline void Vertex<TYPE, COUNT>::TexCoords(TYPE U, TYPE V)
   {
   for (uiter I = 0; I < COUNT; I++) 
      {
      Tex[I].U = U;
      Tex[I].V = V;
      }
   }


/*---------------------------------------------------------------------------
   Type definitions
  ---------------------------------------------------------------------------*/
//Vertices
typedef Vertex<float, 1> Vertex1tf;
typedef Vertex<float, 2> Vertex2tf;
typedef Vertex<float, 4> Vertex4tf;
typedef Vertex<float, 8> Vertex8tf;

typedef Vertex<double, 1> Vertex1td;
typedef Vertex<double, 2> Vertex2td;
typedef Vertex<double, 4> Vertex4td;
typedef Vertex<double, 8> Vertex8td;


//Close namespaces
NAMESPACE_END(NAMESPACE_PROJECT)


//==== End of file ===========================================================
#endif
