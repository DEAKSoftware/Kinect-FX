/*===========================================================================
   Filter Base Class

   Dominik Deak
  ===========================================================================*/

#ifndef ___FILTER_CPP___
#define ___FILTER_CPP___


/*---------------------------------------------------------------------------
   Header files
  ---------------------------------------------------------------------------*/
#include "common.h"
#include "debug.h"
#include "filter.h"
#include "math.h"


//Namespaces
NAMESPACE_BEGIN(NAMESPACE_PROJECT)


/*---------------------------------------------------------------------------
   Constructor.
  ---------------------------------------------------------------------------*/
Filter::Filter(void)
   {
   Clear();
   }

/*---------------------------------------------------------------------------
   Destructor. NOTE: Make sure it is called within a valid OpenGL context.
  ---------------------------------------------------------------------------*/
Filter::~Filter(void)
   {
   Destroy();
   }

/*---------------------------------------------------------------------------
   Clears the structure.
  ---------------------------------------------------------------------------*/
void Filter::Clear(void)
   {
   ViewPort = 0;
   Ratio = 0.0f;
   MP = 0.0f;
   MV = 0.0f;
   MT = 0.0f;
   MC = 0.0f;

   Scale.Set4(1.0f, 1.0f, 1.0f, 0.0f);
   Trans.Set4(0.0f, 0.0f, 0.0f, 0.0f);

   FBOID = 0;
   DBOID = 0;
   CBOID = 0;

   Near = -2.0f;
   Far = 2.0f;
   Range = Math::Abs(Far - Near);

   Select = Filter::SelectVideo;
   EnableVideo = true;
   EnableDepth = false;
   EnableCal = false;
   EnableColour = false;
   }

/*---------------------------------------------------------------------------
   Destroys the structure. NOTE: Make sure it is called within a valid 
   OpenGL context.
  ---------------------------------------------------------------------------*/
void Filter::Destroy(void)
   {
   Model.Destroy();
   Video.Destroy();
   Depth.Destroy();
   Program.Destroy();

   if (CBOID > 0) {glDeleteTextures(1, &CBOID);}
   if (DBOID > 0) {glDeleteRenderbuffers(1, &DBOID);}
   if (FBOID > 0) {glDeleteFramebuffers(1, &FBOID);}

   Clear();
   }

/*---------------------------------------------------------------------------
   Indicates whether the Setup( ) was called.
  ---------------------------------------------------------------------------*/
bool Filter::Ready(void) const 
   {
   return FBOID > 0 && DBOID > 0 && CBOID > 0;
   }

/*---------------------------------------------------------------------------
   Setup function that a intialises assets, matrices and other OpenGL
   related structure for rendering. NOTE: This must be called within a 
   valid OpenGL context.
  ---------------------------------------------------------------------------*/
void Filter::Setup(Buffers &Buffer)
   {
   Destroy();

   if (!GLEW_ARB_framebuffer_object)
      {throw dexception("Current OpenGL context does not support frame buffer objects.");}

   vector2u Res;
   
   switch (Select)
      {
      case Filter::SelectVideo : Res = Buffer.GetVideoResolution(); break;
      case Filter::SelectDepth : Res = Buffer.GetDepthResolution(); break;
      default : throw dexception("Invalid Select enumeration.");;
      }

   if (Res.X < 1 || Res.Y < 1) {return;}
   
   //Create a new frame buffer object
   glGenFramebuffers(1, &FBOID);
   glBindFramebuffer(GL_FRAMEBUFFER, FBOID);

   //Create a new depth buffer object, then associate a 
   // storage space for it, and attach it to the FBO
   glGenRenderbuffers(1, &DBOID);
   glBindRenderbuffer(GL_RENDERBUFFER, DBOID);
   glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, Res.X, Res.Y);
   glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, DBOID);

   //Create a colour buffer in form of a texture and attach it to the FBO
   glGenTextures(1, &CBOID);
   glBindTexture(GL_TEXTURE_2D, CBOID);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, Res.X, Res.Y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
   glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, CBOID, 0);

   //FBO texture scaling
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);

   //Setup view port size
   ViewPort.Set4(0, 0, Res.X, Res.Y);

   //Compute frustum aspect ratio from viewport size
   Ratio.Set((float)ViewPort.C2, (float)ViewPort.C3);
   Ratio /= (float)Math::Min(ViewPort.C2, ViewPort.C3);

   //Default projection matrix
   MP = MP.Identity();
   MP = MP.Ortho(-Ratio.X, Ratio.X, -Ratio.Y, Ratio.Y, Near, Far);

   //Default model view matrix
   MV = MV.Identity();
   MV = MV.Scale(Ratio.X, Ratio.Y, 1.0f);

   //Default texture matrix
   MT = MT.Identity();
   //MT = MT.Scale(1.0f, -1.0f, 1.0f);
   //MT = MT.Translate(0.0f, -1.0f, 0.0f);

   MC = MC.Identity();

   //Error checking   
   GLenum Error = glCheckFramebufferStatus(GL_FRAMEBUFFER);
   if (Error != GL_FRAMEBUFFER_COMPLETE) {throw dexception("Failed to setup frame buffer object: %s.", Debug::StatusFBO(Error));}

   Error = glGetError();
   if (Error != GL_NO_ERROR) {throw dexception("OpenGL generated an error: %s", Debug::ErrorGL(Error));}
   }

