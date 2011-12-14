/*===========================================================================
   TGA File I/O

   Dominik Deak
  ===========================================================================*/

#ifndef ___FILE_TGA_CPP___
#define ___FILE_TGA_CPP___


/*---------------------------------------------------------------------------
   Header files
  ---------------------------------------------------------------------------*/
#include "common.h"
#include "debug.h"
#include "file_tga.h"


//Namespaces
NAMESPACE_BEGIN(NAMESPACE_PROJECT)
NAMESPACE_BEGIN(File)


/*---------------------------------------------------------------------------
   Constructor.
  ---------------------------------------------------------------------------*/
TGA::TGA(void)
   {
   Clear();
   }

/*---------------------------------------------------------------------------
   Destructor.
  ---------------------------------------------------------------------------*/
TGA::~TGA(void)
   {
   Destroy();
   }

/*---------------------------------------------------------------------------
   Clears the structure.
  ---------------------------------------------------------------------------*/
void TGA::Clear(void)
   {
   Header.ID_FieldSize = 0;
   Header.ColMapType = 0;
   Header.ImageType = 0;
   Header.ColMapEntOffset = 0;
   Header.ColMapEntCount = 0;
   Header.ColMapEntSize = 0;
   Header.OriginU = 0;
   Header.OriginV = 0;
   Header.ResU = 0;
   Header.ResV = 0;
   Header.BitsPerPixel = 0;
   Header.ImageDesc = 0;
   }

/*---------------------------------------------------------------------------
   Destroys the structure.
  ---------------------------------------------------------------------------*/
void TGA::Destroy(void)
   {
   if (File.is_open()) {File.close();}
   
   ColourMap.Destroy();

   Clear();
   }

/*---------------------------------------------------------------------------
   Colour converision base class, handles 8-bits by default.
  ---------------------------------------------------------------------------*/
void TGA::Colour::Decode(uint8* Dst, const uint8* Src) const {*Dst = *Src;}
void TGA::Colour::Encode(uint8* Dst, const uint8* Src) const {*Dst = *Src;}

/*---------------------------------------------------------------------------
   15-bit encoding format: ABBBBBGG GGGRRRRR.
  ---------------------------------------------------------------------------*/
void TGA::Colour15::Decode(uint8* Dst, const uint8* Src) const 
   {
   Dst[0] = (uint8)((*reinterpret_cast<const uint16*>(Src) & 0x7C00) >> 7);
   Dst[1] = (uint8)((*reinterpret_cast<const uint16*>(Src) & 0x03E0) >> 2);
   Dst[2] = (uint8)((*reinterpret_cast<const uint16*>(Src) & 0x001F) << 3);
   }

void TGA::Colour15::Encode(uint8* Dst, const uint8* Src) const 
   {
   *reinterpret_cast<uint16*>(Dst) = (((uint16)Src[0] << 7) & 0x7C00) | (((uint16)Src[1] << 2) & 0x03E0) | ((uint16)Src[2] >> 3);
   }

/*---------------------------------------------------------------------------
   24-bit encoding format: BBBBBBBB GGGGGGGG RRRRRRRR.
  ---------------------------------------------------------------------------*/
void TGA::Colour24::Decode(uint8* Dst, const uint8* Src) const 
   {
   Dst[0] = Src[2]; Dst[1] = Src[1]; Dst[2] = Src[0];
   }

void TGA::Colour24::Encode(uint8* Dst, const uint8* Src) const 
   {
   Dst[0] = Src[2]; Dst[1] = Src[1]; Dst[2] = Src[0];
   }

/*---------------------------------------------------------------------------
   32-bit encoding format: BBBBBBBB GGGGGGGG RRRRRRRR AAAAAAAA.
  ---------------------------------------------------------------------------*/
void TGA::Colour32::Decode(uint8* Dst, const uint8* Src) const 
   {
   Dst[0] = Src[2]; Dst[1] = Src[1]; Dst[2] = Src[0]; Dst[3] = Src[3];
   }

