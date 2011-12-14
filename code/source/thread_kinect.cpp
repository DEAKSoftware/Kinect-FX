/*===========================================================================
   Thread That Manages the Kinect Device

   Dominik Deak
  ===========================================================================*/

#ifndef ___THREAD_KINECT_CPP___
#define ___THREAD_KINECT_CPP___


/*---------------------------------------------------------------------------
   Header files
  ---------------------------------------------------------------------------*/
#include "common.h"
#include "debug.h"
#include "thread_kinect.h"


/*---------------------------------------------------------------------------
   Constructor. Accepts a pointer to the partent object.
  ---------------------------------------------------------------------------*/
KinectThread::KinectThread(QObject* Parent, QObject* WidgetVideo, QObject* WidgetDepth, NAMESPACE_PROJECT::Buffers &Buffer) : QThread(Parent), Kinect(Buffer)
   {
   Clear();

   if (Parent == nullptr || WidgetVideo == nullptr || WidgetDepth == nullptr) 
      {throw dexception("Invalid parameters.");}

   setTerminationEnabled(true);

   //Hook signal functions to the parent class' slot functions
   QObject::connect(this, SIGNAL(SignalConnected(bool)), Parent, SLOT(DeviceConnected(bool)));
   QObject::connect(this, SIGNAL(SignalUpdate(void)), WidgetVideo, SLOT(update(void)));
   QObject::connect(this, SIGNAL(SignalUpdate(void)), WidgetDepth, SLOT(update(void)));
   QObject::connect(this, SIGNAL(SignalError(QString)), Parent, SLOT(DeviceError(QString)));
   }

/*---------------------------------------------------------------------------
   Destructor.
  ---------------------------------------------------------------------------*/
KinectThread::~KinectThread(void)
   {
   Destroy();
   }

/*---------------------------------------------------------------------------
   Clears the structure.
  ---------------------------------------------------------------------------*/
void KinectThread::Clear(void)
   {
   Exit = true;
   }

/*---------------------------------------------------------------------------
   Destroys the structure.
  ---------------------------------------------------------------------------*/
void KinectThread::Destroy(void)
   {
   Clear();
   }

/*---------------------------------------------------------------------------
   Sets the video capture mode.
  ---------------------------------------------------------------------------*/
void KinectThread::ChangeVideo(VideoType Type)
   {
   try {
      StopVideo();
      SetupVideo(Type);
      StartVideo();
      }

   catch (std::exception &e) 
      {
      SignalError(e.what());
      Exit = true;
      }
   
   catch (...) 
      {
      SignalError("Trapped an unhandled exception in the Kinect device thread.");
      Exit = true;
      }
   }

/*---------------------------------------------------------------------------
   Thread entry point.
  ---------------------------------------------------------------------------*/
void KinectThread::run(void)
   {
   debug("Started Kinect thread.\n");

   Exit = Error;

   try {
      NAMESPACE_PROJECT::uiter VideoUpdateID = ~0U;
      NAMESPACE_PROJECT::uiter DepthUpdateID = ~0U;

      while (!Exit)
         {
         //Poll to open a new device
         while (!Exit && !Open()) 
            {
            yieldCurrentThread();
            msleep(TimeDetect);
            Exit |= Error;
            }

         if (Exit) {break;}

         SetupVideo();
         SetupDepth();

         //StartVideo();
         //StartDepth();

         SetLED(Kinect::LedGreen);
         SignalConnected(true);

         //Capture
         while (!Exit && Update())
            {
            bool VideoSignal = Buffer.VideoUpdated(VideoUpdateID);
            bool DepthSignal = Buffer.DepthUpdated(DepthUpdateID);
            
            if (VideoSignal || DepthSignal) 
               {
               SignalUpdate();
               }
         
            Exit |= Error;
            }

         Close();

         SignalConnected(false);
         }
      }
   
   catch (std::exception &e) 
      {
      SignalError(e.what());
      Exit = true;
      }
   
   catch (...) 
      {
      SignalError("Trapped an unhandled exception in the Kinect device thread.");
      Exit = true;
      }

   debug("Stopping Kinect thread.\n");
   }

/*---------------------------------------------------------------------------
   Stops the thread.
  ---------------------------------------------------------------------------*/
void KinectThread::stop(void)
   {
   Exit = true;
   }


//==== End of file ===========================================================
#endif
