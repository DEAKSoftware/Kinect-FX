/*===========================================================================
   Texture Object

   Dominik Deak
  ===========================================================================*/

#ifndef ___TEXTURE_CPP___
#define ___TEXTURE_CPP___


/*---------------------------------------------------------------------------
   Header files
  ---------------------------------------------------------------------------*/
#include "array.h"
#include "common.h"
#include "debug.h"
#include "texture.h"


//Namespaces
NAMESPACE_BEGIN(NAMESPACE_PROJECT)


/*---------------------------------------------------------------------------
   Constructor.
  ---------------------------------------------------------------------------*/
Texture::Texture(void)
   {
   Clear();
   }

/*---------------------------------------------------------------------------
  Copy constructor, invoked when the current object is instantiated. This
  method performs a deep copy of the specified object. The current object is 
  unitialised, which must be cleared.
  ---------------------------------------------------------------------------*/
Texture::Texture(const Texture &obj) : MutexHandle()
   {
   Clear();

   Type = obj.Type;
   Format = obj.Format;
   CompType = obj.CompType; 

   //Arrays use assginment operators for deep copying
   Data = obj.Data;   
   
   BitsPerPixel = obj.BitsPerPixel;
   BytesPerPixel = obj.BytesPerPixel;
   BytesPerLine = obj.BytesPerLine;
   Res = obj.Res;
   }

/*---------------------------------------------------------------------------
  Assignment operator, invoked only when the current object already exist.
  This method performs a deep copy of the specified object. The current 
  object may have allocated data which must be destroyed.
  ---------------------------------------------------------------------------*/
Texture &Texture::operator = (const Texture &obj)
   {
   //No action on self assignment
   if (this == &obj) {return *this;}

   Destroy();

   Type = obj.Type;
   Format = obj.Format;
   CompType = obj.CompType; 

   //Arrays use assginment operators for deep copying
   Data = obj.Data;   
   
   BitsPerPixel = obj.BitsPerPixel;
   BytesPerPixel = obj.BytesPerPixel;
   BytesPerLine = obj.BytesPerLine;
   Res = obj.Res;
   
   return *this;
   }

/*---------------------------------------------------------------------------
   Destructor.
  ---------------------------------------------------------------------------*/
Texture::~Texture(void)
   {
   Destroy();
   }

/*---------------------------------------------------------------------------
   Clears the structure.
  ---------------------------------------------------------------------------*/
void Texture::Clear(void)
   {
   Type = Texture::TypeRGB;
   Format = Texture::FormatRGB;
   CompType = GL_UNSIGNED_BYTE;

   BitsPerPixel = 0;
   BytesPerPixel = 0;
   BytesPerLine = 0;
   Res = 0;
   Wrap = false;
   MinFilter = MinLinMipLin;
   MagFilter = MagLinear;
   
   ID = 0;
   }

/*---------------------------------------------------------------------------
   Destroys the structure.
  ---------------------------------------------------------------------------*/
void Texture::Destroy(void)
   {
   if (ID > 0) {glDeleteTextures(1, &ID);}

   Data.Destroy();
   
   Clear();
   }

/*---------------------------------------------------------------------------
   Creates a new texture.

   Res  : Texture resolution in pixels.
   Type : Texture format or type.
  ---------------------------------------------------------------------------*/
void Texture::Create(const vector2u &Res, TexType Type)
   {
   Destroy();

   Texture::Type = Type;
   Texture::Res = Res.Max(1);

   switch (Type)
      {
      case Texture::TypeAlpha : 
         BitsPerPixel = 8;  
         Format = FormatAlpha; 
         CompType = GL_UNSIGNED_BYTE;
         break;

      case Texture::TypeLum : 
         BitsPerPixel = 8;  
         Format = FormatLum; 
         CompType = GL_UNSIGNED_BYTE;
         break;

      case Texture::TypeDepth : 
         BitsPerPixel = 16; 
         Format = FormatDepth; 
         CompType = GL_UNSIGNED_SHORT;
         break;

      case Texture::TypeDisp : 

         if (!GLEW_ARB_texture_float)
            {throw dexception("Current OpenGL context does not support ARB_texture_float extensions.");}

         BitsPerPixel = 32; 
         Format = FormatDisp; 
         CompType = GL_FLOAT;
         break;

      case Texture::TypeRGB : 
         BitsPerPixel = 24; 
         Format = FormatRGB; 
         CompType = GL_UNSIGNED_BYTE;
         break;

      case Texture::TypeRGBA : 
         BitsPerPixel = 32; 
         Format = FormatRGBA; 
         CompType = GL_UNSIGNED_BYTE;
         break;

      default : throw dexception("Undefined texture type.");
      }

   BytesPerPixel = Math::ByteSize(BitsPerPixel);
   BytesPerLine = BytesPerPixel * Texture::Res.U;
   Data.Create(BytesPerLine * Texture::Res.V);
   }