void TGA::Colour32::Encode(uint8* Dst, const uint8* Src) const 
   {
   Dst[0] = Src[2]; Dst[1] = Src[1]; Dst[2] = Src[0]; Dst[3] = Src[3];
   }

/*---------------------------------------------------------------------------
   16-bit grey scale encoding format: AAAAAAAA AAAAAAAA.
  ---------------------------------------------------------------------------*/
void TGA::Grey16::Decode(uint8* Dst, const uint8* Src) const 
   {
   *reinterpret_cast<uint16*>(Dst) = *reinterpret_cast<const uint16*>(Src);
   }

void TGA::Grey16::Encode(uint8* Dst, const uint8* Src) const 
   {
   *reinterpret_cast<uint16*>(Dst) = *reinterpret_cast<const uint16*>(Src);
   }                                

/*---------------------------------------------------------------------------
   Reads the file header.
  ---------------------------------------------------------------------------*/
void TGA::ReadHeader(FileHeader &Header)
   {
   File.read(reinterpret_cast<char*>(&Header.ID_FieldSize), 1);
   File.read(reinterpret_cast<char*>(&Header.ColMapType), 1);
   File.read(reinterpret_cast<char*>(&Header.ImageType), 1);
   File.read(reinterpret_cast<char*>(&Header.ColMapEntOffset), 2);
   File.read(reinterpret_cast<char*>(&Header.ColMapEntCount), 2);
   File.read(reinterpret_cast<char*>(&Header.ColMapEntSize), 1);
   File.read(reinterpret_cast<char*>(&Header.OriginU), 2);
   File.read(reinterpret_cast<char*>(&Header.OriginV), 2);
   File.read(reinterpret_cast<char*>(&Header.ResU), 2);
   File.read(reinterpret_cast<char*>(&Header.ResV), 2);
   File.read(reinterpret_cast<char*>(&Header.BitsPerPixel), 1);
   File.read(reinterpret_cast<char*>(&Header.ImageDesc), 1);
   if (File.bad()) {throw dexception("File I/O error.");}

   if (!Math::MachineLittleEndian())
      {
      Header.ColMapEntOffset = Math::SwapEndian(Header.ColMapEntOffset);
      Header.ColMapEntCount = Math::SwapEndian(Header.ColMapEntCount);
      Header.OriginU = Math::SwapEndian(Header.OriginU);
      Header.OriginV = Math::SwapEndian(Header.OriginV);
      Header.ResU = Math::SwapEndian(Header.ResU);
      Header.ResV = Math::SwapEndian(Header.ResV);
      }

   //Skip ID field after the header
   File.seekg(Header.ID_FieldSize, std::fstream::cur);
   if (File.bad()) {throw dexception("File I/O error.");}
   }

/*---------------------------------------------------------------------------
   Reads the colour map.
  ---------------------------------------------------------------------------*/
void TGA::ReadColourMap(const TGA::Colour &Colour)
   {
   //Skip to the first palette entry
   File.seekg(Header.ColMapEntOffset, std::fstream::cur);
   if (File.bad()) {throw dexception("File I/O error.");}

   usize ByterPerEntry = Math::ByteSize((usize)Header.ColMapEntSize);

   ColourMap.Destroy();
   ColourMap.Create((usize)Header.ColMapEntCount * ByterPerEntry);
   
   uint8 Src[4];
   uint8* Dst = ColourMap.Pointer();
   uint8* End = Dst + ColourMap.Size();

   while (Dst < End)
      {
      File.read(reinterpret_cast<char*>(Src), ByterPerEntry);
      if (File.bad()) {throw dexception("File I/O error.");}

      Colour.Decode(Dst, Src);

      Dst += ByterPerEntry;
      }
   }

