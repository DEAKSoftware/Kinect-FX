/*===========================================================================
   Buffers Used for Capturing Video and Depth

   Dominik Deak
  ===========================================================================*/

#ifndef ___BUFFERS_H___
#define ___BUFFERS_H___


/*---------------------------------------------------------------------------
   Header files
  ---------------------------------------------------------------------------*/
#include "common.h"
#include "texture.h"


//Namespaces
NAMESPACE_BEGIN(NAMESPACE_PROJECT)


/*---------------------------------------------------------------------------
   Classes
  ---------------------------------------------------------------------------*/
class Buffers : public MutexHandle
   {
   //---- Constants and definitions ----
   public:

   enum Select                                     //Enumeration for buffer texture selection 
      {
      Front = 0,                                   //Select the front buffer
      Back = 1                                     //Select the back buffer
      };

   //---- Member data ----
   private:

   Texture* Video[2];                              //Front and back buffer video texture
   Texture* Depth[2];                              //Front and back buffer depth texture
   
   uiter VideoCount;                               //Counts video updates, used for signalling
   uiter DepthCount;                               //Counts depth updates, used for signalling

   //---- Methods ----
   public:

   Buffers(void);
   Buffers(const Buffers &obj);
   Buffers &operator = (const Buffers &obj);
   ~Buffers(void);

   private:

   //Data allocation
   void Clear(void);
   void Destroy(void);

   public:

   //Buffer control and signalling
   bool VideoSwap(void);
   bool DepthSwap(void);
   bool VideoUpdated(uiter &ID);
   bool DepthUpdated(uiter &ID);

   //Data access
   Texture &GetVideo(Select I = Buffers::Front);
   Texture &GetDepth(Select I = Buffers::Front);
   vector2u GetVideoResolution(Select I = Buffers::Front) const;
   vector2u GetDepthResolution(Select I = Buffers::Front) const;
   Texture::TexType GetVideoDataType(Select I = Buffers::Front) const;
   Texture::TexType GetDepthDataType(Select I = Buffers::Front) const;
   inline uiter GetVideoCounter(void) const {return VideoCount;}
   inline uiter GetDepthCounter(void) const {return DepthCount;}
   };


//Close namespaces
NAMESPACE_END(NAMESPACE_PROJECT)


//==== End of file ===========================================================
#endif
