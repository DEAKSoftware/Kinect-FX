/*===========================================================================
   Common File I/O Objects and Assets

   Dominik Deak
  ===========================================================================*/

#ifndef ___FILE_H___
#define ___FILE_H___


/*---------------------------------------------------------------------------
   Header files
  ---------------------------------------------------------------------------*/
#include "file_png.h"
#include "file_text.h"
#include "file_tga.h"


//Namespaces
NAMESPACE_BEGIN(NAMESPACE_PROJECT)
NAMESPACE_BEGIN(File)


/*---------------------------------------------------------------------------
   Asset file names
  ---------------------------------------------------------------------------*/
//Asset subdirectorires, relative to executable
#if defined (WINDOWS)
   static const char* const TextureDir = "assets";
   static const char* const ShaderDir = "assets";
#elif defined (MACOSX)
   static const char* const TextureDir = "../Resources/assets";
   static const char* const ShaderDir = "../Resources/assets";
#else
   static const char* const TextureDir = "assets";
   static const char* const ShaderDir = "assets";
#endif

static const char* const FilterVert = "filter.vert";
static const char* const FilterFrag = "filter.frag";
static const char* const FilterPaletteVert = FilterVert;
static const char* const FilterPaletteFrag = "filter_palette.frag";
static const char* const FilterLinesVert = FilterVert;
static const char* const FilterLinesFrag = "filter_lines.frag";
static const char* const FilterFattyVert = "filter_fatty.vert";
static const char* const FilterFattyFrag = "filter_fatty.frag";
static const char* const FilterSolidsDepthVert = "filter_solids_depth.vert";
static const char* const FilterSolidsVideoVert = "filter_solids_video.vert";
static const char* const FilterSolidsFrag = "filter_solids.frag";
static const char* const FilterNMapVert = FilterVert;
static const char* const FilterNMapFrag = "filter_nmap.frag";

static const char* const TexturePalette = "palette.png";
static const char* const TextureLines = "lines.png";


/*---------------------------------------------------------------------------
   Classes
  ---------------------------------------------------------------------------*/
class Path
   {
   //---- Member data ----
   private:

   static std::string TexturePath;
   static std::string ShaderPath;

   //---- Methods ----
   public:

   Path(void) {}
   ~Path(void) {}

   private:

   Path(const Path &obj);                          //Disable
   Path &operator = (const Path &obj);             //Disable

   public:

   static void SetTexture(const std::string &NewPath);
   static void SetShader(const std::string &NewPath);
   static void SetTexture(const char* NewPath);
   static void SetShader(const char* NewPath);
   static std::string Texture(const std::string &FileName);
   static std::string Shader(const std::string &FileName);
   };


//Close namespaces
NAMESPACE_END(File)
NAMESPACE_END(NAMESPACE_PROJECT)


//==== End of file ===========================================================
#endif
