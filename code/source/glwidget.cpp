/*===========================================================================
   OpenGL Widget

   Dominik Deak
  ===========================================================================*/

#ifndef ___GLWIDGET_CPP___
#define ___GLWIDGET_CPP___


/*---------------------------------------------------------------------------
   Header files
  ---------------------------------------------------------------------------*/
#include "common.h"
#include "debug.h"
#include "glwidget.h"
#include "matrix.h"
#include "mesh.h"


/*---------------------------------------------------------------------------
   Constructor. Performs OpenGL version checks and allocates models.

   Parent : Pointer to the parent widget, must not be nullptr.
   Main   : Pointer to the main window, must not be nullptr.
  ---------------------------------------------------------------------------*/
GLWidget::GLWidget(QMainWindow* Main, QWidget* Parent, NAMESPACE_PROJECT::Buffers &Buffer) : QGLWidget(QGLFormat(QGL::SampleBuffers), Parent), /*QThread(Parent),*/ Buffer(Buffer)
   {
   Clear();

   if (Main == nullptr || Parent == nullptr) 
      {throw dexception("Invalid parameters.");}

   GLWidget::Main = Main;

   QObject::connect(this, SIGNAL(SignalError(QString)), Main, SLOT(RenderError(QString)));

   makeCurrent();

   GLenum Error = glewInit();
   if (GLEW_OK != Error) {throw dexception("glewInit( ) failed: %s.", glewGetErrorString(Error));}

   QGLFormat::OpenGLVersionFlags Flags = QGLFormat::openGLVersionFlags() & QGLFormat::OpenGL_Version_2_0;
   if (Flags == QGLFormat::OpenGL_Version_None) {throw dexception("OpenGL version is not 2.0 or higher.");}

   if (!GLEW_ARB_texture_non_power_of_two)
      {throw dexception("Current OpenGL implementation does not support non power of two textures.");}

   Model.Plane(1, NAMESPACE_PROJECT::Mesh::ModeSolid);
   Model.Buffer(false);
   }

/*---------------------------------------------------------------------------
   Destructor.
  ---------------------------------------------------------------------------*/
GLWidget::~GLWidget(void)
   {
   makeCurrent();
   Destroy();
   }

/*---------------------------------------------------------------------------
   Resets the structure.
  ---------------------------------------------------------------------------*/
void GLWidget::Clear(void)
   {
   FX = nullptr;
   Capture = nullptr;
   Count = 0;
   BufferCount = 1;
   Sync = true;
   Error = false;
   }

/*---------------------------------------------------------------------------
   Deallocates all objects.
  ---------------------------------------------------------------------------*/
void GLWidget::Destroy(void)
   {
   CaptureClose();

   Model.Destroy();
   
   delete FX;
   delete Capture;
   
   Clear();
   }

/*---------------------------------------------------------------------------
   Attaches a filter object to the widget. Note: Must set the OpenGL context 
   to current current in this function.
  
   FX : Pointer to the new filter object. This class will take ownership of
        the filter object and it will handle its destruction. If this 
        parameter is a nullptr, the current filter will be destroyed.
  ---------------------------------------------------------------------------*/
void GLWidget::AttachFilter(NAMESPACE_PROJECT::Filter* FX)
   {
   makeCurrent();
   delete GLWidget::FX;
   GLWidget::FX = FX;
   }

/*---------------------------------------------------------------------------
   Starts a capture stream for the attached effects filter.
  ---------------------------------------------------------------------------*/
void GLWidget::CaptureOpen(const QString &Path, const QString &Prefix, CaptureThread::CapFormat Format, bool Compress)
   {
   if (Error || FX == nullptr) {return;}

   CaptureClose();
   
   Capture = new CaptureThread(Main, Path, Prefix, Format, Compress);
   }

/*---------------------------------------------------------------------------
   Closes the capture stream.
  ---------------------------------------------------------------------------*/
void GLWidget::CaptureClose(void)
   {
   if (Capture == nullptr) {return;}
   
   if (Capture->isRunning())
      {
      Capture->stop();
   
      if (!Capture->wait(TimeCaptureKill))
         {
         debug("Capture thread is still running, attempting to terminate.\n");
         Capture->terminate();
         }
      }

   delete Capture;
   Capture = nullptr;
   }

/*---------------------------------------------------------------------------
   Updates the rendering context geometry.
  ---------------------------------------------------------------------------*/
void GLWidget::UpdateGeometry(const QRect &Rect)
   {
   setGeometry(0, 0, Rect.width(), Rect.height());
   }

/*---------------------------------------------------------------------------
   Updates the rendering context view port and matrices using the OpenGL
   widget's size. Note: Must set the OpenGL context to current current in 
   this function.
  ---------------------------------------------------------------------------*/
void GLWidget::UpdateView(void)
   {
   makeCurrent();
   QRect Rect = rect();
   resizeGL(Rect.width(), Rect.height());
   }

/*---------------------------------------------------------------------------
   Setup OpenGL. QT makes the OpenGL context current prior calling this 
   function.
  ---------------------------------------------------------------------------*/
