/*===========================================================================
   Buffers Used for Capturing Video and Depth

   Dominik Deak
  ===========================================================================*/

#ifndef ___BUFFERS_CPP___
#define ___BUFFERS_CPP___


/*---------------------------------------------------------------------------
   Header files
  ---------------------------------------------------------------------------*/
#include "common.h"
#include "debug.h"
#include "buffers.h"


//Namespaces
NAMESPACE_BEGIN(NAMESPACE_PROJECT)


/*---------------------------------------------------------------------------
   Constructor.
  ---------------------------------------------------------------------------*/
Buffers::Buffers(void)
   {
   Clear();

   Video[0] = new Texture;
   Video[1] = new Texture;
   Depth[0] = new Texture;
   Depth[1] = new Texture;
   }

/*---------------------------------------------------------------------------
  Copy constructor, invoked when the current object is instantiated. This
  method performs a deep copy of the specified object. The current object is
  unitialised, which must be cleared.
  ---------------------------------------------------------------------------*/
Buffers::Buffers(const Buffers &obj) : MutexHandle()
   {
   Clear();

   Video[0] = (obj.Video[0] != nullptr) ? new Texture(*obj.Video[0]) : new Texture;
   Video[1] = (obj.Video[1] != nullptr) ? new Texture(*obj.Video[1]) : new Texture;
   Depth[0] = (obj.Depth[0] != nullptr) ? new Texture(*obj.Depth[0]) : new Texture;
   Depth[1] = (obj.Depth[1] != nullptr) ? new Texture(*obj.Depth[1]) : new Texture;
   
   VideoCount = obj.VideoCount;
   DepthCount = obj.DepthCount;
   }

/*---------------------------------------------------------------------------
  Assignment operator, invoked only when the current object already exist.
  This method performs a deep copy of the specified object. The current
  object may have allocated data which must be destroyed.
  ---------------------------------------------------------------------------*/
Buffers &Buffers::operator = (const Buffers &obj)
   {
   //No action on self assignment
   if (this == &obj) {return *this;}

   Destroy();

   Video[0] = (obj.Video[0] != nullptr) ? new Texture(*obj.Video[0]) : new Texture;
   Video[1] = (obj.Video[1] != nullptr) ? new Texture(*obj.Video[1]) : new Texture;
   Depth[0] = (obj.Depth[0] != nullptr) ? new Texture(*obj.Depth[0]) : new Texture;
   Depth[1] = (obj.Depth[1] != nullptr) ? new Texture(*obj.Depth[1]) : new Texture;

   VideoCount = obj.VideoCount;
   DepthCount = obj.DepthCount;

   return *this;
   }

/*---------------------------------------------------------------------------
   Destructor.
  ---------------------------------------------------------------------------*/
Buffers::~Buffers(void)
   {
   Destroy();
   }

/*---------------------------------------------------------------------------
   Clears the structure.
  ---------------------------------------------------------------------------*/
void Buffers::Clear(void)
   {
   Video[0] = nullptr;
   Video[1] = nullptr;
   Depth[0] = nullptr;
   Depth[1] = nullptr;
   
   VideoCount = 0;
   DepthCount = 0;
   }

/*---------------------------------------------------------------------------
   Destroys the structure.
  ---------------------------------------------------------------------------*/
void Buffers::Destroy(void)
   {
   delete Video[0];
   delete Video[1];
   delete Depth[0];
   delete Depth[1];

   Clear();
   }

/*---------------------------------------------------------------------------
   Swaps the buffers, but only if the back and front buffers are unlocked.
   These functions also signal the update flag for each repsective buffer.
   Returns true if the swap was successful.
  ---------------------------------------------------------------------------*/
//Swap video buffer
bool Buffers::VideoSwap(void)
   {
   MutexControl Mutex(GetMutexHandle());
   MutexControl Mutex0(Video[0]->GetMutexHandle());
   MutexControl Mutex1(Video[1]->GetMutexHandle());

   if (!Mutex.LockRequest()) {return false;}
   if (!Mutex0.LockRequest()) {return false;}
   if (!Mutex1.LockRequest()) {return false;}

   Math::Swap(Video[0], Video[1]);

   VideoCount++;

   return true;
   }

//Swap depth buffer
bool Buffers::DepthSwap(void)
   {
   MutexControl Mutex(GetMutexHandle());
   MutexControl Mutex0(Depth[0]->GetMutexHandle());
   MutexControl Mutex1(Depth[1]->GetMutexHandle());

   if (!Mutex.LockRequest()) {return false;}
   if (!Mutex0.LockRequest()) {return false;}
   if (!Mutex1.LockRequest()) {return false;}

   Math::Swap(Depth[0], Depth[1]);

   DepthCount++;

   return true;
   }

/*---------------------------------------------------------------------------
   Each function accepts an ID value and returns true if the video (or the 
   depth) buffer was updated since the last ID test. The ID parameter will be 
   also assigned with a new value. If the Buffer class is currently locked, 
   the function returns false and the ID parameter will be unaffected.
  ---------------------------------------------------------------------------*/
//Test for video update
bool Buffers::VideoUpdated(uiter &ID)
   {
   MutexControl Mutex(GetMutexHandle());
   if (!Mutex.LockRequest()) {return false;}

   if (VideoCount == ID) {return false;}

   ID = VideoCount;

   return true;
   }

//Test for depth update
bool Buffers::DepthUpdated(uiter &ID)
   {
   MutexControl Mutex(GetMutexHandle());
   if (!Mutex.LockRequest()) {return false;}

   if (DepthCount == ID) {return false;}

   ID = DepthCount;

   return true;
   }

/*---------------------------------------------------------------------------
   Return selected texture buffer. Selects the front texture by default.
  ---------------------------------------------------------------------------*/
Texture &Buffers::GetVideo(Select I) 
   {
   if (I > 1) {throw dexception("Selection index out of range.");}
   return *Video[I];
   }

Texture &Buffers::GetDepth(Select I) 
   {
   if (I > 1) {throw dexception("Selection index out of range.");}
   return *Depth[I];
   }

/*---------------------------------------------------------------------------
   Returns selected texture resolution. Selects the front texture by default.
  ---------------------------------------------------------------------------*/
vector2u Buffers::GetVideoResolution(Select I) const 
   {
   if (I > 1) {throw dexception("Selection index out of range.");}
   return Video[I]->Resolution();
   }

vector2u Buffers::GetDepthResolution(Select I) const 
   {
   if (I > 1) {throw dexception("Selection index out of range.");}
   return Depth[I]->Resolution();
   }

/*---------------------------------------------------------------------------
   Returns selected texture data type. Selects the front texture by default.
  ---------------------------------------------------------------------------*/
Texture::TexType Buffers::GetVideoDataType(Select I) const
   {
   if (I > 1) {throw dexception("Selection index out of range.");}
   return Video[I]->DataType();
   }

Texture::TexType Buffers::GetDepthDataType(Select I) const
   {
   if (I > 1) {throw dexception("Selection index out of range.");}
   return Depth[I]->DataType();
   }


//Close namespaces
NAMESPACE_END(NAMESPACE_PROJECT)


//==== End of file ===========================================================
#endif
