/*===========================================================================
   TGA File I/O

   Dominik Deak
  ===========================================================================*/

#ifndef ___FILE_TGA_H___
#define ___FILE_TGA_H___


/*---------------------------------------------------------------------------
   Header files
  ---------------------------------------------------------------------------*/
#include "common.h"
#include "texture.h"


//Namespaces
NAMESPACE_BEGIN(NAMESPACE_PROJECT)
NAMESPACE_BEGIN(File)


/*---------------------------------------------------------------------------
   Classes
  ---------------------------------------------------------------------------*/
class TGA
   {
   //---- Colour conversion classes ----
   private:

   class Colour
      {
      public:
      virtual void Decode(uint8* Dst, const uint8* Src) const;
      virtual void Encode(uint8* Dst, const uint8* Src) const;
      };

   class Colour15 : public Colour
      {
      public:
      void Decode(uint8* Dst, const uint8* Src) const;
      void Encode(uint8* Dst, const uint8* Src) const;
      };

   class Colour24 : public Colour
      {
      public:
      void Decode(uint8* Dst, const uint8* Src) const;
      void Encode(uint8* Dst, const uint8* Src) const;
      };

   class Colour32 : public Colour
      {
      public:
      void Decode(uint8* Dst, const uint8* Src) const;
      void Encode(uint8* Dst, const uint8* Src) const;
      };

   class Grey16 : public Colour
      {
      public:
      void Decode(uint8* Dst, const uint8* Src) const;
      void Encode(uint8* Dst, const uint8* Src) const;
      };

   //Declare colour conversion classes
   TGA::Colour Colour8;
   TGA::Colour15 Colour15;
   TGA::Colour24 Colour24;
   TGA::Colour32 Colour32;
   TGA::Grey16 Grey16;

   //---- Constants and definitions ----
   public:

   static const usize MaxRes = 65535;              //Maximum resolution

   enum ImageType                                  //Codes 0 to 127 are Truevision specific, 128 to 255 can be application specific
      {
      TypeNone = 0x00,                             //No image data
      TypeCMap = 0x01,                             //Colour mapped
      TypeTrue = 0x02,                             //True colour
      TypeGrey = 0x03,                             //Grey scale
      TypeCMapRLE = 0x09,                          //Colour mapped, with RLE compression
      TypeTrueRLE = 0x0A,                          //True colour, with RLE compression
      TypeGreyRLE = 0x0B                           //Grey scale, with RLE compression
      };

   enum ImageDesc                                  //Image decriptor masks
      {
      MaskAttrib = 0x0F,                           //Number of attribute bits per pixel (alpha channel bits)
      MaskFlipU = 0x10,                            //Right-to-left mask
      MaskFlipV = 0x20                             //Top-to-bottom mask
      };

   struct FileHeader                               //File header structure
      {
      uint8 ID_FieldSize;                          //ID field length
      uint8 ColMapType;                            //Colour map type
      uint8 ImageType;                             //Image type
      uint16 ColMapEntOffset;                      //Offset of the first colour map entry in the table
      uint16 ColMapEntCount;                       //Number of entries in the colour map
      uint8 ColMapEntSize;                         //Establishes the number of bits per entry, typically 15, 16, 24 or 32-bit
      uint16 OriginU;                              //Absolute horizontal coordinate for the lower left corner of the image
      uint16 OriginV;                              //Absolute vertical coordinate for the lower left corner of the image
      uint16 ResU;                                 //Width of the image in pixels
      uint16 ResV;                                 //Height of the image in pixels
      uint8 BitsPerPixel;                          //Bits per pixel
      uint8 ImageDesc;                             //Image descriptor bits
      };

   struct Iterator                                 //Image iterator for handling all orientations
      {
      vector2<iter> S;                             //Start coordinate
      vector2<iter> E;                             //End coordinate, not inclusive in image area
      vector2<iter> I;                             //Increment size
      };

   //---- Member data ----
   private:

   std::fstream File;
   FileHeader Header;
   Array<uint8, 32> ColourMap;

   //---- Methods ----
   public:

   TGA(void);
   ~TGA(void);

   private:

   TGA(const TGA &obj);                            //Disable
   TGA &operator = (const TGA &obj);               //Disable

   //Data allocation
   void Clear(void);
   void Destroy(void);

   //File read
   void ReadHeader(FileHeader &Header);
   void ReadColourMap(const TGA::Colour &Colour);
   void ReadColourMapDecode(Texture &Image);
   void ReadImage(Texture &Image, const TGA::Iterator &Iter, const TGA::Colour &Colour);
   void ReadImageDecode(Texture &Image, const TGA::Iterator &Iter, const TGA::Colour &Colour);

   void SaveHeader(FileHeader Header);
   void SaveImage(const Texture &Image, const TGA::Iterator &Iter, const TGA::Colour &Colour);
   bool ComparePixel(const uint8* Dst, const uint8* Src, const usize BytesPerBixel);
   bool EncodeCount(const Texture &Image, const TGA::Iterator &Iter, vector2<iter> P, usize &Count);
   void SaveImageEncode(const Texture &Image, const TGA::Iterator &Iter, const TGA::Colour &Colour);

   public:

   void Load(Texture &Image, const std::string &Path);
   void Save(const Texture &Image, const std::string &Path, const vector2b &Flip = false, bool Compress = true);
   };


//Close namespaces
NAMESPACE_END(File)
NAMESPACE_END(NAMESPACE_PROJECT)


//==== End of file ===========================================================
#endif
