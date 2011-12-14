/*===========================================================================
   Depth Filter Class With Palette

   Dominik Deak
  ===========================================================================*/

#ifndef ___FILTER_PALETTE_CPP___
#define ___FILTER_PALETTE_CPP___


/*---------------------------------------------------------------------------
   Header files
  ---------------------------------------------------------------------------*/
#include "common.h"
#include "debug.h"
#include "filter_palette.h"


//Namespaces
NAMESPACE_BEGIN(NAMESPACE_PROJECT)


/*---------------------------------------------------------------------------
   Constructor.
  ---------------------------------------------------------------------------*/
FilterPalette::FilterPalette(PaletteType Type)
   {
   Clear();
   FilterPalette::Type = Type;
   }

/*---------------------------------------------------------------------------
   Destructor. NOTE: Make sure it is called within a valid OpenGL context.
  ---------------------------------------------------------------------------*/
FilterPalette::~FilterPalette(void)
   {
   Destroy();
   }

/*---------------------------------------------------------------------------
   Clears the structure.
  ---------------------------------------------------------------------------*/
void FilterPalette::Clear(void)
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
void FilterPalette::Destroy(void)
   {
   Filter::Destroy();

   Palette.Destroy();

   Clear();
   }

/*---------------------------------------------------------------------------
   Class specific setup function for the effects assets.
  ---------------------------------------------------------------------------*/
void FilterPalette::Assets(Buffers &Buffer)
   {
   if (!Ready()) {return;}

   Depth.Create(Buffer.GetDepthResolution(), Buffer.GetDepthDataType());
   Depth.ClearData();
   Depth.Buffer(false);

   Model.Plane(1, Mesh::ModeSolid);
   Model.Buffer(false);

   File::PNG PNG;
   PNG.Load(Palette, File::Path::Shader(File::TexturePalette));
   Palette.SetMinFilter(Texture::MinNearest); //Disable filtering for palette texture in order to prevent bleeding into adjacent tables
   Palette.SetMagFilter(Texture::MagNearest);
   Palette.Buffer(false);

   File::Text Text;
   std::string CodeVert, CodeFrag;
   Text.Load(CodeVert, File::Path::Shader(File::FilterPaletteVert));
   Text.Load(CodeFrag, File::Path::Shader(File::FilterPaletteFrag));
   Program.Attach(CodeVert, Shader::ShaderVert);
   Program.Attach(CodeFrag, Shader::ShaderFrag);
   Program.Buffer(false);

   //Convert table selector to normalised texture coordinates
   float Select = (float)(Palette.Resolution().V - 1);
   if (Select > Math::feps) {Select = (float)Type / Select;}

   Program.Bind();
   glUniform1f(glGetUniformLocation(Program.ID(), "Select"), Select); 
   glUniform1i(glGetUniformLocation(Program.ID(), "Texture"), 0);   //Texture unit 0
   glUniform1i(glGetUniformLocation(Program.ID(), "Palette"), 1);   //Texture unit 1
   Program.Unbind();

   GLenum Error = glGetError();
   if (Error != GL_NO_ERROR) {throw dexception("OpenGL generated an error: %s", Debug::ErrorGL(Error));}
   }

/*---------------------------------------------------------------------------
   Renders the filter effect. Assumes the Bind( ) method was called prior.
  ---------------------------------------------------------------------------*/
void FilterPalette::Render(void)
   {
   if (!Ready()) {return;}

   glEnable(GL_TEXTURE_2D);

   glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
   glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

   Program.Bind();
   Model.Bind(2);
   Depth.Bind(0);
   Palette.Bind(1);
   Model.Render();
   Palette.Unbind(1);
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