/*---------------------------------------------------------------------------
   Applies a post processing step on the image for decoding the colour map. 
   It is assumed that the colour table index values are stored in the red 
   channel of the image, which is then used to fetch the actual colour from 
   the table and subsituted with the image pixel.
  ---------------------------------------------------------------------------*/
void TGA::ReadColourMapDecode(Texture &Image)
   {
   const usize ByterPerPixel = Image.GetBytesPerPixel();
   const usize ByterPerLine = Image.GetBytesPerLine();
   
   uint8* ScanLinePtr = Image.Pointer();
   uint8* ScanLineEnd = ScanLinePtr + ByterPerLine * Image.Resolution().V;
   
   while (ScanLinePtr < ScanLineEnd)
      {
      uint8* Ptr = ScanLinePtr;
      uint8* End = ScanLinePtr + ByterPerLine;

      while (Ptr < End)
         {
         uiter I = 0;
         uiter J = (usize)*Ptr * ByterPerPixel; //Colour table offset
         
         while (I < ByterPerPixel) 
            {
            Ptr[I] = ColourMap[J];
            I++; 
            J++;
            }
         
         Ptr += ByterPerPixel;
         }

      ScanLinePtr = End;
      }
   }

/*---------------------------------------------------------------------------
   Reads uncompressed image data from file.
  ---------------------------------------------------------------------------*/
void TGA::ReadImage(Texture &Image, const TGA::Iterator &Iter, const TGA::Colour &Colour)
   {
   const usize BytesPerBixel = Math::ByteSize((usize)Header.BitsPerPixel);
   uint8* Src, *Dst;
   vector2<iter> P;

   Array<uint8, 16> Buffer;
   Buffer.Create(BytesPerBixel * Image.Resolution().U);

   for (P.V = Iter.S.V; P.V != Iter.E.V; P.V += Iter.I.V)
      {
      Src = Buffer.Pointer();
      
      File.read(reinterpret_cast<char*>(Src), Buffer.Size());
      if (File.bad()) {throw dexception("File I/O error.");}
      
      for (P.U = Iter.S.U; P.U != Iter.E.U; P.U += Iter.I.U)
         {
         Dst = Image.Address((uint)P.U, (uint)P.V);
         Colour.Decode(Dst, Src);
         Src += BytesPerBixel;
         }
      }
   }

/*---------------------------------------------------------------------------
   Reads run-lenght encoded image data from file.
  ---------------------------------------------------------------------------*/
void TGA::ReadImageDecode(Texture &Image, const TGA::Iterator &Iter, const TGA::Colour &Colour)
   {
   const usize BytesPerBixel = Math::ByteSize((usize)Header.BitsPerPixel);
   uint8 *Src, *Dst, Code;
   vector2<iter> P;

   Array<uint8, 16> Buffer;
   Buffer.Create(BytesPerBixel * Image.Resolution().U);

   for (P.V = Iter.S.V; P.V != Iter.E.V; P.V += Iter.I.V)
      {
      for (P.U = Iter.S.U; P.U != Iter.E.U;)
         {
         File.read(reinterpret_cast<char*>(&Code), 1);
         if (File.bad()) {throw dexception("File I/O error.");}
         
         usize Count = ((uiter)Code & 0x7F) + 1;

         //Run-length packet, pixels are repeating
         if (Code & 0x80)
            {
            Src = Buffer.Pointer();

            File.read(reinterpret_cast<char*>(Src), BytesPerBixel);
            if (File.bad()) {throw dexception("File I/O error.");}

            for (uiter I = 0; I < Count && P.U != Iter.E.U; I++, P.U += Iter.I.U)
               {
               Dst = Image.Address((uint)P.U, (uint)P.V);
               Colour.Decode(Dst, Src);
               }
            }

         //Raw packet, pixels are unique
         else
            {
            Src = Buffer.Pointer();

            File.read(reinterpret_cast<char*>(Src), BytesPerBixel * Count);
            if (File.bad()) {throw dexception("File I/O error.");}
            
            for (uiter I = 0; I < Count && P.U != Iter.E.U; I++, P.U += Iter.I.U)
               {
               Dst = Image.Address((uint)P.U, (uint)P.V);
               Colour.Decode(Dst, Src);
               Src += BytesPerBixel;
               }
            }
         }
      }
   }

