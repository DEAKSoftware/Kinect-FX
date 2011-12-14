/*===========================================================================
   Kinect Interface

   Dominik Deak
  ===========================================================================*/

#ifndef ___KINECT_CPP___
#define ___KINECT_CPP___


/*---------------------------------------------------------------------------
   Header files
  ---------------------------------------------------------------------------*/
#include "common.h"
#include "debug.h"
#include "kinect.h"
#include "math.h"
#include "texture.h"


//Namespaces
NAMESPACE_BEGIN(NAMESPACE_PROJECT)


/*---------------------------------------------------------------------------
   Constructor.
  ---------------------------------------------------------------------------*/
Kinect::Kinect(Buffers &Buffer) : Buffer(Buffer)
   {
   Clear();

   DepthTable.Create(DepthTableSize);
   DepthTableSetup();

   freenect_init(&Context, nullptr);
   if (Context == nullptr) {throw dexception("freenect_init( ) failed.");}

   freenect_set_log_level(Context, FREENECT_LOG_WARNING);
   freenect_set_log_callback(Context, (freenect_log_cb)Kinect::HandlerLog);
   }

/*---------------------------------------------------------------------------
   Destructor.
  ---------------------------------------------------------------------------*/
Kinect::~Kinect(void)
   {
   Destroy();
   }

/*---------------------------------------------------------------------------
   Clears the structure.
  ---------------------------------------------------------------------------*/
void Kinect::Clear(void)
   {
   Context = nullptr;
   Device = nullptr;

   Error = false;

   VideoTime = 0;
   DepthTime = 0;

   RangeMet.Max = 10.0f; //Default to 10 metres
   RangeMet.Near = 0.0f;
   RangeMet.Far = RangeMet.Max;
   RangeMet.Clip = 1.0f;

   RangeRaw.Max = 1.0f;
   RangeRaw.Near = 0.0f;
   RangeRaw.Far = RangeRaw.Max;
   RangeRaw.Clip = 1.0f;

   ClipMode = Kinect::EraseBack;
   Linear = true;
   }

/*---------------------------------------------------------------------------
   Destroys the structure.
  ---------------------------------------------------------------------------*/
void Kinect::Destroy(void)
   {
   Close();

   if (Context != nullptr) {freenect_shutdown(Context);}

   DepthTable.Destroy();

   Clear();
   }

/*---------------------------------------------------------------------------
   Computes the depth conversion table from the Near, Far, Max and Clip
   member values. The formula for converting non-linear 11-bit depth values
   to metres is derived from the following references:
   http://nicolas.burrus.name/index.php/Research/KinectCalibration
   http://www.ros.org/wiki/kinect_node
   https://groups.google.com/group/openkinect/browse_thread/thread/31351846fd33c78/e98a94ac605b9f21
  ---------------------------------------------------------------------------*/
void Kinect::DepthTableSetup(void)
   {
   uint16* Dst = DepthTable.Pointer();

   DepthRange &Range = Linear ? RangeMet : RangeRaw;

   float Scale = Math::Reciprocal(Range.Far - Range.Near, Math::fmax);

   //Constants used for converting depth values to linear
   const float k1 = 3.260443197914426052995484940442f;
   const float k2 = 0.0029954659973903424287256471097779f;

   //Used for keeping depth values non-linear
   const float k3 = 1.0f / 2047.0f;

   switch (ClipMode)
      {
      case Kinect::EraseBack :
         {
         for (register uiter I = 0; I < DepthTable.Size(); I++)
            {
            register float Z = Linear ? Math::Reciprocal(Math::Max(k1 - k2 * (float)I, 0.0f), Math::fmax) : k3 * (float)I;
            Z = Math::Clamp((Z - Range.Near) * Scale, 0.0f, 1.0f);
            Dst[I] = (uint16)((Z < Range.Clip ? Z : 1.0f) * 65535.0f);
            }

         break;
         }

      case Kinect::EraseFront :
         {
         for (register uiter I = 0; I < DepthTable.Size(); I++)
            {
            register float Z = Linear ? Math::Reciprocal(Math::Max(k1 - k2 * (float)I, 0.0f), Math::fmax) : k3 * (float)I;
            Z = Math::Clamp((Z - Range.Near) * Scale, 0.0f, 1.0f);
            Dst[I] = (uint16)((Z >= Range.Clip ? Z : 1.0f) * 65535.0f);
            }

         break;
         }

      case Kinect::ClampBack :
         {
         for (register uiter I = 0; I < DepthTable.Size(); I++)
            {
            register float Z = Linear ? Math::Reciprocal(Math::Max(k1 - k2 * (float)I, 0.0f), Math::fmax) : k3 * (float)I;
            Z = Math::Clamp((Z - Range.Near) * Scale, 0.0f, 1.0f);
            Dst[I] = (uint16)((Z < Range.Clip ? Z : Range.Clip) * 65535.0f);
            }

         break;
         }

      case Kinect::ClampFront :
         {
         for (register uiter I = 0; I < DepthTable.Size(); I++)
            {
            register float Z = Linear ? Math::Reciprocal(Math::Max(k1 - k2 * (float)I, 0.0f), Math::fmax) : k3 * (float)I;
            Z = Math::Clamp((Z - Range.Near) * Scale, 0.0f, 1.0f);
            Dst[I] = (uint16)((Z >= Range.Clip ? Z : Range.Clip) * 65535.0f);
            }

         break;
         }

      default : throw dexception("Unknown clipping mode enumeration.");
      }
   }