/*---------------------------------------------------------------------------
   Class specific setup function for the effects assets.
  ---------------------------------------------------------------------------*/
void Filter::Assets(Buffers &Buffer)
   {
   if (!Ready()) {return;}

   Video.Create(Buffer.GetVideoResolution(), Buffer.GetVideoDataType());
   Video.ClearData();
   Video.Buffer(false);

   Model.Plane(1, Mesh::ModeSolid);
   Model.Buffer(false);

   File::Text Text;
   std::string CodeVert, CodeFrag;
   Text.Load(CodeVert, File::Path::Shader(File::FilterVert));
   Text.Load(CodeFrag, File::Path::Shader(File::FilterFrag));
   Program.Attach(CodeVert, Shader::ShaderVert);
   Program.Attach(CodeFrag, Shader::ShaderFrag);
   Program.Buffer(false);

   GLenum Error = glGetError();
   if (Error != GL_NO_ERROR) {throw dexception("OpenGL generated an error: %s", Debug::ErrorGL(Error));}
   }

/*---------------------------------------------------------------------------
   Tests the selected input texture and changes configuration if the 
   resolutions do not match. The function does nothing for identical 
   configurations.
  ---------------------------------------------------------------------------*/
void Filter::Change(Buffers &Buffer)
   {
   vector2u Res[2];
   Texture::TexType Type[2];

   switch (Select)
      {
      case Filter::SelectVideo : 
         Res[0] = Buffer.GetVideoResolution();
         Res[1] = Video.Resolution();
         Type[0] = Buffer.GetVideoDataType();
         Type[1] = Video.DataType();
         break;

      case Filter::SelectDepth : 
         Res[0] = Buffer.GetDepthResolution();
         Res[1] = Depth.Resolution();
         Type[0] = Buffer.GetDepthDataType();
         Type[1] = Depth.DataType();
         break;

      default : throw dexception("Invalid Select enumeration.");
      }

   if (Res[0].U < 1 || Res[0].V < 1) {return;}
   
   if (Res[0].U != Res[1].U || Res[0].V != Res[1].V || Type[0] != Type[1])
      {
      Setup(Buffer);
      Assets(Buffer);
      }
   }

/*---------------------------------------------------------------------------
   Updates the scaling component of the texture calbiration. One unit 
   repesents a single pixel in each direction.
  ---------------------------------------------------------------------------*/
void Filter::AddScale(float X, float Y)
   {
   vector2u Res;

   switch (Select)
      {
      case Filter::SelectVideo : Res = Video.Resolution(); break;
      case Filter::SelectDepth : Res = Depth.Resolution(); break;
      default : throw dexception("Invalid Select enumeration.");
      }

   if (Res.U > 0) {Scale.X += X / (float)Res.U;}
   if (Res.V > 0) {Scale.Y += Y / (float)Res.V;}

   MC = MC.Identity();
   MC = MC.Translate(Trans);
   MC = MC.Scale(Scale);
   }

/*---------------------------------------------------------------------------
   Updates the translation component of the texture calbiration.
  ---------------------------------------------------------------------------*/
void Filter::AddTrans(float X, float Y)
   {
   vector2u Res;

   switch (Select)
      {
      case Filter::SelectVideo : Res = Video.Resolution(); break;
      case Filter::SelectDepth : Res = Depth.Resolution(); break;
      default : throw dexception("Invalid Select enumeration.");
      }

   if (Res.U > 0) {Trans.X += X / (float)Res.U;}
   if (Res.V > 0) {Trans.Y += Y / (float)Res.V;}

   MC = MC.Identity();
   MC = MC.Translate(Trans);
   MC = MC.Scale(Scale);
   }

/*---------------------------------------------------------------------------
   Binds the frame buffer object and intialises the matrix stack. The 
   function leaves with matrix mode in model view state.
  ---------------------------------------------------------------------------*/
void Filter::Bind(void)
   {
   if (!Ready()) {return;}

   glBindFramebuffer(GL_FRAMEBUFFER, FBOID);

   glPushAttrib(GL_ALL_ATTRIB_BITS);
   glViewport(ViewPort.C0, ViewPort.C1, ViewPort.C2, ViewPort.C3);

   glMatrixMode(GL_PROJECTION);
   glPushMatrix();
   glLoadMatrixf(MP.C);

   glMatrixMode(GL_TEXTURE);
   glActiveTexture(GL_TEXTURE0);
   glPushMatrix();
   glLoadMatrixf(MT.C);

   glMatrixMode(GL_MODELVIEW);
   glPushMatrix();
   glLoadMatrixf(MV.C);
   }

