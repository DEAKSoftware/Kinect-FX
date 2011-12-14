/*===========================================================================
   Video Capture Thread

   Dominik Deak
  ===========================================================================*/

#ifndef ___THREAD_CAPTURE_CPP___
#define ___THREAD_CAPTURE_CPP___


/*---------------------------------------------------------------------------
   Header files
  ---------------------------------------------------------------------------*/
#include "common.h"
#include "debug.h"
#include "mutex.h"
#include "thread_capture.h"


/*---------------------------------------------------------------------------
   Static data.
  ---------------------------------------------------------------------------*/
const char* CaptureThread::ExtTGA = "tga";
const char* CaptureThread::ExtPNG = "png";


/*---------------------------------------------------------------------------
   Opens a new target directory for streaming. The subdirectory names are 
   generated automatically, based on the current date and timestamp, and with 
   the Prefix added.
  ---------------------------------------------------------------------------*/
CaptureThread::CaptureThread(QObject* Parent, const QString &Path, const QString &Prefix, CapFormat Format, bool Compress) : QThread(Parent)
   {
   Clear();

   if (Parent == nullptr) {throw dexception("Invalid parameters.");}

   setTerminationEnabled(true);

   //Hook signal functions to the parent class' slot functions
   QObject::connect(this, SIGNAL(SignalError(QString)), Parent, SLOT(CaptureError(QString)));

   CaptureThread::Format = Format;
   CaptureThread::Compress = Compress;
   Dir = Path;

   switch (CaptureThread::Format)
      {
      case CaptureThread::FormatTGA : Ext = CaptureThread::ExtTGA; break;
      case CaptureThread::FormatPNG : Ext = CaptureThread::ExtPNG; break;
      default : throw dexception("The specified file format is unknown.");
      }

   QString SubDir;
   bool Exists = false;
   uint I = 0;
   
   do {
      QDate Date = QDate::currentDate();
      QTime Time = QTime::currentTime();

      SubDir = Prefix;
      SubDir += Date.toString(" yyyy-MM-dd ");
      SubDir += Time.toString("HH-mm-ss-zzz");

      if (I > 0) {SubDir += QString(" %1").arg(I);}

      I++;

      Exists = Dir.exists(SubDir);
      }
   while (Exists && I < MaxSubDirAttempts);

   if (Exists) {throw dexception("Failed to create a unique subdirectory.");}

   if (!Dir.mkdir(SubDir))
      {throw dexception("Failed to create a new subdirectory.");}

   if (!Dir.cd(SubDir))
      {throw dexception("Failed to change into subdirectory.");}
   
   start();
   }

/*---------------------------------------------------------------------------
   Destructor.
  ---------------------------------------------------------------------------*/
CaptureThread::~CaptureThread(void)
   {
   Destroy();
   }

/*---------------------------------------------------------------------------
   Clears the structure.
  ---------------------------------------------------------------------------*/
void CaptureThread::Clear(void)
   {
   Count = 0;

   Format = CaptureThread::FormatTGA;
   Ext = CaptureThread::ExtTGA;

   Compress = false;
   Exit = false;
   }

/*---------------------------------------------------------------------------
   Destroys the structure.
  ---------------------------------------------------------------------------*/
void CaptureThread::Destroy(void)
   {
   Clear();
   }

/*---------------------------------------------------------------------------
   Thread entry point.
  ---------------------------------------------------------------------------*/
void CaptureThread::run(void)
   {
   debug("Started capture thread.\n");

   try {
      while (!Exit) 
         {
         //Wait for new frame
         QMutexLocker MutexLocker(&UpdateMutex);
         UpdateWait.wait(&UpdateMutex);
         
         //Lock frame
         NAMESPACE_PROJECT::MutexControl Mutex(Frame.GetMutexHandle());
         if (!Mutex.LockRequest()) {debug("Mutex already locked, dropping frame.\n"); continue;}

         if (Frame.Size() < 1) {debug("No data in frame, dropping frame.\n"); continue;}

         //Construct file name
         FileName = QString("%1.").arg((long)Count, FileNameDigits, FileNameBase, QLatin1Char('0')) + Ext;
         FileName = Dir.absoluteFilePath(FileName);
         Path = FileName.toAscii();

         //Save frame
         switch (Format)
            {
            case CaptureThread::FormatTGA : 
               TGA.Save(Frame, Path.constData(), false, Compress); 
               break;
      
            case CaptureThread::FormatPNG : 
               PNG.Save(Frame, Path.constData(), Compress ? NAMESPACE_PROJECT::File::PNG::CompSpeed : NAMESPACE_PROJECT::File::PNG::CompNone); 
               break;
      
            default : throw dexception("The specified file format is unknown.");
            }

         Count++;
         }
      }

   catch (std::exception &e) 
      {
      SignalError(e.what());
      Exit = true;
      }
   
   catch (...) 
      {
      SignalError("Trapped an unhandled exception in the capture thread.");
      Exit = true;
      }

   debug("Stopping capture thread.\n");
   }

/*---------------------------------------------------------------------------
   Signals to exit thread.
  ---------------------------------------------------------------------------*/
void CaptureThread::stop(void)
   {
   Exit = true;
   UpdateWait.wakeAll();
   }

/*---------------------------------------------------------------------------
   Signals thread to saves a frame to file. 
  ---------------------------------------------------------------------------*/
void CaptureThread::update(void)
   {
   if (Exit) {return;}
   UpdateWait.wakeAll();
   }


//==== End of file ===========================================================
#endif