/*---------------------------------------------------------------------------
   This function converts the raw 11-bit depth values (in 16-bit alignment)
   to linear values as specified by DepthTable. This should be called
   externally by a separate thread.
  ---------------------------------------------------------------------------*/
void Kinect::DepthPostProcess(void)
   {
   Texture &Depth = Buffer.GetDepth(Buffers::Front);

   MutexControl Mutex(Depth.GetMutexHandle());
   if (!Mutex.LockRequest()) {return;}

   uint16* Table = DepthTable.Pointer();
   uint16* Ptr = reinterpret_cast<uint16*>(Depth.Pointer());

   const usize Size = Depth.Size() / sizeof(uint16);

   for (uiter I = 0; I < Size; I++)
      {
      Ptr[I] = Table[Ptr[I] & 0x07FF];
      }

   Mutex.Unlock();
   }

/*---------------------------------------------------------------------------
   Depth range helper functions.
  ---------------------------------------------------------------------------*/
//Sets the upper limit of the depth range
void Kinect::SetMax(float Value)
   {
   DepthRange &Range = Linear ? RangeMet : RangeRaw;
   Range.Max = Math::Max(Value, 0.1f);
   Range.Near = Math::Clamp(Range.Near, 0.0f, Range.Far);
   Range.Far = Math::Clamp(Range.Far, Range.Near, Range.Max);
   DepthTableSetup();
   }

//Sets the near plane
void Kinect::SetNear(float Value)
   {
   DepthRange &Range = Linear ? RangeMet : RangeRaw;
   Range.Near = Math::Clamp(Value * Range.Max, 0.0f, Range.Far);
   DepthTableSetup();
   }

//Sets the far plane
void Kinect::SetFar(float Value)
   {
   DepthRange &Range = Linear ? RangeMet : RangeRaw;
   Range.Far = Math::Clamp(Value * Range.Max, Range.Near, Range.Max);
   DepthTableSetup();
   }

//Sets the clipping threshold
void Kinect::SetClip(float Value)
   {
   DepthRange &Range = Linear ? RangeMet : RangeRaw;
   Range.Clip = Math::Clamp(Value, 0.0f, 1.0f);
   DepthTableSetup();
   }

//Sets the clipping mode
void Kinect::SetClipMethod(ClippingMode Mode)
   {
   ClipMode = Mode;
   DepthTableSetup();
   }

//Toggles between linear and non-linear depth modes
void Kinect::SetLinear(bool State)
   {
   Linear = State;
   DepthTableSetup();
   }

/*---------------------------------------------------------------------------
   Checks whether there is a kinect device present and attempts to opent it.
   Returns true if the device is conntected, returns false otherwise.
  ---------------------------------------------------------------------------*/
bool Kinect::Open(void)
   {
   if (Context == nullptr) {return false;}
   if (Device != nullptr) {return true;}

   int Count = freenect_num_devices(Context);
   if (Count < 1) {return false;}

   if (freenect_open_device(Context, &Device, 0) != 0)
      {throw dexception("freenect_open_device( ) failed.");}

   freenect_set_user(Device, reinterpret_cast<void*>(this));

   if (freenect_get_user(Device) != reinterpret_cast<void*>(this))
      {throw dexception("freenect_get_user( ) failed.");}

   return true;
   }

/*---------------------------------------------------------------------------
   Closes the device.
  ---------------------------------------------------------------------------*/