void GLWidget::initializeGL(void)
   {
   try {
      //Basic setup
      glEnable(GL_DEPTH_TEST);
      glEnable(GL_CULL_FACE);
      glEnable(GL_RESCALE_NORMAL);
      glCullFace(GL_BACK);
      glShadeModel(GL_SMOOTH);

      //Blending
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      //Antialiasing
      glEnable(GL_POINT_SMOOTH);
      glEnable(GL_LINE_SMOOTH);
      glEnable(GL_MULTISAMPLE);

      //Clear buffers
      glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
      glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

      //Setup the matrix stack
      UpdateView();
   
      BufferCount = doubleBuffer() ? 2 : 1;
      Count = 0;

      GLenum Error = glGetError();
      if (Error != GL_NO_ERROR) {throw dexception("OpenGL generated an error: %s", NAMESPACE_PROJECT::Debug::ErrorGL(Error));}
      }

   catch (std::exception &e) 
      {
      SignalError(e.what());
      Error = true;
      }
   
   catch (...) 
      {
      SignalError("Trapped an unhandled exception in the GLWidget filter update function.");
      Error = true;
      }
   }

/*---------------------------------------------------------------------------
   Handles resize events for OpenGL. It computes the new view port, the
   projeciton matrix and the model view matrix. QT makes the OpenGL context 
   current prior calling this function.
  ---------------------------------------------------------------------------*/
void GLWidget::resizeGL(int X, int Y)
   {
   try {
      NAMESPACE_PROJECT::vector2f VRatio = 1.0f;
      NAMESPACE_PROJECT::vector2f FRatio = 1.0f;

      //Normalised aspect ratio of the view port
      if (X > 0 && Y > 0)
         {
         glViewport(0, 0, X, Y);
         VRatio.Set((float)X, (float)Y);
         VRatio /= (float)NAMESPACE_PROJECT::Math::Min(X, Y);
         }

      //Normalised aspect ratio of the filter object
      if (FX != nullptr)
         {
         NAMESPACE_PROJECT::vector2u Res = FX->Resolution();
         if (Res.X > 0 && Res.Y > 0)
            {
            FRatio.Set((float)Res.X, (float)Res.Y);
            FRatio /= (float)NAMESPACE_PROJECT::Math::Min(Res.X, Res.Y);
            }
         }

      //Scale frame buffer object to fit inside view port
      if (VRatio.X < FRatio.X)
         {
         FRatio *= VRatio.X / FRatio.X;
         }
      else if (VRatio.Y < FRatio.Y)
         {
         FRatio *= VRatio.Y / FRatio.Y;
         }

      //Texture matrix
      glMatrixMode(GL_TEXTURE);
      glLoadIdentity();
      glScalef(1.0f, -1.0f, 1.0f);
      glTranslatef(0.0f, -1.0f, 0.0f);

      //Projection matrix
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      glOrtho(-VRatio.X, VRatio.X, -VRatio.Y, VRatio.Y, -1.0f, 1.0f);

      //Model view matrix
      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();
      glScalef(FRatio.X, FRatio.Y, 1.0f);
      }

   catch (std::exception &e) 
      {
      SignalError(e.what());
      Error = true;
      }
   
   catch (...) 
      {
      SignalError("Trapped an unhandled exception in the GLWidget filter update function.");
      Error = true;
      }
   }

/*---------------------------------------------------------------------------
   Render event. QT makes the OpenGL context current prior calling this 
   function.
  ---------------------------------------------------------------------------*/
void GLWidget::paintGL(void)
   {
   if (Error || FX == nullptr) {return;}

   try {
      //Force clear buffer on first couple of calls
      if (Count < BufferCount)
         {
         glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
         glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
         Count++;
         }

      //Initialise filter if not ready
      if (!FX->Ready())
         {
         FX->Setup(Buffer);
         FX->Assets(Buffer);
         UpdateView();
         }

      //Update filter with new content (returns false if no relevant buffers were updated)
      if (!FX->Update(Buffer)) {return;}

      //Render effects
      FX->Bind();
      FX->Render();
      FX->Unbind();

      //Capture if appropriate
      bool Dropped = false;
      if (Capture != nullptr)
         {
         Dropped = !FX->Capture(Capture->Texture(), Sync);
         if (!Dropped) {Capture->update();}
         }

      //Display the frame buffer object in the GLWidget
      if (FX->ID() > 0) 
         {
         glEnable(GL_TEXTURE_2D);
         glEnable(GL_BLEND);

         glMatrixMode(GL_MODELVIEW);

         glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
         glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

         glBindTexture(GL_TEXTURE_2D, FX->ID());
      
         glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

         Model.Bind(1);
         Model.Render();
         Model.Unbind(1);

         glBindTexture(GL_TEXTURE_2D, 0);

         //Display warning when frame was dropped
         if (Dropped)
            {
            glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
            renderText(0.8f, -0.95f, 1.0f, "Dropped");
            }

         glDisable(GL_BLEND);
         glDisable(GL_TEXTURE_2D);

         Count++;

         #if defined (DEBUG)
            GLenum Error = glGetError();
            if (Error != GL_NO_ERROR) {throw dexception("OpenGL generated an error: %s", NAMESPACE_PROJECT::Debug::ErrorGL(Error));}
         #endif
         }
      }

   catch (std::exception &e) 
      {
      SignalError(e.what());
      Error = true;
      }

   catch (...) 
      {
      SignalError("Trapped an unhandled exception in the GLWidget filter update function.");
      Error = true;
      }
   }


//==== End of file ===========================================================
#endif
