/*===========================================================================
   Effects for Rendering Geometric Shapes

   Dominik Deak
  ===========================================================================*/

#ifndef ___FILTER_SOLIDS_CPP___
#define ___FILTER_SOLIDS_CPP___


/*---------------------------------------------------------------------------
   Header files
  ---------------------------------------------------------------------------*/
#include "common.h"
#include "debug.h"
#include "filter_solids.h"


//Namespaces
NAMESPACE_BEGIN(NAMESPACE_PROJECT)


/*---------------------------------------------------------------------------
   Constructor.
  ---------------------------------------------------------------------------*/
FilterSolids::FilterSolids(SolidType Type)
   {
   Clear();
   
   FilterSolids::Type = Type;

   switch (Type)
      {
      case FilterSolids::Cubes :
      case FilterSolids::Spheres :
      case FilterSolids::CubesFar :
      case FilterSolids::SpheresFar :
         EnableVideo = false;
         EnableDepth = true;
         EnableCal = false;
         break;

      case FilterSolids::CubesTinted :
      case FilterSolids::SpheresTinted :
         EnableVideo = true;
         EnableDepth = true;
         EnableCal = true;
         break;

      default : dexception("Unknown filter enumeration type.");
      }
   }

/*---------------------------------------------------------------------------
   Destructor.
  ---------------------------------------------------------------------------*/
FilterSolids::~FilterSolids(void)
   {
   Destroy();
   }

/*---------------------------------------------------------------------------
   Clears the structure.
  ---------------------------------------------------------------------------*/
void FilterSolids::Clear(void)
   {
   Filter::Clear();

   Mat.SetAmbient(vector4f(0.4f, 0.3f, 0.3f, 1.0f));
   Mat.SetDiffuse(vector4f(0.9f, 0.9f, 0.9f, 1.0f));
   Mat.SetSpecular(vector4f(0.6f, 0.6f, 0.5f, 0.0f));
   Mat.SetShininess(45.0f);

   Grid = 1;
   GridNorm = 1.0f;
   Offset = 0.0f;
   UID = 0;

   Near = -1.0f;
   Far = -7.0f;
   Range = Math::Abs(Far - Near);

   Select = Filter::SelectDepth;
   EnableVideo = false;
   EnableDepth = true;
   EnableColour = true;
   }

/*---------------------------------------------------------------------------
   Destroys the structure.
  ---------------------------------------------------------------------------*/
void FilterSolids::Destroy(void)
   {
   Filter::Destroy();

   Clear();
   }

/*---------------------------------------------------------------------------
   Class specific setup function for the effects assets.
  ---------------------------------------------------------------------------*/
