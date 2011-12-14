/*===========================================================================
   Array Template Class

   Dominik Deak
  ===========================================================================*/

#ifndef ___ARRAY_H___
#define ___ARRAY_H___


/*---------------------------------------------------------------------------
   Header files
  ---------------------------------------------------------------------------*/
#include "common.h"
#include "debug.h"


//Namespaces
NAMESPACE_BEGIN(NAMESPACE_PROJECT)


/*---------------------------------------------------------------------------
  The array class. The GRAN option sets the allocation granularity size.
  ---------------------------------------------------------------------------*/
template <typename TYPE, usize GRAN = 1> class Array
   {
   //---- Member data ----
   private:

   TYPE* Items;
   usize Count;
   usize Total;

   //---- Methods ----
   public:

   //Constructor, assigment, and destructor 
   inline Array(void);
   inline Array(const Array<TYPE, GRAN> &A);
   inline Array<TYPE, GRAN> &operator = (const Array<TYPE, GRAN> &A);
   inline ~Array(void);

   //Data allocation
   inline void Clear(void);
   inline void Destroy(void);
   inline void Reserve(usize Extra);
   inline void Create(usize Extra);
   inline void Compact(void);

   //Adding and removing items
   inline void  operator += (const Array<TYPE, GRAN> &A);
   inline void  operator += (const TYPE &Item);
   inline uiter operator +  (const TYPE &Item);
   inline void Remove(uiter I);

   //Data access
   inline TYPE  operator [] (uiter I) const;
   inline TYPE& operator [] (uiter I);  
   inline TYPE* Pointer(void) const;
   inline usize Size(void) const;

   private:
   
   //Data allocation
   inline void Resize(usize T);
   };


/*---------------------------------------------------------------------------
  Default constructor.
  ---------------------------------------------------------------------------*/
template <typename TYPE, usize GRAN> 
inline Array<TYPE, GRAN>::Array(void) 
   {
   Clear();
   }

/*---------------------------------------------------------------------------
  Copy constructor, invoked when the current object is instantiated. This
  method performs a deep copy of the specified object. The current object is 
  unitialised, which must be cleared.
  ---------------------------------------------------------------------------*/
template <typename TYPE, usize GRAN> 
inline Array<TYPE, GRAN>::Array(const Array<TYPE, GRAN> &A)
   {
   Clear();
   Reserve(A.Size());

   for (uiter I = 0; I < A.Size(); I++)
      {
      *this += A[I];
      }
   }

/*---------------------------------------------------------------------------
  Assignment operator, invoked only when the current object already exist.
  This method performs a deep copy of the specified object. The current 
  object may have allocated data which must be destroyed.
  ---------------------------------------------------------------------------*/
template <typename TYPE, usize GRAN> 
inline Array<TYPE, GRAN> &Array<TYPE, GRAN>::operator = (const Array<TYPE, GRAN> &A)
   {
   //No action on self assignment
   if (this == &A) {return *this;}

   Destroy();
   Reserve(A.Size());

   for (uiter I = 0; I < A.Size(); I++)
      {
      *this += A[I];
      }
   
   return *this;
   }

/*---------------------------------------------------------------------------
  Destructor.
  ---------------------------------------------------------------------------*/
template <typename TYPE, usize GRAN> 
inline Array<TYPE, GRAN>::~Array(void)
   {
   Destroy();
   }

/*---------------------------------------------------------------------------
   Clears the array contets.
  ---------------------------------------------------------------------------*/
template <typename TYPE, usize GRAN> 
inline void Array<TYPE, GRAN>::Clear(void)
   {
   Items = nullptr;
   Count = 0;
   Total = 0;
   }

/*---------------------------------------------------------------------------
   Destroys array contents.
  ---------------------------------------------------------------------------*/
template <typename TYPE, usize GRAN> 
inline void Array<TYPE, GRAN>::Destroy(void)
   {
   delete[] Items;
   Clear();
   }

/*---------------------------------------------------------------------------
  Resizes array according to the new total.
  ---------------------------------------------------------------------------*/
template <typename TYPE, usize GRAN> 
inline void Array<TYPE, GRAN>::Resize(usize T)
   {
   if (T == Total) {return;}

   if (T < Count) {throw dexception("Invalid parameters.");}

   if (T < 1) {Destroy();}

   TYPE* New = new TYPE[T];
      
   Total = T;

   for (uiter I = 0; I < Count; I++)
      {
      New[I] = Items[I];
      }

   delete[] Items;
   Items = New;
   }

/*---------------------------------------------------------------------------
   Reserves some extra space on the array, on top of the existing Count.
  ---------------------------------------------------------------------------*/
