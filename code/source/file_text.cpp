/*===========================================================================
   Text File I/O

   Dominik Deak
  ===========================================================================*/

#ifndef ___FILE_TEXT_CPP___
#define ___FILE_TEXT_CPP___


/*---------------------------------------------------------------------------
   Header files
  ---------------------------------------------------------------------------*/
#include "common.h"
#include "debug.h"
#include "file_text.h"


//Namespaces
NAMESPACE_BEGIN(NAMESPACE_PROJECT)
NAMESPACE_BEGIN(File)


/*---------------------------------------------------------------------------
   Constructor.
  ---------------------------------------------------------------------------*/
Text::Text(void)
   {
   Clear();
   }

/*---------------------------------------------------------------------------
   Destructor.
  ---------------------------------------------------------------------------*/
Text::~Text(void)
   {
   Destroy();
   }

/*---------------------------------------------------------------------------
   Clears the structure.
  ---------------------------------------------------------------------------*/
void Text::Clear(void)
   {
   }

/*---------------------------------------------------------------------------
   Destroys the structure.
  ---------------------------------------------------------------------------*/
void Text::Destroy(void)
   {
   if (File.is_open()) {File.close();}
   
   Clear();
   }

/*---------------------------------------------------------------------------
   Load a string buffer from a file.
  ---------------------------------------------------------------------------*/
void Text::Load(std::string &String, const std::string &Path)
   {
   Destroy();
   
   File.open(Path.c_str(), std::fstream::in);
   
   if (File.bad() || !File.is_open()) 
      {throw dexception("Failed to open \"%s\".", Path.c_str());}

   File.seekg(0, std::fstream::end);
   
   if ((usize)File.tellg() > Text::MaxSize) 
      {throw dexception("File size exceeds limit (%d) for \"%s\".", Text::MaxSize, Path.c_str());}
   
   File.seekg(0);

   std::string Line;

   while (File.good())
      {
      std::getline(File, Line);
      String += Line;
      String += "\n";   //Preserve newline character in source
      }

   Destroy();
   }

/*---------------------------------------------------------------------------
   Saves a text file.
  ---------------------------------------------------------------------------*/
void Text::Save(const std::string &String, const std::string &Path)
   {
   Destroy();

   File.open(Path.c_str(), std::fstream::out);
   
   if (File.bad() || !File.is_open()) {throw dexception("Failed to open \"%s\".", Path.c_str());}

   File << String;

   Destroy();
   }


//Close namespaces
NAMESPACE_END(File)
NAMESPACE_END(NAMESPACE_PROJECT)


//==== End of file ===========================================================
#endif
