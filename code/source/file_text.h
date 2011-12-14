/*===========================================================================
   Text File I/O

   Dominik Deak
  ===========================================================================*/

#ifndef ___FILE_TEXT_H___
#define ___FILE_TEXT_H___


/*---------------------------------------------------------------------------
   Header files
  ---------------------------------------------------------------------------*/
#include "common.h"


//Namespaces
NAMESPACE_BEGIN(NAMESPACE_PROJECT)
NAMESPACE_BEGIN(File)


/*---------------------------------------------------------------------------
  Classes
  ---------------------------------------------------------------------------*/
class Text
   {
   //---- Constants and definitions ----
   public:

   static const usize MaxSize = 1 << 24;           //Maximum file size that can be read

   //---- Member data ----
   private:

   std::fstream File;                              //The file stream

   //---- Methods ----
   public:

   Text(void);
   ~Text(void);

   private:

   Text(const Text &obj);                          //Disable
   Text &operator = (const Text &obj);             //Disable

   //Data allocation
   void Clear(void);
   void Destroy(void);

   public:

   void Load(std::string &String, const std::string &Path);
   void Save(const std::string &String, const std::string &Path);
   };


//Close namespaces
NAMESPACE_END(File)
NAMESPACE_END(NAMESPACE_PROJECT)


//==== End of file ===========================================================
#endif
