/*===========================================================================
   Common File I/O Objects and Assets

   Dominik Deak
  ===========================================================================*/

#ifndef ___FILE_CPP___
#define ___FILE_CPP___


/*---------------------------------------------------------------------------
   Header files
  ---------------------------------------------------------------------------*/
#include "common.h"
#include "debug.h"
#include "file.h"


//Namespaces
NAMESPACE_BEGIN(NAMESPACE_PROJECT)
NAMESPACE_BEGIN(File)


/*---------------------------------------------------------------------------
   Static data.
  ---------------------------------------------------------------------------*/
std::string Path::TexturePath = ".";
std::string Path::ShaderPath = ".";


/*---------------------------------------------------------------------------
   Sets a particular asset path. Note, new path must not have traling slash.
  ---------------------------------------------------------------------------*/
void Path::SetTexture(const std::string &NewPath)
   {
   if (NewPath.size() < 1) {return;}
   TexturePath = NewPath;
   }

void Path::SetShader(const std::string &NewPath)
   {
   if (NewPath.size() < 1) {return;}
   ShaderPath = NewPath;
   }

void Path::SetTexture(const char* NewPath)
   {
   if (NewPath == nullptr) {return;}
   TexturePath = NewPath;
   }

void Path::SetShader(const char* NewPath)
   {
   if (NewPath == nullptr) {return;}
   ShaderPath = NewPath;
   }

/*---------------------------------------------------------------------------
   Returns the full path for an asset file.
  ---------------------------------------------------------------------------*/
std::string Path::Texture(const std::string &FileName)
   {
   return TexturePath + "/" + FileName;
   }

std::string Path::Shader(const std::string &FileName)
   {
   return TexturePath + "/" + FileName;
   }


//Close namespaces
NAMESPACE_END(File)
NAMESPACE_END(NAMESPACE_PROJECT)


//==== End of file ===========================================================
#endif
