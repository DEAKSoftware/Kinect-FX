/*===========================================================================
   Mutual Exclusion Classes for Concurrent Programming

   Dominik Deak
  ===========================================================================*/

#ifndef ___MUTEX_H___
#define ___MUTEX_H___


/*---------------------------------------------------------------------------
   Header files
  ---------------------------------------------------------------------------*/
#include "common.h"


//Namespaces
NAMESPACE_BEGIN(NAMESPACE_PROJECT)


/*---------------------------------------------------------------------------
   Mutex handle class.
  ---------------------------------------------------------------------------*/
class MutexHandle : private QMutex
   {
   friend class MutexControl;

   //---- Member data ----
   private:

   //---- Methods ----
   public:

   inline MutexHandle& GetMutexHandle(void) {return *this;}
   };


/*---------------------------------------------------------------------------
   Mutex manipulation class. This object will use its destructor to 
   automatically unlock the mutex handle when it falls out of scope.
  ---------------------------------------------------------------------------*/
class MutexControl
   {
   //---- Member data ----
   private:

   MutexHandle &Handle;
   bool Locked;

   //---- Methods ----
   public:

   MutexControl(MutexHandle &obj);
   ~MutexControl(void);

   private:

   MutexControl(const MutexControl &obj);          //Disable
   MutexControl &operator = (const MutexControl &obj); //Disable

   public:
   
   bool LockRequest(void);
   void Lock(void);
   void Unlock(void);
   };


//Close namespaces
NAMESPACE_END(NAMESPACE_PROJECT)


//==== End of file ===========================================================
#endif
