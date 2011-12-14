/*===========================================================================
   Video Capture Thread

   Dominik Deak

   Note: The Meta-Object Compiler must be run on this header to generate 
   the required C++ file to allow interfacing between objects.
  ===========================================================================*/

#ifndef ___THREAD_CAPTURE_H___
#define ___THREAD_CAPTURE_H___


/*---------------------------------------------------------------------------
   Header files
  ---------------------------------------------------------------------------*/
#include "common.h"
#include "file.h"
#include "texture.h"


/*---------------------------------------------------------------------------
   Worker thread for streaming files
  ---------------------------------------------------------------------------*/
class CaptureThread : public QThread
   {
   //---- Qt specific ----
   Q_OBJECT

   //---- Constants and definitions ----
   public:

   enum CapFormat                                  //CaptureThread format enumeration
      {
      FormatTGA = 0,                               //CaptureThread as raw TGA files
      FormatPNG = 1                                //CaptureThread as PNG files
      };

   static const char* ExtTGA;
   static const char* ExtPNG;

   static const uint MaxSubDirAttempts = 10;       //Number of times to attempt for creating a subdirectory
   static const uint TimeSubDirAttempts = 50;      //Sleep interval between subdirectory creation attempts
   static const uint FileNameDigits = 8;           //Number of digits to use in the file name counter
   static const uint FileNameBase = 10;            //Numeric base to use in the file name counter

   //---- Member data ----
   private:

   CapFormat Format;                               //Specifies the capture file format
   bool Compress;                                  //Apply data compression
   NAMESPACE_PROJECT::File::PNG PNG;               //PNG file I/O
   NAMESPACE_PROJECT::File::TGA TGA;               //TGA file I/O
   NAMESPACE_PROJECT::Texture Frame;               //Frame data
   NAMESPACE_PROJECT::uint64 Count;                //Frame counter

   QDir Dir;                                       //Path where the frames will be streamed
   QString FileName, Ext;                          //File name and format extension
   QByteArray Path;                                //Temporary byte array for string conversion

   QWaitCondition UpdateWait;                      //Thread wait condition
   QMutex UpdateMutex;                             //Thread update mutex
   bool Exit;                                      //Signals to exit thread
 
   //---- Methods ----
   public:

   CaptureThread(QObject* Parent, const QString &Path, const QString &Prefix, CapFormat Format = CaptureThread::FormatTGA, bool Compress = true);
   ~CaptureThread(void);

   private:

   CaptureThread(const CaptureThread &obj);                    //Disable
   CaptureThread &operator = (const CaptureThread &obj);       //Disable

   //Data allocation
   void Clear(void);
   void Destroy(void);

   public:

   //Thread control
   void run(void);
   void stop(void);
   void update(void);

   //Data access
   inline NAMESPACE_PROJECT::Texture& Texture(void) {return Frame;}

   signals:

   void SignalError(QString Message);
   };


//==== End of file ===========================================================
#endif
