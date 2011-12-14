/*===========================================================================
   OpenGL Widget

   Dominik Deak
  ===========================================================================*/

#ifndef ___GLWIDGET_H___
#define ___GLWIDGET_H___


/*---------------------------------------------------------------------------
   Header files
  ---------------------------------------------------------------------------*/
#include "buffers.h"
#include "thread_capture.h"
#include "common.h"
#include "filter.h"
#include "mesh.h"
#include "texture.h"


/*---------------------------------------------------------------------------
  Classes
  ---------------------------------------------------------------------------*/
class GLWidget : public QGLWidget
   {
   //---- Qt specific ----
   Q_OBJECT

      //---- Constants and definitions ----
   public:

   static const int TimeCaptureKill = 3000;       //Wait time for the capture thread before terminating, in ms

   //---- Member data ----
   private:

   QMainWindow* Main;                              //Pointer to the main window
   NAMESPACE_PROJECT::Mesh Model;                  //Plane for rendering the frame buffer object texture
   NAMESPACE_PROJECT::Filter* FX;                  //Current effects filter object
   NAMESPACE_PROJECT::Buffers &Buffer;             //Kinect buffers
   NAMESPACE_PROJECT::uiter Count;                 //Frame counter
   NAMESPACE_PROJECT::uiter BufferCount;           //Number of buffers
   CaptureThread* Capture;                         //Stream the FBO to file
   bool Sync;                                      //Capture in synchronous mode
   bool Error;                                     //Indicates than an error has occured

   //---- Methods ----
   public:

   GLWidget(QMainWindow* Main, QWidget* Parent, NAMESPACE_PROJECT::Buffers &Buffer);
   ~GLWidget(void);

   private:

   //Data allocation
   void Clear(void);
   void Destroy(void);

   //Rendering callback functions
   void initializeGL(void);
   void resizeGL(int X, int Y);
   void paintGL(void);

   public:

   //Filter interface
   void AttachFilter(NAMESPACE_PROJECT::Filter* FX = nullptr);
   NAMESPACE_PROJECT::Filter* GetFilter(void) {return FX;}

   //Capture interface
   void CaptureOpen(const QString &Path, const QString &Prefix, CaptureThread::CapFormat Format, bool Compress);
   void CaptureClose(void);
   void CaptureSync(bool Sync) {GLWidget::Sync = Sync;}
   
   //Widget updates
   void UpdateGeometry(const QRect &Rect);
   void UpdateView(void);

   signals:

   void SignalError(QString Message);
   };


//==== End of file ===========================================================
#endif
