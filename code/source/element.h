/*===========================================================================
   Elementary Index Types for Mesh Geometry

   Dominik Deak
  ===========================================================================*/

#ifndef ___ELEMENTS_H___
#define ___ELEMENTS_H___


/*---------------------------------------------------------------------------
   Header files
  ---------------------------------------------------------------------------*/
#include "common.h"
#include "vector.h"


//Namespaces
NAMESPACE_BEGIN(NAMESPACE_PROJECT)


/*---------------------------------------------------------------------------
  Elementary index stucture, used for defining faces, edges and points.
  ---------------------------------------------------------------------------*/
template <usize COUNT> struct Element
   {
   //---- Member data -----
   public:

   uint32 Index[COUNT];

   public:

   //---- Methods ----
   inline Element(void) {Clear();}
   inline ~Element(void) {}

   inline void Clear(void);
   inline uiter Get(uiter I);
   inline void Set(uiter I, uiter V);
   inline void Add(uiter I, uiter V);

   inline static usize Count(void) {return COUNT;}
   inline static usize Max(void) {return (usize)0x0FFFFFFFF;}
   };


/*---------------------------------------------------------------------------
   Clears structure.
  ---------------------------------------------------------------------------*/
template <usize COUNT> 
inline void Element<COUNT>::Clear(void)
   {
   for (uiter I = 0; I < COUNT; I++) 
      {
      Index[I] = 0;
      }
   }

/*---------------------------------------------------------------------------
   Gets an index entry, and checks for valid parameters.

   I : Index entry that is part of this element.
  ---------------------------------------------------------------------------*/
template <usize COUNT> 
inline uiter Element<COUNT>::Get(uiter I)
   {
   if (I >= COUNT) {throw dexception("Invalid array index.");}
   return (uiter)Index[I];
   }

/*---------------------------------------------------------------------------
   Sets an index entry, and checks for valid parameters.

   I : Index entry that is part of this element.
   V : Value that indetifies the vertex entry.
  ---------------------------------------------------------------------------*/
template <usize COUNT> 
inline void Element<COUNT>::Set(uiter I, uiter V)
   {
   if (I >= COUNT) {throw dexception("Invalid array index.");}
   if (V > Max()) {throw dexception("Value out of range.");}
   Index[I] = (uint32)V;
   }

/*---------------------------------------------------------------------------
   Adds a value to an index entry, and checks for valid parameters.

   I : Index entry that is part of this element.
   V : Value to add.
  ---------------------------------------------------------------------------*/
template <usize COUNT> 
inline void Element<COUNT>::Add(uiter I, uiter V)
   {
   if (I >= COUNT) {throw dexception("Invalid array index.");}
   
   V += (uiter)Index[I];
   if (V > Max()) {throw dexception("Value out of range.");}
   
   Index[I] = (uint32)V;
   }


/*---------------------------------------------------------------------------
   Type definitions
  ---------------------------------------------------------------------------*/
//Point indexing
typedef Element<1> Point1;

//Edge indexing
typedef Element<2> Edge2;

//Face indexing
typedef Element<3> Face3;
typedef Element<4> Face4;
typedef Element<5> Face5;
typedef Element<6> Face6;
typedef Element<7> Face7;
typedef Element<8> Face8;
typedef Element<9> Face9;


//Close namespaces
NAMESPACE_END(NAMESPACE_PROJECT)


//==== End of file ===========================================================
#endif