/*---------------------------------------------------------------------------
   Clears the texture data.
  ---------------------------------------------------------------------------*/
void Texture::ClearData(void)
   {
   if (Data.Size() < 1) {return;}
   memset(Data.Pointer(), 0, Data.Size());
   }

/*---------------------------------------------------------------------------
   Sets the minification filter.
  ---------------------------------------------------------------------------*/
void Texture::SetMinFilter(TexMinFilter Filter)
   {
   MinFilter = Filter;
   }

/*---------------------------------------------------------------------------
   Sets the magnification filter.
  ---------------------------------------------------------------------------*/
void Texture::SetMagFilter(TexMagFilter Filter)
   {
   MagFilter = Filter;
   }

/*---------------------------------------------------------------------------
   Sets the texture wrapping attribute.
  ---------------------------------------------------------------------------*/
void Texture::SetWrap(const vector2b &Wrap)
   {
   Texture::Wrap = Wrap;
   }

/*---------------------------------------------------------------------------
   Generates OpenGL texture objects from texture data.

   Keep : If set true, the original data will be preserved.
  ---------------------------------------------------------------------------*/
void Texture::Buffer(bool Keep)
   {
   if (ID > 0) {glDeleteTextures(1, &ID);}

   ID = 0;

   if (Data.Size() < 1) {return;}

   glEnable(GL_TEXTURE_2D); //Needed for glGenerateMipmap( ) to work on ATI

   glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

   glGenTextures(1, &ID);
   glBindTexture(GL_TEXTURE_2D, ID);

   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, MinFilter);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, MagFilter);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, Wrap.U ? GL_REPEAT : GL_CLAMP_TO_EDGE);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, Wrap.V ? GL_REPEAT : GL_CLAMP_TO_EDGE);

   glTexImage2D(GL_TEXTURE_2D, 0, Type, Res.U, Res.V, 0, Format, CompType, Data.Pointer());
   glGenerateMipmap(GL_TEXTURE_2D);

   GLenum Error = glGetError();
   if (Error != GL_NO_ERROR) {throw dexception("OpenGL generated an error: %s", Debug::ErrorGL(Error));}

   if (!Keep) 
      {
      Data.Destroy();
      BitsPerPixel = 0;
      BytesPerPixel = 0;
      BytesPerLine = 0;
      }
   
   debug("Committed texture ID: %d.\n", ID);
   }

/*---------------------------------------------------------------------------
   Binds a particular texture unit for rendering.
  ---------------------------------------------------------------------------*/
void Texture::Bind(uiter Unit) const
   {
   if (ID < 1) {return;}
   
   glActiveTexture(GL_TEXTURE0 + (GLenum)Unit);
   glBindTexture(GL_TEXTURE_2D, ID);
   }

/*---------------------------------------------------------------------------
   Unbinds a particular texture unit after rendering.
  ---------------------------------------------------------------------------*/
void Texture::Unbind(uiter Unit) const
   {
   if (ID < 1) {return;}

   glActiveTexture(GL_TEXTURE0 + (GLenum)Unit);
   glBindTexture(GL_TEXTURE_2D, 0);
   }

/*---------------------------------------------------------------------------
   Updates the texture object in video memory with the current data. 
   Assumes Bind( ) was called prior.
  ---------------------------------------------------------------------------*/
void Texture::Update(void)
   {
   if (ID < 1 || Data.Size() < 1) {return;}

   glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, Res.U, Res.V, Format, CompType, Data.Pointer());
   glGenerateMipmap(GL_TEXTURE_2D);
   }

/*---------------------------------------------------------------------------
   Updates the texture object in video memory with data from another texture. 
   Assumes Bind( ) was called prior.
  ---------------------------------------------------------------------------*/
void Texture::Update(const Texture &obj)
   {
   if (ID < 1 || obj.Size() < 1 || Type != obj.DataType()) {return;}
   
   const vector2u TexRes = obj.Resolution();
   if (Res.U != TexRes.U || Res.V != TexRes.V) {return;}

   glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, Res.U, Res.V, Format, CompType, obj.Pointer());
   glGenerateMipmap(GL_TEXTURE_2D);
   }


//Close namespaces
NAMESPACE_END(NAMESPACE_PROJECT)


//==== End of file ===========================================================
#endif