void Kinect::Close(void)
   {
   StopVideo();
   StopDepth();
   SetLED(Kinect::LedOff);

   if (Device != nullptr)
      {
      #if !defined (KINECT_UNOFFICIAL)
         freenect_set_video_buffer(Device, nullptr);
         freenect_set_depth_buffer(Device, nullptr);
      #endif

      freenect_close_device(Device);

      Device = nullptr;
      }
   }

/*---------------------------------------------------------------------------
   Creates a video capture steam.
  ---------------------------------------------------------------------------*/
void Kinect::SetupVideo(VideoType Type)
   {
   switch (Type)
      {
      case Kinect::VideoRGB : SetupVideoRGB(); break;
      case Kinect::VideoBayer : SetupVideoBayer(); break;
      case Kinect::VideoIR : SetupVideoIR(); break;
      default : throw dexception("Unknown video type.");
      }
   }

/*---------------------------------------------------------------------------
   Creates a RGB video capture steam.
  ---------------------------------------------------------------------------*/
void Kinect::SetupVideoRGB(void)
   {
   if (Device == nullptr) {return;}

   Texture &VideoFront = Buffer.GetVideo(Buffers::Front);
   Texture &VideoBack = Buffer.GetVideo(Buffers::Back);

   MutexControl MutexFront(VideoFront.GetMutexHandle());
   MutexControl MutexBack(VideoBack.GetMutexHandle());
   MutexFront.Lock();
   MutexBack.Lock();

   VideoFront.Create(vector2u(FREENECT_FRAME_W, FREENECT_FRAME_H), Texture::TypeRGB);
   VideoBack.Create(vector2u(FREENECT_FRAME_W, FREENECT_FRAME_H), Texture::TypeRGB);
   VideoFront.ClearData();
   VideoBack.ClearData();

   if (VideoFront.Size() != FREENECT_VIDEO_RGB_SIZE) {throw dexception("Incorrect video texture size.");}
   if (VideoBack.Size() != FREENECT_VIDEO_RGB_SIZE) {throw dexception("Incorrect video texture size.");}

   if (freenect_set_video_format(Device, FREENECT_VIDEO_RGB) != 0)
      {throw dexception("freenect_set_video_format( ) failed.");}

   //Buffer the stream straight into the back texture
   #if !defined (KINECT_UNOFFICIAL)
   if (freenect_set_video_buffer(Device, reinterpret_cast<void*>(VideoBack.Pointer())) != 0)
      {throw dexception("freenect_set_video_buffer( ) failed.");}
   #endif

   freenect_set_video_callback(Device, Kinect::HandlerVideoRGB);
   }

/*---------------------------------------------------------------------------
   Creates a Bayer encoded video capture steam.
  ---------------------------------------------------------------------------*/
void Kinect::SetupVideoBayer(void)
   {
   if (Device == nullptr) {return;}

   Texture &VideoFront = Buffer.GetVideo(Buffers::Front);
   Texture &VideoBack = Buffer.GetVideo(Buffers::Back);

   MutexControl MutexFront(VideoFront.GetMutexHandle());
   MutexControl MutexBack(VideoBack.GetMutexHandle());
   MutexFront.Lock();
   MutexBack.Lock();

   VideoFront.Create(vector2u(FREENECT_FRAME_W, FREENECT_FRAME_H), Texture::TypeLum);
   VideoBack.Create(vector2u(FREENECT_FRAME_W, FREENECT_FRAME_H), Texture::TypeLum);
   VideoFront.ClearData();
   VideoBack.ClearData();

   if (VideoFront.Size() != FREENECT_VIDEO_BAYER_SIZE) {throw dexception("Incorrect video texture size.");}
   if (VideoBack.Size() != FREENECT_VIDEO_BAYER_SIZE) {throw dexception("Incorrect video texture size.");}

   if (freenect_set_video_format(Device, FREENECT_VIDEO_BAYER) != 0)
      {throw dexception("freenect_set_video_format( ) failed.");}

   //Buffer the stream straight into the back texture
   #if !defined (KINECT_UNOFFICIAL)
   if (freenect_set_video_buffer(Device, reinterpret_cast<void*>(VideoBack.Pointer())) != 0)
      {throw dexception("freenect_set_video_buffer( ) failed.");}
   #endif

   freenect_set_video_callback(Device, Kinect::HandlerVideoBayer);
   }

/*---------------------------------------------------------------------------
   Creates an infrared video capture steam.
  ---------------------------------------------------------------------------*/
