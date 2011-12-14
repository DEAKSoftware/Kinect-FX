/*===========================================================================
   PNG File I/O

   Dominik Deak
  ===========================================================================*/

#ifndef ___FILE_PNG_CPP___
#define ___FILE_PNG_CPP___


/*---------------------------------------------------------------------------
   Header files
  ---------------------------------------------------------------------------*/
#include "common.h"
#include "debug.h"
#include "file_png.h"


//Namespaces
NAMESPACE_BEGIN(NAMESPACE_PROJECT)
NAMESPACE_BEGIN(File)


/*---------------------------------------------------------------------------
   Constructor.
  ---------------------------------------------------------------------------*/
PNG::PNG(void)
   {
   Clear();
   }

/*---------------------------------------------------------------------------
   Destructor.
  ---------------------------------------------------------------------------*/
PNG::~PNG(void)
   {
   Destroy();
   }

/*---------------------------------------------------------------------------
   Clears the structure.
  ---------------------------------------------------------------------------*/
void PNG::Clear(void)
   {
   Mode    = PNG::ModeNull;
   File    = nullptr;
   PngPtr  = nullptr;
   InfoPtr = nullptr;
   }

/*---------------------------------------------------------------------------
   Destroys the structure.
  ---------------------------------------------------------------------------*/
void PNG::Destroy(void)
   {
   Rows.Destroy();

   switch (Mode)
      {
      case PNG::ModeRead : png_destroy_read_struct(&PngPtr, &InfoPtr, nullptr); break;
      case PNG::ModeWrite : png_destroy_write_struct(&PngPtr, &InfoPtr); break;
      default: break;
      }

   if (File != nullptr) {fclose(File);}

   Clear();
   }

/*---------------------------------------------------------------------------
   Loads a PNG file.

   Image  : Image will be returned there.
   Path   : Path to image file, relative to the current working directory.
   Gamma  : Display gamma to apply on textures, 2.2 is typical.
  ---------------------------------------------------------------------------*/
void PNG::Load(Texture &Image, const std::string &Path, float Gamma)
   {
   Destroy();

   //Important - set class to reading mode
   Mode = PNG::ModeRead;

   #if defined (WINDOWS)
      if (fopen_s(&File, Path.c_str(), "rb") != 0)
         {throw dexception("Failed to open file: %s.", Path.c_str());}
   #else
      File = fopen(Path.c_str(), "rb");
      if (File == nullptr) {throw dexception("Failed to open file: %s.", Path.c_str());}
   #endif

   uint8 Header[PNG::HeaderSize];
   if (fread((png_bytep)Header, 1, PNG::HeaderSize, File) == 0)
      {throw dexception("fread( ) failed.");}

   if (!png_check_sig(Header, PNG::HeaderSize))
      {throw dexception("Not a valid PNG file.");}

   PngPtr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
   if (PngPtr == nullptr) {throw dexception("png_create_read_struct( ) failed.");}

   InfoPtr = png_create_info_struct(PngPtr);
   if (InfoPtr == nullptr) {throw dexception("png_create_info_struct( ) failed.");}

   if (setjmp(png_jmpbuf(PngPtr)))
      {throw dexception("setjmp( ) failed.");}

   //Read into structures
   png_init_io(PngPtr, File);
   png_set_sig_bytes(PngPtr, PNG::HeaderSize);
   png_read_info(PngPtr, InfoPtr);

   //Determine image attributes
   vector2<png_uint_32> Res;
   int BitDepth, ColourType;
   png_get_IHDR(PngPtr, InfoPtr, &Res.U, &Res.V, &BitDepth, &ColourType, nullptr, nullptr, nullptr);

   Texture::TexType Type = Texture::TypeRGB;
   switch (ColourType)
      {
      case PNG_COLOR_TYPE_GRAY :
         if (BitDepth < 8)
            {
            Type = Texture::TypeLum;
            png_set_expand_gray_1_2_4_to_8(PngPtr);
            }
         else if (BitDepth == 16)
            {
            Type = Texture::TypeDepth;
            }
         break;

      case PNG_COLOR_TYPE_GRAY_ALPHA :
         Type = Texture::TypeRGBA;
         png_set_gray_to_rgb(PngPtr);
         break;

      case PNG_COLOR_TYPE_PALETTE :
         Type = Texture::TypeRGB;
         png_set_palette_to_rgb(PngPtr);
         break;

      case PNG_COLOR_TYPE_RGB :
         Type = Texture::TypeRGB;
         png_set_strip_alpha(PngPtr); //If libpng reports RGB, make sure we completely strip alpha
         break;

      case PNG_COLOR_TYPE_RGB_ALPHA :
         Type = Texture::TypeRGBA;
         break;

      default : throw dexception("Unknown colour type.");
      }

   //Convert palette alpha into a separate alpha channel
   if (png_get_valid(PngPtr, InfoPtr, PNG_INFO_tRNS))
      {
      png_set_tRNS_to_alpha(PngPtr);
      if (ColourType == PNG_COLOR_TYPE_PALETTE) {Type = Texture::TypeRGBA;}
      }

   //Force 8 bit per channel
   if (BitDepth >= 16)
      {
      if (Type != Texture::TypeDepth) {png_set_strip_16(PngPtr);}
      }
   else if (BitDepth < 8)
      {
      png_set_packing(PngPtr);
      }

   //Specify display and file gamma (assume 0.45455 for missing file gamma)
   double FileGamma;
   if (!png_get_gAMA(PngPtr, InfoPtr, &FileGamma)) {FileGamma = 0.45455;}
   png_set_gamma(PngPtr, Gamma, FileGamma);

   //Swap byte order for int16 values on big endian machines
   if ((BitDepth >= 16) && !Math::MachineLittleEndian())
      {png_set_swap(PngPtr);}

   //Apply the above transformation settings
   png_read_update_info(PngPtr, InfoPtr);

   //Allocate image
   Image.Create(cast_vector2(uint, Res), Type);

   if (Image.GetBytesPerLine() < (usize)png_get_rowbytes(PngPtr, InfoPtr))
      {throw dexception("Incorrect scan line size for allocated image.");}

   //Populate row pointer array
   Rows.Create((usize)Res.V);
   for (uiter I = 0; I < (usize)Res.V; I++)
      {
      Rows[I] = Image.Address(0, I);
      }

   //Decode
   png_read_image(PngPtr, Rows.Pointer());
   png_read_end(PngPtr, InfoPtr);

   //Clean up
   Destroy();
   }

