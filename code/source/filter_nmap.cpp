/*===========================================================================
   Normal Map Filter Class

   Dominik Deak
  ===========================================================================*/

#ifndef ___FILTER_NMAP_CPP___
#define ___FILTER_NMAP_CPP___


/*---------------------------------------------------------------------------
   Header files
  ---------------------------------------------------------------------------*/
#include "common.h"
#include "debug.h"
#include "filter_nmap.h"


//Namespaces
NAMESPACE_BEGIN(NAMESPACE_PROJECT)


/*---------------------------------------------------------------------------
   Constructor.
  ---------------------------------------------------------------------------*/
FilterNMap::FilterNMap(void)
   {
   Clear();
   }

/*---------------------------------------------------------------------------
   Destructor. NOTE: Make sure it is called within a valid OpenGL context.
  ---------------------------------------------------------------------------*/
FilterNMap::~FilterNMap(void)
   {
   Destroy();
   }

/*---------------------------------------------------------------------------
   Clears the structure.
  ---------------------------------------------------------------------------*/
void FilterNMap::Clear(void)
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
void FilterNMap::Destroy(void)
   {
   Filter::Destroy();

   Clear();
   }

/*---------------------------------------------------------------------------
   Class specific setup function for the effects assets.
  ---------------------------------------------------------------------------*/
void FilterNMap::Assets(Buffers &Buffer)
   {
   if (!Ready()) {return;}

   Depth.Create(Buffer.GetDepthResolution(), Buffer.GetDepthDataType());
   Depth.ClearData();
   Depth.Buffer(false);

   Model.Plane(1, Mesh::ModeSolid);
   Model.Buffer(false);

   File::Text Text;
   std::string CodeVert, CodeFrag;
   Text.Load(CodeVert, File::Path::Shader(File::FilterNMapVert));
   Text.Load(CodeFrag, File::Path::Shader(File::FilterNMapFrag));
   Program.Attach(CodeVert, Shader::ShaderVert);
   Program.Attach(CodeFrag, Shader::ShaderFrag);
   Program.Buffer(false);

   Program.Bind();
   glUniform1i(glGetUniformLocation(Program.ID(), "Texture"), 0);   //Texture unit 0
   Program.Unbind();

   GLenum Error = glGetError();
   if (Error != GL_NO_ERROR) {throw dexception("OpenGL generated an error: %s", Debug::ErrorGL(Error));}
   }

/*---------------------------------------------------------------------------
   Renders the filter effect. Assumes the Bind( ) method was called prior.
  ---------------------------------------------------------------------------*/
void FilterNMap::Render(void)
   {
   if (!Ready()) {return;}

   glEnable(GL_TEXTURE_2D);

   glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
   glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

   Program.Bind();
   Model.Bind(1);
   Depth.Bind(0);
   Model.Render();
   Depth.Unbind(0);
   Model.Unbind(1);
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
