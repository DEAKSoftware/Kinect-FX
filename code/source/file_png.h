/*===========================================================================
   PNG File I/O

   Dominik Deak
  ===========================================================================*/

#ifndef ___FILE_PNG_H___
#define ___FILE_PNG_H___


/*---------------------------------------------------------------------------
   Header files
  ---------------------------------------------------------------------------*/
#include "array.h"
#include "common.h"
#include "texture.h"


//Namespaces
NAMESPACE_BEGIN(NAMESPACE_PROJECT)
NAMESPACE_BEGIN(File)


/*---------------------------------------------------------------------------
  PNG file I/O class
  ---------------------------------------------------------------------------*/
class PNG
   {
   //---- Constants and definitions ----
   public:

   enum FileMode                                   //Enumerations for file IO mode
      {
      ModeNull = 0,                                //File is not open 
      ModeRead = 1,                                //File was open for reading
      ModeWrite = 2                                //File was open for writing
      };

   enum CompLevel                                  //Compression level enumeration
      {
      CompLevel0 = 0,                              //No compression level
      CompLevel1 = 1,
      CompLevel2 = 2,
      CompLevel3 = 3,
      CompLevel4 = 4,
      CompLevel5 = 5,
      CompLevel6 = 6,
      CompLevel7 = 7,
      CompLevel8 = 8,
      CompLevel9 = 9,                              //Best compression level
      CompNone = Z_NO_COMPRESSION,                 //No compression level, equivalent to CompLevel0
      CompSpeed = Z_BEST_SPEED,                    //Compression with best performace, equivalent to CompLevel1
      CompDefault = Z_DEFAULT_COMPRESSION,         //Default compression level
      CompBest = Z_BEST_COMPRESSION                //Best compression level, equivalent to CompLevel9
      };

   static const usize HeaderSize = 8;              //Header size to test for PNG format

   //---- Member data ----
   private:

   FileMode Mode;
   FILE* File;                                     //File handle for the PNG image
   png_structp PngPtr;                             //PNG read structure
   png_infop InfoPtr;                              //PNG information structure
   Array<png_bytep, 16> Rows;                      //Row pointer array for decompressing the image

   //---- Methods ----
   public:

   PNG(void);
   ~PNG(void);

   private:

   PNG(const PNG &obj);                            //Disable
   PNG &operator = (const PNG &obj);               //Disable

   //Data allocation
   void Clear(void);
   void Destroy(void);

   public:

   void Load(Texture &Image, const std::string &Path, float Gamma = 2.2f);
   void Save(const Texture &Image, const std::string &Path, CompLevel Compress = PNG::CompDefault);
   };


//Close namespaces
NAMESPACE_END(File)
NAMESPACE_END(NAMESPACE_PROJECT)


//==== End of file ===========================================================
#endif