void Kinect::SetupVideoIR(void)
   {
   if (Device == nullptr) {return;}

   Texture &VideoFront = Buffer.GetVideo(Buffers::Front);
   Texture &VideoBack = Buffer.GetVideo(Buffers::Back);

   MutexControl MutexFront(VideoFront.GetMutexHandle());
   MutexControl MutexBack(VideoBack.GetMutexHandle());
   MutexFront.Lock();
   MutexBack.Lock();

   VideoFront.Create(vector2u(FREENECT_IR_FRAME_W, FREENECT_IR_FRAME_H), Texture::TypeLum);
   VideoBack.Create(vector2u(FREENECT_IR_FRAME_W, FREENECT_IR_FRAME_H), Texture::TypeLum);
   VideoFront.ClearData();
   VideoBack.ClearData();

   if (VideoFront.Size() != FREENECT_VIDEO_IR_8BIT_SIZE) {throw dexception("Incorrect video texture size.");}
   if (VideoBack.Size() != FREENECT_VIDEO_IR_8BIT_SIZE) {throw dexception("Incorrect video texture size.");}

   if (freenect_set_video_format(Device, FREENECT_VIDEO_IR_8BIT) != 0)
      {throw dexception("freenect_set_video_format( ) failed.");}

   //Buffer the stream straight into the back texture
   #if !defined (KINECT_UNOFFICIAL)
   if (freenect_set_video_buffer(Device, reinterpret_cast<void*>(VideoBack.Pointer())) != 0)
      {throw dexception("freenect_set_video_buffer( ) failed.");}
   #endif

   freenect_set_video_callback(Device, Kinect::HandlerVideoIR);
   }

/*---------------------------------------------------------------------------
   Creates a depth buffer capture steam.
  ---------------------------------------------------------------------------*/
void Kinect::SetupDepth(void)
   {
   if (Device == nullptr) {return;}

   Texture &DepthFront = Buffer.GetDepth(Buffers::Front);
   Texture &DepthBack = Buffer.GetDepth(Buffers::Back);

   MutexControl MutexFront(DepthFront.GetMutexHandle());
   MutexControl MutexBack(DepthBack.GetMutexHandle());
   MutexFront.Lock();
   MutexBack.Lock();

   DepthFront.Create(vector2u(FREENECT_FRAME_W, FREENECT_FRAME_H), Texture::TypeDepth);
   DepthBack.Create(vector2u(FREENECT_FRAME_W, FREENECT_FRAME_H), Texture::TypeDepth);
   DepthFront.ClearData();
   DepthBack.ClearData();

   if (DepthFront.Size() != FREENECT_DEPTH_11BIT_SIZE) {throw dexception("Incorrect depth texture size.");}
   if (DepthBack.Size() != FREENECT_DEPTH_11BIT_SIZE) {throw dexception("Incorrect depth texture size.");}

   if (freenect_set_depth_format(Device, FREENECT_DEPTH_11BIT) != 0)
      {throw dexception("freenect_set_depth_format( ) failed.");}

   //Buffer the stream straight into the back texture
   #if !defined (KINECT_UNOFFICIAL)
   if (freenect_set_depth_buffer(Device, reinterpret_cast<void*>(DepthBack.Pointer())) != 0)
      {throw dexception("freenect_set_depth_buffer( ) failed.");}
   #endif

   freenect_set_depth_callback(Device, Kinect::HandlerDepth);
   }

/*---------------------------------------------------------------------------
   Start capturing streams.
  ---------------------------------------------------------------------------*/
void Kinect::StartVideo(void)
   {
   if (Device == nullptr) {return;}

   Texture &VideoFront = Buffer.GetVideo(Buffers::Front);
   Texture &VideoBack = Buffer.GetVideo(Buffers::Back);

   if (VideoFront.Size() > 0 && VideoBack.Size() > 0) {freenect_start_video(Device);}
   }

void Kinect::StartDepth(void)
   {
   if (Device == nullptr) {return;}

   Texture &DepthFront = Buffer.GetDepth(Buffers::Front);
   Texture &DepthBack = Buffer.GetDepth(Buffers::Back);

   if (DepthFront.Size() > 0 && DepthBack.Size() > 0) {freenect_start_depth(Device);}
   }

/*---------------------------------------------------------------------------
   Stop capturing streams.
  ---------------------------------------------------------------------------*/
void Kinect::StopVideo(void)
   {
   if (Device == nullptr) {return;}
   freenect_stop_video(Device);
   }

