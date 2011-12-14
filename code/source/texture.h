/*===========================================================================
   Texture Object

   Dominik Deak
  ===========================================================================*/

#ifndef ___TEXTURE_H___
#define ___TEXTURE_H___


/*---------------------------------------------------------------------------
   Header files
  ---------------------------------------------------------------------------*/
#include "array.h"
#include "common.h"
#include "mutex.h"
#include "vector.h"


//Namespaces
NAMESPACE_BEGIN(NAMESPACE_PROJECT)


/*---------------------------------------------------------------------------
  The texture object
  ---------------------------------------------------------------------------*/
class Texture : public MutexHandle
   {
   //---- Constants and definitions ----
   public:

   enum TexType                                    //Texture internal format
      {
      TypeAlpha = GL_ALPHA8,                       //8-bit alpha
      TypeLum = GL_LUMINANCE8,                     //8-bit luminance
      TypeDepth = GL_LUMINANCE16,                  //16-bit depth
      TypeDisp = GL_LUMINANCE32F_ARB,              //32-bit float vertex displacement map (requires ARB_texture_float)
      TypeRGB = GL_RGB8,                           //24-bit colour
      TypeRGBA = GL_RGBA8                          //32-bit colour with alpha
      };

   enum TexFormat
      {
      FormatAlpha = GL_ALPHA,                      //Alpha
      FormatLum = GL_LUMINANCE,                    //Luminance
      FormatDepth = GL_LUMINANCE,                  //Depth
      FormatDisp = GL_LUMINANCE,                   //Displacement map
      FormatRGB = GL_RGB,                          //Colour
      FormatRGBA = GL_RGBA                         //Colour with alpha
      };

   enum TexMinFilter                               //Texture minification filter enumeration
      {
      MinNearest = GL_NEAREST,                     //Nearest neighbour sampling
      MinLinear = GL_LINEAR,                       //Bilinear interpolation
      MinNearMipNear = GL_NEAREST_MIPMAP_NEAREST,  //Nearest neighbour texel sampling, nearest neighbour mipmap selection
      MinLinMipNear = GL_LINEAR_MIPMAP_NEAREST,    //Bilinear texel sampling, nearest neighbour mipmap interpolation
      MinNearMipLin = GL_NEAREST_MIPMAP_LINEAR,    //Nearest neighbour texel sampling, linear mipmap interpolation
      MinLinMipLin = GL_LINEAR_MIPMAP_LINEAR       //Bilinear texel sampling, linear mipmap interpolation
      };

   enum TexMagFilter                               //Texture magnification filter enumeration
      {
      MagNearest = GL_NEAREST,                     //Nearest neighbour sampling
      MagLinear = GL_LINEAR,                       //Bilinear interpolation
      };

   //---- Member data ----
   private:

   TexType Type;                                   //Texture type
   TexFormat Format;                               //OpenGL texture format
   GLenum CompType;                                //OpenGL type per colour component
   Array<uint8, 256> Data;                         //Actual image data
   usize BitsPerPixel;                             //Bits per pixel
   usize BytesPerPixel;                            //Bytes per pixel
   usize BytesPerLine;                             //Bytes per horizontal line
   vector2u Res;                                   //Texture resolution
   TexMinFilter MinFilter;                         //Minification filter
   TexMagFilter MagFilter;                         //Magnification filter
   vector2b Wrap;                                  //Wrap or clamp texture in each dimension

   GLuint ID;                                      //Texture object ID

   //---- Methods ----
   public:

   Texture(void);
   Texture(const Texture &obj);
   Texture &operator = (const Texture &obj);
   ~Texture(void);

   //Data allocation
   void Clear(void);
   void Destroy(void);
   void Create(const vector2u &Res, TexType Type = Texture::TypeRGB);
   void ClearData(void);

   //Texture attributes
   void SetMinFilter(TexMinFilter Filter);
   void SetMagFilter(TexMagFilter Filter);
   void SetWrap(const vector2b &Wrap);
   
   //Rendering related
   void Buffer(bool Keep);
   void Bind(uiter Unit) const;
   void Unbind(uiter Unit) const;
   void Update(void);
   void Update(const Texture &obj);

   //Data access
   inline GLuint GetID(void) const {return ID;}
   inline TexType DataType(void) const {return Type;}
   inline TexFormat DataFormat(void) const {return Format;}
   inline GLenum DataCompType(void) const {return CompType;}
   inline uint8* Pointer(void) const {return Data.Pointer();}
   inline usize Size(void) const {return Data.Size();}
   inline usize GetBitsPerPixel(void) const {return BitsPerPixel;}
   inline usize GetBytesPerPixel(void) const {return BytesPerPixel;}
   inline usize GetBytesPerLine(void) const {return BytesPerLine;}
   inline vector2u Resolution(void) const {return Res;}
   inline vector2b WrapMode(void) const {return Wrap;}
   inline uiter Offset(uiter U, uiter V) const {return V * BytesPerLine + U * BytesPerPixel;}
   inline uiter Offset(const vector2u &Coord) const {return Coord.V * BytesPerLine + Coord.U * BytesPerPixel;}
   inline uint8* Address(uiter U, uiter V) const {return Data.Pointer() + V * BytesPerLine + U * BytesPerPixel;}
   inline uint8* Address(const vector2u &Coord) const {return Data.Pointer() + Coord.V * BytesPerLine + Coord.U * BytesPerPixel;}
   inline uint16* Address16(uiter U, uiter V) const {return reinterpret_cast<uint16*>(Data.Pointer() + V * BytesPerLine + U * BytesPerPixel);}
   inline uint16* Address16(const vector2u &Coord) const {return reinterpret_cast<uint16*>(Data.Pointer() + Coord.V * BytesPerLine + Coord.U * BytesPerPixel);}
   inline uint32* Address32(uiter U, uiter V) const {return reinterpret_cast<uint32*>(Data.Pointer() + V * BytesPerLine + U * BytesPerPixel);}
   inline uint32* Address32(const vector2u &Coord) const {return reinterpret_cast<uint32*>(Data.Pointer() + Coord.V * BytesPerLine + Coord.U * BytesPerPixel);}
   };


//Close namespaces
NAMESPACE_END(NAMESPACE_PROJECT)


//==== End of file ===========================================================
#endif