/*---------------------------------------------------------------------------
   Loads a TGA file.

   Image : Image will be returned there.
   Path  : Path to image file, relative to the current working directory.
  ---------------------------------------------------------------------------*/
void TGA::Load(Texture &Image, const std::string &Path)
   {
   Destroy();

   File.open(Path.c_str(), std::fstream::in | std::fstream::binary);
   
   if (File.bad() || !File.is_open()) 
      {throw dexception("Failed to open \"%s\".", Path.c_str());}

   bool Decompress = false;
   bool DecodeMap = false;
   
   Texture::TexType Type = Texture::TypeRGB;
   
   TGA::Colour* ImageColour = &Colour8;
   TGA::Colour* PaletteColour = &Colour8;

   ReadHeader(Header);

   switch (Header.ImageType)
      {
      case TGA::TypeNone : throw dexception("File contains no image.");

      case TGA::TypeCMap :
      case TGA::TypeCMapRLE :
         {
         if (Header.ColMapType != 1) 
            {throw dexception("File contains an unsupported colour map type.");}
         
         if (Header.BitsPerPixel != 8) 
            {throw dexception("Currently only 256-colour palettes are supported.");}
      
         switch (Header.ColMapEntSize)
            {
            case 15 : Type = Texture::TypeRGB; PaletteColour = &Colour15; break;
            case 16 : Type = Texture::TypeRGB; PaletteColour = &Colour15; break;
            case 24 : Type = Texture::TypeRGB; PaletteColour = &Colour24; break;
            case 32 : Type = Texture::TypeRGBA; PaletteColour = &Colour32; break;
            default : throw dexception("File contains an unsupported colour map encoding.");
            }

         ImageColour = &Colour8;
         Decompress = Header.ImageType == TGA::TypeCMapRLE;
         DecodeMap = true;
         
         break;
         }
      
      case TGA::TypeTrue :
      case TGA::TypeTrueRLE :
         {
         switch (Header.BitsPerPixel)
            {
            case 8  : Type = Texture::TypeLum; ImageColour = &Colour8; break;
            case 15 : Type = Texture::TypeRGB; ImageColour = &Colour15; break;
            case 16 : Type = Texture::TypeRGB; ImageColour = &Colour15; break;
            case 24 : Type = Texture::TypeRGB; ImageColour = &Colour24; break;
            case 32 : Type = Texture::TypeRGBA; ImageColour = &Colour32; break;
            default : throw dexception("File contains an image with unsupported bits per pixel.");
            }

         Decompress = Header.ImageType == TGA::TypeTrueRLE;

         break;
         }
      
      case TGA::TypeGrey :
      case TGA::TypeGreyRLE :
         {
         if (Header.BitsPerPixel != 8) 
            {throw dexception("Currently only 8-bit grey scale images are supported.");}
      
         Type = Texture::TypeLum;
         ImageColour = &Colour8; 
         Decompress = Header.ImageType == TGA::TypeGreyRLE;
         break;
         }
      
      default : throw dexception("File contains unkown image type.");
      }

   //Allocate image
   Image.Create(vector2u(Header.ResU, Header.ResV), Type);

   //Flip direction
   vector2b Flip;
   Flip.U = (Header.ImageDesc & TGA::MaskFlipU) != 0;
   Flip.V = (Header.ImageDesc & TGA::MaskFlipV) == 0; //Always invert as Texture object origin is at top left

   //Setup iterators
   TGA::Iterator Iter;
   vector2<iter> Res = cast_vector2(iter, Image.Resolution());
   Iter.S.U = Flip.U ? Res.U - 1 : 0;
   Iter.S.V = Flip.V ? Res.V - 1 : 0;
   Iter.E.U = Flip.U ? -1 : Res.U;
   Iter.E.V = Flip.V ? -1 : Res.V;
   Iter.I.U = Flip.U ? -1 : 1;
   Iter.I.V = Flip.V ? -1 : 1;

   //Read and decode image data
   if (DecodeMap) {ReadColourMap(*PaletteColour);}
   if (Decompress) {ReadImageDecode(Image, Iter, *ImageColour);} else {ReadImage(Image, Iter, *ImageColour);}
   if (DecodeMap) {ReadColourMapDecode(Image);}

   Destroy();
   }