void Kinect::StopDepth(void)
   {
   if (Device == nullptr) {return;}
   freenect_stop_depth(Device);
   }

/*---------------------------------------------------------------------------
   Returns true if there is an active context and if the device is
   conntected, returns false otherwise.
  ---------------------------------------------------------------------------*/
bool Kinect::Connected(void)
   {
   return Context != nullptr && Device != nullptr;
   }

/*---------------------------------------------------------------------------
   Update function for the USB event processor. Returns true if the device is
   conntected, returns false otherwise. This method should be called
   periodically from a separate thread.
  ---------------------------------------------------------------------------*/
bool Kinect::Update(void)
   {
   if (Context == nullptr) {return false;}

   if (freenect_process_events(Context) != 0)
      {throw dexception("freenect_process_events( ) failed.");}

   return Device != nullptr;
   }

/*---------------------------------------------------------------------------
   Controls the LED.
  ---------------------------------------------------------------------------*/
void Kinect::SetLED(ModeLED Mode)
   {
   if (Device == nullptr) {return;}

   if (freenect_set_led(Device, static_cast<freenect_led_options>(Mode)) != 0)
      {throw dexception("freenect_set_led( ) failed.");}
   }

/*---------------------------------------------------------------------------
   Callback handler for logging.
  ---------------------------------------------------------------------------*/
void Kinect::HandlerLog(freenect_context* Context, freenect_loglevel Level, const char* Message, ...)
   {
   if (Context == nullptr || Message == nullptr) {return;}
   Debug::Log("Freenect (%d): %s", Level, Message);
   }

/*---------------------------------------------------------------------------
   Callback for capturing the RGB video buffer.
  ---------------------------------------------------------------------------*/
void Kinect::HandlerVideoRGB(freenect_device* Device, KINECT_VIDEO_TYPE* Buffer, uint32_t Time)
   {
   if (Device == nullptr || Buffer == nullptr) {return;}

   Kinect* obj = reinterpret_cast<Kinect*>(freenect_get_user(Device));
   if (obj == nullptr) {return;}

   #if defined (KINECT_UNOFFICIAL)

      Texture &VideoBack = obj->Buffer.GetVideo(Buffers::Back);
      MutexControl MutexBack(VideoBack.GetMutexHandle());
      if (!MutexBack.LockRequest()) {return;}

      uint8* Src = reinterpret_cast<uint8*>(Buffer);
      uint8* Dst = reinterpret_cast<uint8*>(VideoBack.Pointer());
      const usize Size = Math::Min(VideoBack.Size(), (usize)FREENECT_VIDEO_RGB_SIZE);
      for (uiter I = 0; I < Size; I++) {Dst[I] = Src[I];}

      MutexBack.Unlock();

   #endif

   if (!obj->Buffer.VideoSwap()) {return;}

   #if !defined (KINECT_UNOFFICIAL)

      Texture &VideoBack = obj->Buffer.GetVideo(Buffers::Back);
      MutexControl MutexBack(VideoBack.GetMutexHandle());
      if (!MutexBack.LockRequest()) {return;}

      if (freenect_set_video_buffer(Device, reinterpret_cast<void*>(VideoBack.Pointer())) != 0)
         {debug("freenect_set_video_buffer( ) failed."); obj->Error = true;}

      MutexBack.Unlock();

   #endif

   obj->VideoTime = (uint32)Time;
   }

/*---------------------------------------------------------------------------
   Callback for capturing the Bayer encoded video buffer.
  ---------------------------------------------------------------------------*/
void Kinect::HandlerVideoBayer(freenect_device* Device, KINECT_VIDEO_TYPE* Buffer, uint32_t Time)
   {
   if (Device == nullptr || Buffer == nullptr) {return;}

   Kinect* obj = reinterpret_cast<Kinect*>(freenect_get_user(Device));
   if (obj == nullptr) {return;}

   #if defined (KINECT_UNOFFICIAL)

      Texture &VideoBack = obj->Buffer.GetVideo(Buffers::Back);
      MutexControl MutexBack(VideoBack.GetMutexHandle());
      if (!MutexBack.LockRequest()) {return;}

      uint8* Src = reinterpret_cast<uint8*>(Buffer);
      uint8* Dst = reinterpret_cast<uint8*>(VideoBack.Pointer());
      const usize Size = Math::Min(VideoBack.Size(), (usize)FREENECT_VIDEO_BAYER_SIZE);
      for (uiter I = 0; I < Size; I++) {Dst[I] = Src[I];}

      MutexBack.Unlock();

   #endif

   if (!obj->Buffer.VideoSwap()) {return;}

   #if !defined (KINECT_UNOFFICIAL)

      Texture &VideoBack = obj->Buffer.GetVideo(Buffers::Back);
      MutexControl MutexBack(VideoBack.GetMutexHandle());
      if (!MutexBack.LockRequest()) {return;}

      if (freenect_set_video_buffer(Device, reinterpret_cast<void*>(VideoBack.Pointer())) != 0)
         {debug("freenect_set_video_buffer( ) failed."); obj->Error = true;}

      MutexBack.Unlock();

   #endif

   obj->VideoTime = (uint32)Time;
   }

