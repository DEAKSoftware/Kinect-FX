/*===========================================================================
   OpenGL Shader Program

   Dominik Deak
  ===========================================================================*/

#ifndef ___SHADER_H___
#define ___SHADER_H___


/*---------------------------------------------------------------------------
   Header files
  ---------------------------------------------------------------------------*/
#include "array.h"
#include "common.h"
#include "mutex.h"


//Namespaces
NAMESPACE_BEGIN(NAMESPACE_PROJECT)


/*---------------------------------------------------------------------------
  Shader program class
  ---------------------------------------------------------------------------*/
class Shader : public MutexHandle
   {
   //---- Constants and definitions ----
   public:

   //Common uniform names
   static const char* UniformMP;                   //Projection matrix
   static const char* UniformMV;                   //Model view matrix
   static const char* UniformMT;                   //Texture matrix
   static const char* UniformMN;                   //Normal matrix
   static const char* UniformMVP;                  //Product of the model view and projection matrix

   enum ShaderType                                 //Shader type
      {
      ShaderVert = GL_VERTEX_SHADER,               //Attach a vertex shader
      ShaderFrag = GL_FRAGMENT_SHADER              //Attach a fragment shader
      };

   typedef struct ShaderSource                     //Source code entry
      {
      ShaderType Type;                             //Specifies the type of shader source
      std::string Code;                            //The actual source code 
      };

   //---- Member data ----
   private:

   Array<ShaderSource, 4> Source;                  //Array of shader sources
   Array<GLuint, 4> SID;                           //Array of compiled shader IDs
   GLuint PID;                                     //The linked program ID

   //---- Methods ----
   public:

   Shader(void);
   Shader(const Shader &obj);
   Shader &operator = (const Shader &obj);
   ~Shader(void);

   //Data allocation
   void Clear(void);
   void Destroy(void);
   void Attach(const char* Code, ShaderType Type);
   void Attach(const std::string &Code, ShaderType Type);

   //Rendering related
   void Delete(void);
   void Buffer(bool Keep);
   void Bind(void) const;
   void Unbind(void) const;

   //Data access
   inline GLuint ID(void) const {return PID;}
   };


//Close namespaces
NAMESPACE_END(NAMESPACE_PROJECT)


//==== End of file ===========================================================
#endif
