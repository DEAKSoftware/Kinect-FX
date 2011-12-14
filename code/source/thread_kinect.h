/*===========================================================================
   Thread That Manages the Kinect Device

   Dominik Deak

   Note: The Meta-Object Compiler must be run on this header to generate 
   the required C++ file to allow interfacing between objects.
  ===========================================================================*/

#ifndef ___THREAD_KINECT_H___
#define ___THREAD_KINECT_H___


/*---------------------------------------------------------------------------
   Header files
  ---------------------------------------------------------------------------*/
#include "buffers.h"
#include "common.h"
#include "kinect.h"


/*---------------------------------------------------------------------------
   The device thread class basically extends the kinect interface into a
   threaded object. 
  ---------------------------------------------------------------------------*/
class KinectThread : public QThread, public NAMESPACE_PROJECT::Kinect
   {
   //---- Qt specific ----
   Q_OBJECT

   //---- Constants and definitions ----
   public:

   static const uint TimeDetect = 500;             //Sleep interval while detecting the device, in ms

   //---- Member data ----
   private:

   bool Exit;                                      //Flag that signals to exit thread

   //---- Methods ----
   public:

   KinectThread(QObject* Parent, QObject* WidgetVideo, QObject* WidgetDepth, NAMESPACE_PROJECT::Buffers &Buffer);
   ~KinectThread(void);

   private:

   KinectThread(const KinectThread &obj);          //Disable
   KinectThread &operator = (const KinectThread &obj); //Disable

   //Data allocation
   void Clear(void);
   void Destroy(void);

   public:

   void ChangeVideo(VideoType Type = Kinect::VideoRGB);

   //Thread execution and control
   void run(void);
   void stop(void);

   signals:

   void SignalConnected(bool Status);
   void SignalUpdate(void);
   void SignalError(QString Message);
   };


//==== End of file ===========================================================
#endif
