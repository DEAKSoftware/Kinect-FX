/*===========================================================================
   Kinect Interface

   Dominik Deak
  ===========================================================================*/

#ifndef ___KINECT_H___
#define ___KINECT_H___


/*---------------------------------------------------------------------------
   Header files
  ---------------------------------------------------------------------------*/
#include "buffers.h"
#include "common.h"
#include "texture.h"
#include "vector.h"


//Namespaces
NAMESPACE_BEGIN(NAMESPACE_PROJECT)


/*---------------------------------------------------------------------------
  Definitions
  ---------------------------------------------------------------------------*/
//#define KINECT_UNOFFICIAL                          //Enable this to use Zephod's WIN32 kinect driver library

#if defined (KINECT_UNOFFICIAL)
   #define FREENECT_DEPTH_11BIT        FREENECT_FORMAT_11_BIT
   #define FREENECT_DEPTH_11BIT_SIZE   FREENECT_DEPTH_SIZE
   #define FREENECT_IR_FRAME_H         FREENECT_FRAME_H
   #define FREENECT_IR_FRAME_W         FREENECT_FRAME_W
   #define FREENECT_VIDEO_BAYER        FREENECT_FORMAT_BAYER
   #define FREENECT_VIDEO_BAYER_SIZE   FREENECT_BAYER_SIZE
   #define FREENECT_VIDEO_IR_8BIT      FREENECT_FORMAT_BAYER
   #define FREENECT_VIDEO_IR_8BIT_SIZE FREENECT_BAYER_SIZE
   #define FREENECT_VIDEO_RGB          FREENECT_FORMAT_RGB
   #define FREENECT_VIDEO_RGB_SIZE     FREENECT_RGB_SIZE
   #define freenect_set_video_callback freenect_set_rgb_callback
   #define freenect_set_video_format   freenect_set_rgb_format
   #define freenect_start_video        freenect_start_rgb
   #define freenect_stop_video         freenect_stop_rgb
   #define freenect_video_format       freenect_rgb_format
   #define KINECT_VIDEO_TYPE           freenect_pixel
   #define KINECT_DEPTH_TYPE           freenect_depth
#else
   #define KINECT_VIDEO_TYPE           void
   #define KINECT_DEPTH_TYPE           void
#endif


/*---------------------------------------------------------------------------
  Classes
  ---------------------------------------------------------------------------*/
class Kinect
   {
   //---- Constants and definitions ----
   public:

   typedef struct DepthRange
      {
      float Near;                                  //Near plane position
      float Far;                                   //Far plane position
      float Max;                                   //Maximum range of the clipping planes
      float Clip;                                  //Clipper position
      };

   enum ModeLED                                    //LED control options
      {
      LedOff = LED_OFF,                            //Turn off LED
      LedGreen = LED_GREEN,                        //Set LED to green
      LedRed = LED_RED,                            //Set LED to red
      LedYellow = LED_YELLOW,                      //Set LED to yellow
      LedBlinkGreen = LED_BLINK_GREEN,             //Set LED to blinking green
      LedBlinkRedYellow = LED_BLINK_RED_YELLOW     //Set LED to blinking red and yellow
      };

   enum ClippingMode                               //Depth clipping mode
      {
      EraseBack = 0,                               //Erase values below threshold
      EraseFront = 1,                              //Erase values above threshold
      ClampBack = 2,                               //Clamp values below threshold
      ClampFront = 3                               //Clamp values above threshold
      };

   enum VideoType                                  //Type of video buffer to fetch
      {
      VideoRGB = 1,                                //Decoded RGB
      VideoBayer = 2,                              //Bayer encoding
      VideoIR = 3                                  //Infrared data
      };

   static const usize DepthTableSize = 2048;       //Corresponds to maximum 11-bit value

   //---- Member data ----
   protected:

	freenect_context* Context;                      //Freenect context
	freenect_device* Device;                        //Freenect device
   
   bool Error;                                     //Error flag returned by the callback functions

   Buffers &Buffer;                                //Video and depth buffers
   
   uint32 VideoTime;                               //Time step of the video frame
   uint32 DepthTime;                               //Time step of the depth frame

   Array<uint16, 8> DepthTable;                    //Depth look-up table for computing linear and normalised depth values
   ClippingMode ClipMode;                          //Clipping behaviour of the depth buffer

   DepthRange RangeMet;                            //Metric depth scale 
   DepthRange RangeRaw;                            //Raw depth scale 
   bool Linear;                                    //Depth range is transformed to linear range

   //---- Methods ----
   public:

   Kinect(Buffers &Buffer);
   ~Kinect(void);

   private:

   Kinect(const Kinect &obj);                      //Disable
   Kinect &operator = (const Kinect &obj);         //Disable

   //Data allocation
   void Clear(void);
   void Destroy(void);

   protected:

   void DepthTableSetup(void);
   void DepthPostProcess(void);

   public:

   //Interface setup
   bool Open(void);
   void Close(void);
   void SetupVideo(VideoType Type = Kinect::VideoRGB);
   void SetupVideoRGB(void);
   void SetupVideoBayer(void);
   void SetupVideoIR(void);
   void SetupDepth(void);
   
   //Interface control
   void StartVideo(void);
   void StartDepth(void);
   void StopVideo(void);
   void StopDepth(void);
   bool Connected(void);
   bool Update(void);
   void SetLED(ModeLED Mode = Kinect::LedOff);

   //Depth buffer control
   void SetMax(float Value);
   void SetNear(float Value);
   void SetFar(float Value);
   void SetClip(float Value);
   void SetClipMethod(ClippingMode Mode = Kinect::EraseBack);
   void SetLinear(bool State);
   inline float GetMax(void) {return Linear ? RangeMet.Max : RangeRaw.Max;}
   inline float GetNear(void) {return Linear ? RangeMet.Near : RangeRaw.Near;}
   inline float GetFar(void) {return Linear ? RangeMet.Far : RangeRaw.Far;}
   
   private:

   static void HandlerLog(freenect_context* Context, freenect_loglevel Level, const char* Message, ...);
   static void HandlerVideoRGB(freenect_device* Device, KINECT_VIDEO_TYPE* Buffer, uint32_t Time);
   static void HandlerVideoBayer(freenect_device* Device, KINECT_VIDEO_TYPE* Buffer, uint32_t Time);
   static void HandlerVideoIR(freenect_device* Device, KINECT_VIDEO_TYPE* Buffer, uint32_t Time);
   static void HandlerDepth(freenect_device* Device, KINECT_DEPTH_TYPE* Buffer, uint32_t Time);
   };


//Close namespaces
NAMESPACE_END(NAMESPACE_PROJECT)


//==== End of file ===========================================================
#endif
