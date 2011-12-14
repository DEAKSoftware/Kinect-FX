/*===========================================================================
   Fatty Filter Class

   Dominik Deak
  ===========================================================================*/

#ifndef ___FILTER_FATTY_CPP___
#define ___FILTER_FATTY_CPP___


/*---------------------------------------------------------------------------
   Header files
  ---------------------------------------------------------------------------*/
#include "common.h"
#include "debug.h"
#include "filter_fatty.h"


//Namespaces
NAMESPACE_BEGIN(NAMESPACE_PROJECT)


/*---------------------------------------------------------------------------
   Constructor.
  ---------------------------------------------------------------------------*/
FilterFatty::FilterFatty(void)
   {
   Clear();
   }

/*---------------------------------------------------------------------------
   Destructor. NOTE: Make sure it is called within a valid OpenGL context.
  ---------------------------------------------------------------------------*/
FilterFatty::~FilterFatty(void)
   {
   Destroy();
   }

/*---------------------------------------------------------------------------
   Clears the structure.
  ---------------------------------------------------------------------------*/
void FilterFatty::Clear(void)
   {
   Filter::Clear();

   Select = Filter::SelectVideo;
   EnableVideo = true;
   EnableDepth = true;
   EnableCal = true;
   EnableColour = true;

   Mat.SetDiffuse(vector4f(0.9f, 0.9f, 0.9f, 1.0f));
   Mat.SetSpecular(vector4f(0.2f, 0.2f, 0.2f, 0.0f));
   Mat.SetShininess(100.0f);
   }

/*---------------------------------------------------------------------------
   Destroys the structure. NOTE: Make sure it is called within a valid 
   OpenGL context.
  ---------------------------------------------------------------------------*/
void FilterFatty::Destroy(void)
   {
   Filter::Destroy();

   Clear();
   }

/*---------------------------------------------------------------------------
   Setup function for the fatty effect. The following sources have more info
   on camera calibration:
   http://www.ros.org/wiki/kinect_calibration/technical
   http://nicolas.burrus.name/index.php/Research/KinectCalibration
  ---------------------------------------------------------------------------*/