/*---------------------------------------------------------------------------
   Restores the matrix stack, updates the colour buffer texture and then
   unbinds the frame buffer object. The function leaves with matrix mode in 
   projection state.
  ---------------------------------------------------------------------------*/
void Filter::Unbind(void)
   {
   if (!Ready()) {return;}

   glMatrixMode(GL_MODELVIEW);
   glPopMatrix();

   glMatrixMode(GL_TEXTURE);
   glActiveTexture(GL_TEXTURE0);
   glPopMatrix();

   glMatrixMode(GL_PROJECTION);
   glPopMatrix();

   glPopAttrib();
   
   //Update texture mipmaps post rendering
   glBindTexture(GL_TEXTURE_2D, CBOID);
   glGenerateMipmap(GL_TEXTURE_2D);

   glBindFramebuffer(GL_FRAMEBUFFER, 0);
   }

/*---------------------------------------------------------------------------
   Renders the filter effect. Assumes the Bind( ) method was called prior.
  ---------------------------------------------------------------------------*/
void Filter::Render(void)
   {
   if (!Ready()) {return;}

   glEnable(GL_TEXTURE_2D);

   glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
   glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

   glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

   Program.Bind();
   Video.Bind(0);
   Model.Bind(1);
   Model.Render();
   Model.Unbind(1);
   Video.Unbind(0);
   Program.Unbind();

   glDisable(GL_TEXTURE_2D);
   
   #if defined (DEBUG)
      GLenum Error = glGetError();
      if (Error != GL_NO_ERROR) {throw dexception("OpenGL generated an error: %s", Debug::ErrorGL(Error));}
   #endif
}

/*---------------------------------------------------------------------------
   This function will update either the video texture, or depth texture, or
   both (if applicable). It may also recofigure the filter if the selected
   input has a different resolution. Returns true if either of the textures
   were updated.
  ---------------------------------------------------------------------------*/
bool Filter::Update(Buffers &Buffer)
   {
   if (!Ready()) {return false;}

   //Test for resolution change, if applicable
   Change(Buffer);
   
   bool Updated = false;

   if (Buffer.VideoUpdated(VideoUpdateID) && EnableVideo)
      {
      Texture &VideoFront = Buffer.GetVideo();
      MutexControl Mutex(VideoFront.GetMutexHandle());
      if (Mutex.LockRequest())
         {
         Video.Bind(0);
         Video.Update(VideoFront);
         Video.Unbind(0);
         Mutex.Unlock();
         Updated |= true;
         }
      }

   if (Buffer.DepthUpdated(DepthUpdateID) && EnableDepth)
      {
      Texture &DepthFront = Buffer.GetDepth();
      MutexControl Mutex(DepthFront.GetMutexHandle());
      if (Mutex.LockRequest())
         {
         Depth.Bind(0);
         Depth.Update(DepthFront);
         Depth.Unbind(0);
         Mutex.Unlock();
         Updated |= true;
         }
      }

   #if defined (DEBUG)
      GLenum Error = glGetError();
      if (Error != GL_NO_ERROR) {throw dexception("OpenGL generated an error: %s", Debug::ErrorGL(Error));}
   #endif

   return Updated;
   }

/*---------------------------------------------------------------------------
   Captures the contents of the frame buffer object for streaming purposes.
   Returns true if the frame was successfully captured. If the Wait flag is
   set, the function with use a blocking mutex lock on the Frame texture.
  ---------------------------------------------------------------------------*/
bool Filter::Capture(Texture &Frame, bool Wait)
   {
   if (!Ready()) {return false;}

   MutexControl Mutex(Frame.GetMutexHandle());
   if (!Wait && !Mutex.LockRequest()) {return false;}
   else {Mutex.Lock();}
   
   vector2u Res = Frame.Resolution();
   if (Res.U != ViewPort.C2 || Res.V != ViewPort.C3 || Frame.DataType() != Texture::TypeRGB) 
      {
      Res.Set(ViewPort.C2, ViewPort.C3);
      Frame.Create(Res, Texture::TypeRGB);
      }

   glBindFramebuffer(GL_READ_FRAMEBUFFER, FBOID);
   glReadPixels(0, 0, Res.U, Res.V, Frame.DataFormat(), Frame.DataCompType(), Frame.Pointer());
   glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

   #if defined (DEBUG)
      GLenum Error = glGetError();
      if (Error != GL_NO_ERROR) {throw dexception("OpenGL generated an error: %s", Debug::ErrorGL(Error));}
   #endif
   
   return true;
   }


//Close namespaces
NAMESPACE_END(NAMESPACE_PROJECT)


//==== End of file ===========================================================
#endif
