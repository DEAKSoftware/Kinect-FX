/*===========================================================================
   Line Effects Filter Class

   Dominik Deak
  ===========================================================================*/

#ifndef ___FILTER_LINES_CPP___
#define ___FILTER_LINES_CPP___


/*---------------------------------------------------------------------------
   Header files
  ---------------------------------------------------------------------------*/
#include "common.h"
#include "debug.h"
#include "filter_lines.h"


//Namespaces
NAMESPACE_BEGIN(NAMESPACE_PROJECT)


/*---------------------------------------------------------------------------
   Constructor.
  ---------------------------------------------------------------------------*/
FilterLines::FilterLines(void)
   {
   Clear();
   }

/*---------------------------------------------------------------------------
   Destructor. NOTE: Make sure it is called within a valid OpenGL context.
  ---------------------------------------------------------------------------*/
FilterLines::~FilterLines(void)
   {
   Destroy();
   }

/*---------------------------------------------------------------------------
   Clears the structure.
  ---------------------------------------------------------------------------*/
void FilterLines::Clear(void)
   {
   Filter::Clear();

   Select = Filter::SelectDepth;
   EnableVideo = false;
   EnableDepth = true;
   }

/*---------------------------------------------------------------------------
   Destroys the structure. NOTE: Make sure it is called within a valid 
   OpenGL context.
  ---------------------------------------------------------------------------*/
void FilterLines::Destroy(void)
   {
   Filter::Destroy();

   Lines.Destroy();

   Clear();
   }

/*---------------------------------------------------------------------------
   Class specific setup function for the effects assets.
  ---------------------------------------------------------------------------*/
void FilterLines::Assets(Buffers &Buffer)
   {
   if (!Ready()) {return;}

   GLint Count = 0;
   glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS_ARB, &Count);
   if (Count < 1) {throw dexception("Current OpenGL context does not support vertex texture image units.");}

   vector2u Res = Buffer.GetDepthResolution();
   Depth.Create(Res, Buffer.GetDepthDataType());
   Depth.ClearData();
   Depth.Buffer(false);

   Model.Plane(1, Mesh::ModeSolid);
   Model.Buffer(false);

   File::PNG PNG;
   PNG.Load(Lines, File::Path::Texture(File::TextureLines));
   Lines.SetWrap(true);
   Lines.Buffer(false);

   File::Text Text;
   std::string CodeVert, CodeFrag;
   Text.Load(CodeVert, File::Path::Shader(File::FilterLinesVert));
   Text.Load(CodeFrag, File::Path::Shader(File::FilterLinesFrag));
   Program.Attach(CodeVert, Shader::ShaderVert);
   Program.Attach(CodeFrag, Shader::ShaderFrag);
   Program.Buffer(false);

   Program.Bind();
   glUniform1i(glGetUniformLocation(Program.ID(), "Texture"), 0);   //Texture unit 0
   glUniform1i(glGetUniformLocation(Program.ID(), "Lines"), 1);   //Texture unit 1
   glUniform1f(glGetUniformLocation(Program.ID(), "Scale"), (float)Res.V / 12.0f); //Scale factor for the line texture sampler
   Program.Unbind();

   GLenum Error = glGetError();
   if (Error != GL_NO_ERROR) {throw dexception("OpenGL generated an error: %s", Debug::ErrorGL(Error));}
   }

/*---------------------------------------------------------------------------
   Renders the filter effect. Assumes the Bind( ) method was called prior.
  ---------------------------------------------------------------------------*/
void FilterLines::Render(void)
   {
   if (!Ready()) {return;}

   glEnable(GL_TEXTURE_2D);

   glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
   glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

   Program.Bind();
   Model.Bind(2);
   Depth.Bind(0);
   Lines.Bind(1);
   Model.Render();
   Lines.Unbind(1);
   Depth.Unbind(0);
   Model.Unbind(2);
   Program.Unbind();

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