void FilterSolids::Assets(Buffers &Buffer)
   {
   if (!Ready()) {return;}

   vector2u Res = Buffer.GetDepthResolution();
   Depth.Create(Res, Buffer.GetDepthDataType());
   Depth.ClearData();
   Depth.Buffer(false);

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   glPushMatrix();
   Light.SetPosition(vector4f(0.5f, -0.5f, Near + 1.0f, 0.0f));
   Light.Buffer(0);
   glPopMatrix();

   Grid = Res / GridDiv;
   Grid = Grid.Clamp(GridMin, GridMax);
   GridNorm = cast_vector2(float, Grid);
   GridNorm = GridNorm.Rcp();
   uint GridSmallest = Math::Min(Grid.U, Grid.V);

   Offset = cast_vector2(float, Grid);
   Offset = (Offset - 1.0f) * 0.5f;

   //Geometry subdivisions for curved solids
   uint PolyDiv = (Math::Min(Res.U, Res.V) / GridSmallest) / PolyDivPixels;
   PolyDiv = Math::Clamp(PolyDiv, PolyDivMin, PolyDivMax);

   File::Text Text;
   std::string CodeVert, CodeFrag;

   switch (Type)
      {
      case FilterSolids::Cubes :
         Model.Cube(1, Mesh::ModeSolid);
         Text.Load(CodeVert, File::Path::Shader(File::FilterSolidsDepthVert));
         EnableVideo = false;
         EnableDepth = true;
         EnableCal = false;
         Mat.SetShininess(0.0f);
         break;

      case FilterSolids::Spheres :
         Model.Sphere(PolyDiv, Mesh::ModeSolid);
         Text.Load(CodeVert, File::Path::Shader(File::FilterSolidsDepthVert));
         EnableVideo = false;
         EnableDepth = true;
         EnableCal = false;
         break;

      case FilterSolids::CubesTinted :
         Model.Cube(1, Mesh::ModeSolid);
         Text.Load(CodeVert, File::Path::Shader(File::FilterSolidsVideoVert));
         Video.Create(Buffer.GetVideoResolution(), Buffer.GetVideoDataType());
         Video.ClearData();
         Video.Buffer(false);
         EnableVideo = true;
         EnableDepth = true;
         EnableCal = true;
         Mat.SetShininess(0.0f);
         break;

      case FilterSolids::SpheresTinted :
         Model.Sphere(PolyDiv, Mesh::ModeSolid);
         Text.Load(CodeVert, File::Path::Shader(File::FilterSolidsVideoVert));
         Video.Create(Buffer.GetVideoResolution(), Buffer.GetVideoDataType());
         Video.ClearData();
         Video.Buffer(false);
         EnableVideo = true;
         EnableDepth = true;
         EnableCal = true;
         break;

      case FilterSolids::CubesFar :
         Model.Cube(1, Mesh::ModeSolid);
         Text.Load(CodeVert, File::Path::Shader(File::FilterSolidsDepthVert));
         EnableVideo = false;
         EnableDepth = true;
         EnableCal = false;
         Mat.SetShininess(0.0f);
         Far = -1000.0f;
         break;

      case FilterSolids::SpheresFar :
         Model.Sphere(PolyDiv, Mesh::ModeSolid);
         Text.Load(CodeVert, File::Path::Shader(File::FilterSolidsDepthVert));
         EnableVideo = false;
         EnableDepth = true;
         EnableCal = false;
         Far = -1000.0f;
         break;

      default : dexception("Unknown filter enumeration type.");
      }

   Range = Math::Abs(Far - Near);

   Model.Buffer(false);

   Text.Load(CodeFrag, File::Path::Shader(File::FilterSolidsFrag));
   Program.Attach(CodeVert, Shader::ShaderVert);
   Program.Attach(CodeFrag, Shader::ShaderFrag);
   Program.Buffer(false);

   Program.Bind();
   glUniform1f(glGetUniformLocation(Program.ID(), "Range"), Math::Abs(Range));
   glUniform1i(glGetUniformLocation(Program.ID(), "Depth"), 0);   //Texture unit 0
   glUniform1i(glGetUniformLocation(Program.ID(), "Video"), 1);   //Texture unit 1
   UID = glGetUniformLocation(Program.ID(), "TexCoord");
   glUniform2f(UID, 0.0f, 0.0f);
   Program.Unbind();

   //Projection matrix
   MP = MP.Identity();
   MP = MP.Frustum(-Ratio.X, Ratio.X, -Ratio.Y, Ratio.Y, Math::Abs(Near), Math::Abs(Far));

   //Modelview matrix
   float Scale = 1.0f / (float)GridSmallest;
   MV = MV.Identity();
   MV = MV.Scale(Scale, Scale, Scale);
   MV = MV.Translate(0.0f, 0.0f, (Near - Scale - 0.01f) / Scale);

   GLenum Error = glGetError();
   if (Error != GL_NO_ERROR) {throw dexception("OpenGL generated an error: %s", Debug::ErrorGL(Error));}
   }

/*---------------------------------------------------------------------------
   Renders the filter effect. Assumes the Bind( ) method was called prior.
  ---------------------------------------------------------------------------*/
void FilterSolids::Render(void)
   {
   if (!Ready()) {return;}

   glEnable(GL_TEXTURE_2D);
   glEnable(GL_LIGHTING);
   glEnable(GL_BLEND);

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
   Model.Bind(0);
   Depth.Bind(0);       
   Video.Bind(1);

   glMatrixMode(GL_MODELVIEW);

   for (uint Y = 0; Y < Grid.Y; Y++)
      {
      for (uint X = 0; X < Grid.X; X++)
         {
         vector2f Coord((float)X, (float)Y);
         Coord = (Coord - Offset) * 2.0f;

         vector2f TexCoord((float)X, (float)Y);
         TexCoord *= GridNorm;
         glUniform2f(UID, TexCoord.X, TexCoord.Y);

         glPushMatrix();
         glTranslatef(Coord.X, Coord.Y, 0.0f);
         Model.Render();
         glPopMatrix();
         }
      }

   Video.Unbind(1);
   Depth.Unbind(0);
   Model.Unbind(0);
   Program.Unbind();

   Light.Unbind();

   glMatrixMode(GL_TEXTURE);
   glActiveTexture(GL_TEXTURE1);
   glPopMatrix();

   glDisable(GL_BLEND);
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