/*---------------------------------------------------------------------------
   Saves the file header. Note, the Header is passed as a copy, because some 
   requires modification before saving.
  ---------------------------------------------------------------------------*/
void TGA::SaveHeader(FileHeader Header)
   {
   if (!Math::MachineLittleEndian())
      {
      Header.ColMapEntOffset = Math::SwapEndian(Header.ColMapEntOffset);
      Header.ColMapEntCount = Math::SwapEndian(Header.ColMapEntCount);
      Header.OriginU = Math::SwapEndian(Header.OriginU);
      Header.OriginV = Math::SwapEndian(Header.OriginV);
      Header.ResU = Math::SwapEndian(Header.ResU);
      Header.ResV = Math::SwapEndian(Header.ResV);
      }

   File.write(reinterpret_cast<char*>(&Header.ID_FieldSize), 1);
   File.write(reinterpret_cast<char*>(&Header.ColMapType), 1);
   File.write(reinterpret_cast<char*>(&Header.ImageType), 1);
   File.write(reinterpret_cast<char*>(&Header.ColMapEntOffset), 2);
   File.write(reinterpret_cast<char*>(&Header.ColMapEntCount), 2);
   File.write(reinterpret_cast<char*>(&Header.ColMapEntSize), 1);
   File.write(reinterpret_cast<char*>(&Header.OriginU), 2);
   File.write(reinterpret_cast<char*>(&Header.OriginV), 2);
   File.write(reinterpret_cast<char*>(&Header.ResU), 2);
   File.write(reinterpret_cast<char*>(&Header.ResV), 2);
   File.write(reinterpret_cast<char*>(&Header.BitsPerPixel), 1);
   File.write(reinterpret_cast<char*>(&Header.ImageDesc), 1);
   if (File.bad()) {throw dexception("File I/O error.");}
   }

/*---------------------------------------------------------------------------
   Saves an uncompressed image data to file.
  ---------------------------------------------------------------------------*/
void TGA::SaveImage(const Texture &Image, const TGA::Iterator &Iter, const TGA::Colour &Colour)
   {
   const usize BytesPerBixel = Math::ByteSize((usize)Header.BitsPerPixel);
   uint8* Dst, *Src;
   vector2<iter> P;

   Array<uint8, 16> Buffer;
   Buffer.Create(BytesPerBixel * Image.Resolution().U);

   for (P.V = Iter.S.V; P.V != Iter.E.V; P.V += Iter.I.V)
      {
      Dst = Buffer.Pointer();

      for (P.U = Iter.S.U; P.U != Iter.E.U; P.U += Iter.I.U)
         {
         Src = Image.Address((uint)P.U, (uint)P.V);
         Colour.Encode(Dst, Src);
         Dst += BytesPerBixel;
         }

      File.write(reinterpret_cast<char*>(Buffer.Pointer()), Buffer.Size());
      if (File.bad()) {throw dexception("File I/O error.");}
      }
   }

/*---------------------------------------------------------------------------
   Compares two pixels and returns true if they are equal.
  ---------------------------------------------------------------------------*/
bool TGA::ComparePixel(const uint8* Ptr0, const uint8* Ptr1, const usize BytesPerBixel)
   {
   for (usize I = 0; I < BytesPerBixel; I++)
      {
      if (Ptr0[I] != Ptr1[I]) {return false;}
      }
   
   return true;
   }

