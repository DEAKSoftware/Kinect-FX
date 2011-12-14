/*===========================================================================
   OpenGL Shader Program

   Dominik Deak
  ===========================================================================*/

#ifndef ___SHADER_CPP___
#define ___SHADER_CPP___


/*---------------------------------------------------------------------------
   Header files
  ---------------------------------------------------------------------------*/
#include "common.h"
#include "debug.h"
#include "shader.h"


//Namespaces
NAMESPACE_BEGIN(NAMESPACE_PROJECT)


/*---------------------------------------------------------------------------
   Static globals.
  ---------------------------------------------------------------------------*/
const char* Shader::UniformMP = "MP";
const char* Shader::UniformMV = "MV";
const char* Shader::UniformMT = "MT";
const char* Shader::UniformMN = "MN";
const char* Shader::UniformMVP = "MVP";


/*---------------------------------------------------------------------------
   Constructor.
  ---------------------------------------------------------------------------*/
Shader::Shader(void)
   {
   Clear();
   }

/*---------------------------------------------------------------------------
  Copy constructor, invoked when the current object is instantiated. This
  method performs a deep copy of the specified object. The current object is
  unitialised, which must be cleared.
  ---------------------------------------------------------------------------*/
Shader::Shader(const Shader &obj) : MutexHandle()
   {
   Clear();

   Source = obj.Source;
   }

/*---------------------------------------------------------------------------
  Assignment operator, invoked only when the current object already exist.
  This method performs a deep copy of the specified object. The current
  object may have allocated data which must be destroyed.
  ---------------------------------------------------------------------------*/
Shader &Shader::operator = (const Shader &obj)
   {
   //No action on self assignment
   if (this == &obj) {return *this;}

   Destroy();

   Source = obj.Source;

   return *this;
   }

/*---------------------------------------------------------------------------
   Destructor.
  ---------------------------------------------------------------------------*/
Shader::~Shader(void)
   {
   Destroy();
   }

/*---------------------------------------------------------------------------
   Clears the structure.
  ---------------------------------------------------------------------------*/
void Shader::Clear(void)
   {
   PID = 0;
   }

/*---------------------------------------------------------------------------
   Destroys the structure.
  ---------------------------------------------------------------------------*/
void Shader::Destroy(void)
   {
   Source.Destroy();

   Delete();

   Clear();
   }

/*---------------------------------------------------------------------------
   Attaches a fragment or vertex shader source code to this object.
  ---------------------------------------------------------------------------*/
//Attach code as a C string
void Shader::Attach(const char* Code, ShaderType Type)
   {
   if (Code == nullptr) {return;}
   
   ShaderSource S;
   S.Type = Type;
   S.Code = Code;
   Source += S;
   }

//Attach code as a C++ string
void Shader::Attach(const std::string &Code, ShaderType Type)
   {
   if (Code.size() < 1) {return;}

   ShaderSource S;
   S.Type = Type;
   S.Code = Code;
   Source += S;
   }

/*---------------------------------------------------------------------------
   Deletes OpenGL objects allocated by the Buffer( ) method.
  ---------------------------------------------------------------------------*/
void Shader::Delete(void)  
   {
   if (PID > 0)
      {
      GLsizei Count = 0;
      GLuint ID[16];

      do {
         glGetAttachedShaders(PID, 16, &Count, ID);
         for (uiter I = 0; I < (usize)Count; I++)
            {
            glDetachShader(PID, ID[I]);
            glDeleteShader(ID[I]);
            }
         }
      while (Count > 0);
      
      glDeleteProgram(PID);

      PID = 0;
      }
   else
      {
      for (uiter I = 0; I < SID.Size(); I++)
         {
         if (SID[I] < 1) {continue;}
         glDeleteShader(SID[I]);
         }
      }

   SID.Destroy();
   }

/*---------------------------------------------------------------------------
   Compiles all the shader sources then links them to a program ID.

   Keep : If set true the original sources will be kept in memory.
  ---------------------------------------------------------------------------*/
void Shader::Buffer(bool Keep)
   {
   Delete();

   if (Source.Size() < 1) {return;}

   GLint Status;
   Array<char> InfoLog;

   SID.Reserve(Source.Size());

   //New program ID
   PID = glCreateProgram();

   //Compile each shader source and attach to program
   for (uiter I = 0; I < Source.Size(); I++)
      {
      SID += glCreateShader(Source[I].Type);
      const char* Code = Source[I].Code.c_str();

      //Compile
      glShaderSource(SID[I], 1, &Code, nullptr);
      glCompileShader(SID[I]);

      //Check for shader errors
      glGetShaderiv(SID[I], GL_COMPILE_STATUS, &Status);
      if (Status == GL_FALSE) 
         {
         glGetShaderiv(SID[I], GL_INFO_LOG_LENGTH, &Status);
         if (Status > 0) 
            {
            InfoLog.Create((usize)Status + 1);
            InfoLog[0] = 0;
            glGetShaderInfoLog(SID[I], Status, NULL, InfoLog.Pointer());
            InfoLog[(uiter)Status] = 0;
            debug("Shader information log:\n\n%s\n", InfoLog.Pointer());
            }
         
         throw dexception("Failed to compile shader.");
         }

      //Attach shader to program
      glAttachShader(PID, SID[I]);
      }
  
   //Link compiled shaders in program
   glLinkProgram(PID);

   //Check for program errors
   glGetProgramiv(PID, GL_LINK_STATUS, &Status);
   if (Status == GL_FALSE) 
      {
      glGetShaderiv(PID, GL_INFO_LOG_LENGTH, &Status);
      if (Status > 0) 
         {
         InfoLog.Create((usize)Status + 1);
         InfoLog[0] = 0;
         glGetProgramInfoLog(PID, Status, NULL, InfoLog.Pointer());
         InfoLog[(uiter)Status] = 0;
         debug("Program information log:\n\n%s\n", InfoLog.Pointer());
         }
         
      throw dexception("Failed to link program.");
      }

   GLenum Error = glGetError();
   if (Error != GL_NO_ERROR) {throw dexception("OpenGL generated an error: %s", Debug::ErrorGL(Error));}

   if (!Keep)
      {
      Source.Destroy();
      }

   debug("Committed program ID: %d.\n", PID);
   }

/*---------------------------------------------------------------------------
   Binds a shader program before rendering a primitive
  ---------------------------------------------------------------------------*/
void Shader::Bind(void) const
   {
   if (PID < 1) {return;}
   glUseProgram(PID);
   }

/*---------------------------------------------------------------------------
   Unbinds a shader program
  ---------------------------------------------------------------------------*/
void Shader::Unbind(void) const
   {
   if (PID < 1) {return;}
   glUseProgram(0);
   }


//Close namespaces
NAMESPACE_END(NAMESPACE_PROJECT)


//==== End of file ===========================================================
#endif