void FilterFatty::Assets(Buffers &Buffer)
   {
   if (!Ready()) {return;}

   GLint Count = 0;
   glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS_ARB, &Count);
   if (Count < 1) {throw dexception("Current OpenGL context does not support vertex texture image units.");}

   Light.SetPosition(vector4f(0.0f, 2.0f, 2.0f, 0.0f));
   Light.Buffer(0);

   vector2u Res = Buffer.GetVideoResolution();
   Video.Create(Res, Buffer.GetVideoDataType());
   Video.ClearData();
   Video.Buffer(false);

   Depth.Create(Buffer.GetDepthResolution(), Buffer.GetDepthDataType());
   Depth.ClearData();
   Depth.Buffer(false);

   Model.Plane(Res >> 1, Mesh::ModeSolid);
   Model.Buffer(false);

   File::Text Text;
   std::string CodeVert, CodeFrag;
   Text.Load(CodeVert, File::Path::Shader(File::FilterFattyVert));
   Text.Load(CodeFrag, File::Path::Shader(File::FilterFattyFrag));
   Program.Attach(CodeVert, Shader::ShaderVert);
   Program.Attach(CodeFrag, Shader::ShaderFrag);
   Program.Buffer(false);

   vector2f d[2];
   d[0].U = 1.0f / (float)Res.U;
   d[0].V = 1.0f / (float)Res.V;
   d[1].U = d[0].U * 2.0f;
   d[1].V = d[0].V * 2.0f;

   //Canny's edge detector constants combined with luminance weights.
   // We declare them here, to be used as a uniform, because GLSL does
   // not support const array of vectors.
   // http://suraj.lums.edu.pk/~cs436a02/CannyImplementation.htm
   vector4f K[10] =
      {
      vector4f(0.2990f, 0.5870f, 0.1140f, 0.0f) * vector4f(0.058824f),
      vector4f(0.2990f, 0.5870f, 0.1140f, 0.0f) * vector4f(0.270588f),
      vector4f(0.2990f, 0.5870f, 0.1140f, 0.0f) * vector4f(0.447059f),
      vector4f(0.2990f, 0.5870f, 0.1140f, 0.0f) * vector4f(0.270588f),
      vector4f(0.2990f, 0.5870f, 0.1140f, 0.0f) * vector4f(0.058824f),

      vector4f(0.2990f, 0.5870f, 0.1140f, 0.0f) * vector4f(0.137255f),
      vector4f(0.2990f, 0.5870f, 0.1140f, 0.0f) * vector4f(0.607843f),
      vector4f(0.2990f, 0.5870f, 0.1140f, 0.0f),
      vector4f(0.2990f, 0.5870f, 0.1140f, 0.0f) * vector4f(0.607843f),
      vector4f(0.2990f, 0.5870f, 0.1140f, 0.0f) * vector4f(0.137255f)
      };

   Program.Bind();
   glUniform1i(glGetUniformLocation(Program.ID(), "Depth"), 0);   //Texture unit 0
   glUniform1i(glGetUniformLocation(Program.ID(), "Video"), 1);   //Texture unit 1
   glUniform2f(glGetUniformLocation(Program.ID(), "d[0]"), d[0].U, d[0].V);
   glUniform2f(glGetUniformLocation(Program.ID(), "d[1]"), d[1].U, d[1].V);
   glUniform3f(glGetUniformLocation(Program.ID(), "K[0]"), K[0].X, K[0].Y, K[0].Z);
   glUniform3f(glGetUniformLocation(Program.ID(), "K[1]"), K[1].X, K[1].Y, K[1].Z);
   glUniform3f(glGetUniformLocation(Program.ID(), "K[2]"), K[2].X, K[2].Y, K[2].Z);
   glUniform3f(glGetUniformLocation(Program.ID(), "K[3]"), K[3].X, K[3].Y, K[3].Z);
   glUniform3f(glGetUniformLocation(Program.ID(), "K[4]"), K[4].X, K[4].Y, K[4].Z);
   glUniform3f(glGetUniformLocation(Program.ID(), "K[5]"), K[5].X, K[5].Y, K[5].Z);
   glUniform3f(glGetUniformLocation(Program.ID(), "K[6]"), K[6].X, K[6].Y, K[6].Z);
   glUniform3f(glGetUniformLocation(Program.ID(), "K[7]"), K[7].X, K[7].Y, K[7].Z);
   glUniform3f(glGetUniformLocation(Program.ID(), "K[8]"), K[8].X, K[8].Y, K[8].Z);
   glUniform3f(glGetUniformLocation(Program.ID(), "K[9]"), K[9].X, K[9].Y, K[9].Z);
   Program.Unbind();

   GLenum Error = glGetError();
   if (Error != GL_NO_ERROR) {throw dexception("OpenGL generated an error: %s", Debug::ErrorGL(Error));}
   }

/*---------------------------------------------------------------------------
   Renders the filter effect. Assumes the Bind( ) method was called prior.
  ---------------------------------------------------------------------------*/
void FilterFatty::Render(void)
   {
   if (!Ready()) {return;}

   glEnable(GL_TEXTURE_2D);
   glEnable(GL_LIGHTING);

   //Video texture uses its own matrix
   glMatrixMode(GL_TEXTURE);
   glActiveTexture(GL_TEXTURE1);
   glPushMatrix();
   glLoadMatrixf(MC.C);

   glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
   glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

   Light.Bind();
   Mat.Bind();

   Program.Bind();
   Model.Bind(2);
   Depth.Bind(0);
   Video.Bind(1);       

   Model.Render();

   Video.Unbind(1);
   Depth.Unbind(0);
   Model.Unbind(2);
   Program.Unbind();

   Light.Unbind();

   glMatrixMode(GL_TEXTURE);
   glActiveTexture(GL_TEXTURE1);
   glPopMatrix();

   glDisable(GL_LIGHTING);
   glDisable(GL_TEXTURE_2D);

   #if defined (DEBUG)
      GLenum Error = glGetError();
      if (Error != GL_NO_ERROR) {throw dexception("OpenGL generated an error: %s", Debug::ErrorGL(Error));}
   #endif
   }


//Close namespaces
NAMESPACE_END(NAMESPACE_PROJECT)


//==== End of file ===========================================================
#endif