/*---------------------------------------------------------------------------
   Determines the run-length of a scanline. It returns true if the Count 
   represents repeating pixels, returns false if the Count represents a group 
   of non-repeating pixels. Note, the repetition count is one less than the 
   actual number of pixels in the group, as specified by the file format.
   Therefore, a Count of 0 represents 1 pixel in the group.
  ---------------------------------------------------------------------------*/
bool TGA::EncodeCount(const Texture &Image, const TGA::Iterator &Iter, vector2<iter> P, usize &Count)
   {
   const usize BytesPerBixel = Image.GetBytesPerPixel();

   Count = 0;

   uint8* Ptr0 = Image.Address((uint)P.U, (uint)P.V);
   
   //Find the run-rength of repeating pixels
   iter Next = P.U + Iter.I.U;
   while (Next != Iter.E.U && Count < 0x7F)
      {
      uint8* Ptr1 = Image.Address((uint)Next, (uint)P.V);

      if (!ComparePixel(Ptr0, Ptr1, BytesPerBixel)) {break;}

      Next += Iter.I.U;
      Count++;
      }

   if (Count > 0) {return true;}

   //Find the number of non-repeating pixels
   Next = P.U + Iter.I.U;
   while (Next != Iter.E.U && Count < 0x7F)
      {
      uint8* Ptr0 = Image.Address((uint)P.U, (uint)P.V);
      uint8* Ptr1 = Image.Address((uint)Next, (uint)P.V);

      if (ComparePixel(Ptr0, Ptr1, BytesPerBixel)) {break;}

      P.U = Next;
      Next += Iter.I.U;
      Count++;
      }

   return false;
   }

/*---------------------------------------------------------------------------
   Saves a compressed image data to file.
  ---------------------------------------------------------------------------*/
void TGA::SaveImageEncode(const Texture &Image, const TGA::Iterator &Iter, const TGA::Colour &Colour)
   {
   const usize BytesPerBixel = Math::ByteSize((usize)Header.BitsPerPixel);
   uint8* Dst, *Src, Code;
   vector2<iter> P;

   Array<uint8, 16> Buffer;
   Buffer.Create(BytesPerBixel * Image.Resolution().U);

   for (P.V = Iter.S.V; P.V != Iter.E.V; P.V += Iter.I.V)
      {
      for (P.U = Iter.S.U; P.U != Iter.E.U;)
         {
         usize Count;

         if (EncodeCount(Image, Iter, P, Count))
            {
            Code = (uint8)(0x80 | Count);

            File.write(reinterpret_cast<char*>(&Code), 1);
            if (File.bad()) {throw dexception("File I/O error.");}

            Dst = Buffer.Pointer();
            Src = Image.Address((uint)P.U, (uint)P.V);
            Colour.Encode(Dst, Src);

            File.write(reinterpret_cast<char*>(Buffer.Pointer()), BytesPerBixel);
            if (File.bad()) {throw dexception("File I/O error.");}

            P.U += (Count + 1) * Iter.I.U;
            }
         else
            {
            Code = (uint8)Count;

            File.write(reinterpret_cast<char*>(&Code), 1);
            if (File.bad()) {throw dexception("File I/O error.");}

            Dst = Buffer.Pointer();

            for (uiter I = 0; I <= Count && P.U != Iter.E.U; I++, P.U += Iter.I.U) //Note, actual pixels to save is Count + 1
               {
               Src = Image.Address((uint)P.U, (uint)P.V);
               Colour.Encode(Dst, Src);
               Dst += BytesPerBixel;
               }

            File.write(reinterpret_cast<char*>(Buffer.Pointer()), (Count + 1) * BytesPerBixel);
            if (File.bad()) {throw dexception("File I/O error.");}
            }
         }
      }
   }