/*---------------------------------------------------------------------------
   Callback for capturing the infrared video buffer.
  ---------------------------------------------------------------------------*/
void Kinect::HandlerVideoIR(freenect_device* Device, KINECT_VIDEO_TYPE* Buffer, uint32_t Time)
   {
   if (Device == nullptr || Buffer == nullptr) {return;}

   Kinect* obj = reinterpret_cast<Kinect*>(freenect_get_user(Device));
   if (obj == nullptr) {return;}

   #if defined (KINECT_UNOFFICIAL)

      Texture &VideoBack = obj->Buffer.GetVideo(Buffers::Back);
      MutexControl MutexBack(VideoBack.GetMutexHandle());
      if (!MutexBack.LockRequest()) {return;}

      uint8* Src = reinterpret_cast<uint8*>(Buffer);
      uint8* Dst = reinterpret_cast<uint8*>(VideoBack.Pointer());
      const usize Size = Math::Min(VideoBack.Size(), (usize)FREENECT_VIDEO_IR_8BIT_SIZE);
      for (uiter I = 0; I < Size; I++) {Dst[I] = Src[I];}

      MutexBack.Unlock();

   #endif

   if (!obj->Buffer.VideoSwap()) {return;}

   #if !defined (KINECT_UNOFFICIAL)

      Texture &VideoBack = obj->Buffer.GetVideo(Buffers::Back);
      MutexControl MutexBack(VideoBack.GetMutexHandle());
      if (!MutexBack.LockRequest()) {return;}

      if (freenect_set_video_buffer(Device, reinterpret_cast<void*>(VideoBack.Pointer())) != 0)
         {debug("freenect_set_video_buffer( ) failed."); obj->Error = true;}

      MutexBack.Unlock();

   #endif

   obj->VideoTime = (uint32)Time;
   }

/*---------------------------------------------------------------------------
   Callback for capturing the depth buffer.
  ---------------------------------------------------------------------------*/
void Kinect::HandlerDepth(freenect_device* Device, KINECT_DEPTH_TYPE* Buffer, uint32_t Time)
   {
   if (Device == nullptr || Buffer == nullptr) {return;}

   Kinect* obj = reinterpret_cast<Kinect*>(freenect_get_user(Device));
   if (obj == nullptr) {return;}

   #if defined (KINECT_UNOFFICIAL)

      Texture &DepthBack = obj->Buffer.GetDepth(Buffers::Back);
      MutexControl MutexBack(DepthBack.GetMutexHandle());
      if (!MutexBack.LockRequest()) {return;}

      uint16* Src = reinterpret_cast<uint16*>(Buffer);
      uint16* Dst = reinterpret_cast<uint16*>(DepthBack.Pointer());
      const usize Size = Math::Min(DepthBack.Size(), (usize)FREENECT_DEPTH_11BIT_SIZE) / sizeof(uint16);
      for (uiter I = 0; I < Size; I++) {Dst[I] = Src[I];}

      MutexBack.Unlock();

   #endif

   if (!obj->Buffer.DepthSwap()) {return;}

   #if !defined (KINECT_UNOFFICIAL)

      Texture &DepthBack = obj->Buffer.GetDepth(Buffers::Back);
      MutexControl MutexBack(DepthBack.GetMutexHandle());
      if (!MutexBack.LockRequest()) {return;}

      if (freenect_set_depth_buffer(Device, reinterpret_cast<void*>(DepthBack.Pointer())) != 0)
         {debug("freenect_set_depth_buffer( ) failed."); obj->Error = true;}

      MutexBack.Unlock();

   #endif

   obj->DepthPostProcess();

   obj->DepthTime = (uint32)Time;
   }


//Close namespaces
NAMESPACE_END(NAMESPACE_PROJECT)


//==== End of file ===========================================================
#endif