template <typename TYPE, usize GRAN> 
inline void Array<TYPE, GRAN>::Reserve(usize Extra)
   {
   if (Extra < 1) {return;}
   
   Extra += Count;
   
   usize Rem = Extra % GRAN;
   if (Rem > 0) {Extra += GRAN - Rem;}
   
   Resize(Extra);
   }

/*---------------------------------------------------------------------------
   Creates extra entries on the array, on top of the existing Count. The
   new entries will be in an unitialised state, depending on the entry object
   type.
  ---------------------------------------------------------------------------*/
template <typename TYPE, usize GRAN> 
inline void Array<TYPE, GRAN>::Create(usize Extra)
   {
   if (Extra < 1) {return;}

   Reserve(Extra);
   
   Count += Extra;
   }

/*---------------------------------------------------------------------------
   Forces the array to be compacted so that Total matches Count. Useful for
   finalising the array when no more items are added.
  ---------------------------------------------------------------------------*/
template <typename TYPE, usize GRAN> 
inline void Array<TYPE, GRAN>::Compact(void)
   {
   if (Total > Count)
      {
      Resize(Count);
      }
   }

/*---------------------------------------------------------------------------
   Appends an external array to this array.
  ---------------------------------------------------------------------------*/
template <typename TYPE, usize GRAN> 
inline void Array<TYPE, GRAN>::operator += (const Array<TYPE, GRAN> &A)
   {
   if (A.Size() < 1) {return;}

   Reserve(A.Size());

   //NOTE: Make a local copy of the external array size, 
   // just in case the array is appending itself
   usize Size = A.Size(); 
   for (uiter I = 0; I < Size; I++)
      {
      *this += A[I];
      }
   }

/*---------------------------------------------------------------------------
   Appends new item at the end of the array. The method will grow the array 
   if necessary, which can be an expensive process for large arrays. In that
   case, it is better to preallocate extra space via the Reserve( ) method
   before using this function.
  ---------------------------------------------------------------------------*/
template <typename TYPE, usize GRAN> 
inline void Array<TYPE, GRAN>::operator += (const TYPE &Item)
   {
   if (Total <= Count) 
      {
      Resize(Total + GRAN);
      }

   Items[Count++] = Item;
   }

/*---------------------------------------------------------------------------
   Appends new item at the end of the array and returns its array index.
   The method will grow the array if necessary, which can be an expensive
   process for large arrays. In that case, it is better to preallocate extra 
   space via the Reserve( ) method before using this function.
  ---------------------------------------------------------------------------*/
template <typename TYPE, usize GRAN> 
inline uiter Array<TYPE, GRAN>::operator + (const TYPE &Item)
   {
   if (Total <= Count) 
      {
      Resize(Total + GRAN);
      }

   uiter I = Count;
   
   Items[Count++] = Item;
   
   return I;
   }

/*---------------------------------------------------------------------------
   Removes item at the specified index and then it compacts array. The method 
   will compact the array if necessary. Array compaction is an expensive 
   process for large arrays, hence use this method sparingly.
  ---------------------------------------------------------------------------*/
template <typename TYPE, usize GRAN> 
inline void Array<TYPE, GRAN>::Remove(uiter I)
   {
   if (I >= Count) {return;}
      
   for (uiter J = I + 1; J < Count; I++, J++)
      {
      Items[I] = Items[J];
      }
      
   Count--;
      
   if (Total - Count >= GRAN) 
      {
      Resize(Total - GRAN);
      }
   }

/*---------------------------------------------------------------------------
   Operator for returning entries.
  ---------------------------------------------------------------------------*/
template <typename TYPE, usize GRAN> 
inline TYPE Array<TYPE, GRAN>::operator [] (uiter I) const
   {
   if (I >= Count) {throw dexception("Invalid array index.");}
   return Items[I];
   }

/*---------------------------------------------------------------------------
   Operator for modifying entries by reference.
  ---------------------------------------------------------------------------*/
template <typename TYPE, usize GRAN> 
inline TYPE& Array<TYPE, GRAN>::operator [] (uiter I)
   {
   if (I >= Count) {throw dexception("Invalid array index.");}
   return Items[I];
   }

/*---------------------------------------------------------------------------
   Returns the base pointer of the array. Only used in special circumstances,
   such as buffering arrays in OpenGL, and so on. For general purpose array
   access, use the operator[] method instead.
  ---------------------------------------------------------------------------*/
template <typename TYPE, usize GRAN> 
inline TYPE* Array<TYPE, GRAN>::Pointer(void) const 
   {
   return Items;
   }

/*---------------------------------------------------------------------------
   Returns the number of entries in the array.
  ---------------------------------------------------------------------------*/
template <typename TYPE, usize GRAN> 
inline usize Array<TYPE, GRAN>::Size(void) const 
   {
   return Count;
   }


//Close namespaces
NAMESPACE_END(NAMESPACE_PROJECT)


//==== End of file ===========================================================
#endif