/*---------------------------------------------------------------------------
   Saves a TGA file.

   Image    : Image to save.
   Path     : Path to image file, relative to the current working directory.
   Flip     : Flip the image horizontally, vertically, or both.
   Compress : Apply run-lenght encoding on the image.
  ---------------------------------------------------------------------------*/
void TGA::Save(const Texture &Image, const std::string &Path, const vector2b &Flip, bool Compress)
   {
   Destroy();

   vector2<iter> Res = cast_vector2(iter, Image.Resolution());

   if (Res.U < 1 || Res.V < 1 || (uiter)Res.U > TGA::MaxRes || (uiter)Res.V > TGA::MaxRes)
      {throw dexception("Image resolution is out of range.");}

   File.open(Path.c_str(), std::fstream::out | std::fstream::binary);
   
   if (File.bad() || !File.is_open()) 
      {throw dexception("Failed to open \"%s\".", Path.c_str());}

   std::string ID;
   ID  = "Created by ";
   ID += AppName;
   ID += " ";
   ID += AppVersion;
   ID += ", ";
   ID += AppAuthor;

   Header.ID_FieldSize = (uint8)ID.size() + 1;
   Header.OriginU = 0;
   Header.OriginV = 0;
   Header.ResU = (uint16)Res.U;
   Header.ResV = (uint16)Res.V;
   Header.ImageDesc  =  Flip.U ? TGA::MaskFlipU : 0;
   Header.ImageDesc |= !Flip.V ? TGA::MaskFlipV : 0;  //Always invert as Texture object origin is at top left

   Header.ColMapType = 0;
   Header.ColMapEntOffset = 0;
   Header.ColMapEntCount = 0;
   Header.ColMapEntSize = 0;

   TGA::Colour* ImageColour = &Colour8;
   
   switch (Image.DataType())
      {
      case Texture::TypeAlpha : 
         Header.ImageDesc |= 8; //Indicate 8-bit alpha
      case Texture::TypeLum : 
         Header.ImageType = Compress ? (uint8)TGA::TypeGreyRLE : (uint8)TGA::TypeGrey;
         break;
      
      case Texture::TypeDepth : 
         Header.ImageType = Compress ? (uint8)TGA::TypeGreyRLE : (uint8)TGA::TypeGrey;
         ImageColour = &Grey16;
         break;
      
      case Texture::TypeRGB : 
         Header.ImageType = Compress ? (uint8)TGA::TypeTrueRLE : (uint8)TGA::TypeTrue;
         ImageColour = &Colour24;
         break;
      
      case Texture::TypeRGBA : 
         Header.ImageType = Compress ? (uint8)TGA::TypeTrueRLE : (uint8)TGA::TypeTrue;
         ImageColour = &Colour32;
         Header.ImageDesc |= 8; //Indicate 8-bit alpha
         break;
      
      default : throw dexception("Unkown or unsupported image type.");
      }

   Header.BitsPerPixel = (uint8)Image.GetBitsPerPixel();
   
   SaveHeader(Header);

   if (Header.ID_FieldSize > 0)
      {
      File.write(ID.c_str(), Header.ID_FieldSize);
      if (File.bad()) {throw dexception("File I/O error.");}
      }

   //Setup iterators
   TGA::Iterator Iter;
   Iter.S.U = Flip.U ? Res.U - 1 : 0;
   Iter.S.V = Flip.V ? Res.V - 1 : 0;
   Iter.E.U = Flip.U ? -1 : Res.U;
   Iter.E.V = Flip.V ? -1 : Res.V;
   Iter.I.U = Flip.U ? -1 : 1;
   Iter.I.V = Flip.V ? -1 : 1;

   if (Compress) {SaveImageEncode(Image, Iter, *ImageColour);} 
   else {SaveImage(Image, Iter, *ImageColour);}

   Destroy();
   }


//Close namespaces
NAMESPACE_END(File)
NAMESPACE_END(NAMESPACE_PROJECT)


//==== End of file ===========================================================
#endif