/*---------------------------------------------------------------------------
   Saves a PNG file.

   Image    : Image to save.
   Path     : Path to image file, relative to the current working directory.
   Compress : Specifies the compression level to use for encoding.
  ---------------------------------------------------------------------------*/
void PNG::Save(const Texture &Image, const std::string &Path, CompLevel Compress)
   {
   Destroy();

   //Important - set class to writing mode
   Mode = PNG::ModeWrite;

   vector2u Res = Image.Resolution();
   if (Res.U < 1 || Res.V < 1) {return;}

   #if defined (WINDOWS)
      if (fopen_s(&File, Path.c_str(), "wb") != 0)
         {throw dexception("Failed to open file: %s.", Path.c_str());}
   #else
      File = fopen(Path.c_str(), "wb");
      if (File == nullptr) {throw dexception("Failed to open file: %s.", Path.c_str());}
   #endif

   PngPtr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
   if (PngPtr == nullptr) {throw dexception("png_create_write_struct( ) failed.");}

   InfoPtr = png_create_info_struct(PngPtr);
   if (InfoPtr == nullptr) {throw dexception("png_create_info_struct( ) failed.");}

   if (setjmp(png_jmpbuf(PngPtr)))
      {throw dexception("setjmp( ) failed.");}

   png_init_io(PngPtr, File);

   png_set_compression_level(PngPtr, (int)Compress);

   int BitDepth, ColourType;
   switch (Image.DataType())
      {
      case Texture::TypeAlpha : BitDepth = 8;  ColourType = PNG_COLOR_TYPE_GRAY; break;
      case Texture::TypeLum   : BitDepth = 8;  ColourType = PNG_COLOR_TYPE_GRAY; break;
      case Texture::TypeDepth : BitDepth = 16; ColourType = PNG_COLOR_TYPE_GRAY; break;
      case Texture::TypeRGB   : BitDepth = 8;  ColourType = PNG_COLOR_TYPE_RGB; break;
      case Texture::TypeRGBA  : BitDepth = 8;  ColourType = PNG_COLOR_TYPE_RGB_ALPHA; break;
      default : throw dexception("Unsupported image type."); break;
      }

   png_set_IHDR(PngPtr, InfoPtr, (png_uint_32)Res.U, (png_uint_32)Res.V, BitDepth, ColourType, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

   png_set_sRGB(PngPtr, InfoPtr, PNG_sRGB_INTENT_ABSOLUTE);
   png_set_sRGB_gAMA_and_cHRM(PngPtr, InfoPtr, PNG_sRGB_INTENT_ABSOLUTE);

   png_color_16 BackGnd;
   BackGnd.red   = 0;
   BackGnd.green = 0;
   BackGnd.blue  = 0;
   png_set_bKGD(PngPtr, InfoPtr, &BackGnd);

   png_text PngText[2];
   memset(PngText, 0, sizeof(PngText));

   PngText[0].compression = PNG_TEXT_COMPRESSION_NONE;
   PngText[0].key = const_cast<png_charp>("Software");
   PngText[0].text = const_cast<png_charp>(AppName);
   PngText[0].text_length = strlen(PngText[0].text);

   PngText[1].compression = PNG_TEXT_COMPRESSION_NONE;
   PngText[1].key = const_cast<png_charp>("Author");
   PngText[1].text = const_cast<png_charp>(AppAuthor);
   PngText[1].text_length = strlen(PngText[1].text);

   png_set_text(PngPtr, InfoPtr, PngText, 2);

   png_write_info(PngPtr, InfoPtr);

   //Populate row pointer array
   Rows.Create((usize)Res.V);
   for (uiter I = 0; I < (usize)Res.V; I++)
      {
      Rows[I] = Image.Address(0, I);
      }

   //Encode
   png_write_image(PngPtr, Rows.Pointer());
   png_write_end(PngPtr, nullptr);

   //Clean up
   Destroy();
   }


//Close namespaces
NAMESPACE_END(File)
NAMESPACE_END(NAMESPACE_PROJECT)


//==== End of file ===========================================================
#endif
