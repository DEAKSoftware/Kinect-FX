/*===========================================================================
   Mutual Exclusion Classes for Concurrent Programming

   Dominik Deak
  ===========================================================================*/

#ifndef ___MUTEX_CPP___
#define ___MUTEX_CPP___


/*---------------------------------------------------------------------------
   Header files
  ---------------------------------------------------------------------------*/
#include "common.h"
#include "debug.h"
#include "mutex.h"


//Namespaces
NAMESPACE_BEGIN(NAMESPACE_PROJECT)


/*---------------------------------------------------------------------------
   Constructor.
  ---------------------------------------------------------------------------*/
MutexControl::MutexControl(MutexHandle &obj) : Handle(obj), Locked(false)
   {}

/*---------------------------------------------------------------------------
   Destructor.
  ---------------------------------------------------------------------------*/
MutexControl::~MutexControl(void)
   {
   Unlock();
   }

/*---------------------------------------------------------------------------
   Request a lock. Returns true if the request was successful. If the resource
   was already locked by another thread, the function returns false.
  ---------------------------------------------------------------------------*/
bool MutexControl::LockRequest(void) 
   {
   if (Locked) {return false;}
   Locked = Handle.tryLock();
   return Locked;
   }

/*---------------------------------------------------------------------------
   Waits for existing lock to be released and then locks the resource. This
   method basically blocks the execution of a thread until the resource is
   available. 
  ---------------------------------------------------------------------------*/
void MutexControl::Lock(void) 
   {
   if (Locked) {return;}
   Handle.lock();
   Locked = true;
   }

/*---------------------------------------------------------------------------
   Unlocks the resource.
  ---------------------------------------------------------------------------*/
void MutexControl::Unlock(void) 
   {
   if (!Locked) {return;}
   Handle.unlock();
   Locked = false;
   }


//Close namespaces
NAMESPACE_END(NAMESPACE_PROJECT)


//==== End of file ===========================================================
#endif
