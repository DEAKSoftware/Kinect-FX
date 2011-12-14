/*===========================================================================
   Exception Handling and Debug Logging

   Dominik Deak
  ===========================================================================*/

#ifndef ___DEBUG_CPP___
#define ___DEBUG_CPP___


/*---------------------------------------------------------------------------
   Header files
  ---------------------------------------------------------------------------*/
#include "common.h"
#include "debug.h"


//Namespaces
NAMESPACE_BEGIN(NAMESPACE_PROJECT)


/*---------------------------------------------------------------------------
   Static globals.
  ---------------------------------------------------------------------------*/
std::ofstream Debug::File;
const char* const Debug::FileName = "debug.txt";


/*---------------------------------------------------------------------------
   Constructor that builds a formatted message. Constructor logs will auto-
   matically append a new line character.
  ---------------------------------------------------------------------------*/
//Constructor accepting function name and variadic arguments
Debug::Debug(const char* Function, const char* Format, ...) throw()
   {
   Clear();

   if (Format == nullptr) {return;}

   try {
      va_list ArgList;
      va_start(ArgList, Format);
      Construct(Function, Format, ArgList);
      va_end(ArgList);
      }
   
   catch(...) {return;}
   }

//Constructor accepting variadic arguments only
Debug::Debug(const char* Format, ...) throw()
   {
   Clear();

   if (Format == nullptr) {return;}

   try {
      va_list ArgList;
      va_start(ArgList, Format);
      Construct(nullptr, Format, ArgList);
      va_end(ArgList);
      }
   
   catch(...) {return;}
   }

/*---------------------------------------------------------------------------
  Copy constructor, invoked when the current object is instantiated. This
  method performs a deep copy of the specified object. The current object is
  unitialised, which must be cleared.
  ---------------------------------------------------------------------------*/
Debug::Debug(const Debug &obj) throw() : std::exception()
   {
   Clear();
   
   if (obj.Size < 1) {return;}
   
   Message = new(std::nothrow) char[obj.Size];
   if (Message == nullptr) {return;}

   Size = obj.Size;

   for (uiter I = 0; I < Size; I++) {Message[I] = obj.Message[I];}
   }

/*---------------------------------------------------------------------------
  Assignment operator, invoked only when the current object already exist.
  This method performs a deep copy of the specified object. The current
  object may have allocated data which must be destroyed.
  ---------------------------------------------------------------------------*/
Debug &Debug::operator = (const Debug &obj) throw()
   {
   //No action on self assignment
   if (this == &obj) {return *this;}

   Destroy();

   if (obj.Size < 1) {return *this;}
   
   Message = new(std::nothrow) char[obj.Size];
   if (Message == nullptr) {return *this;}

   Size = obj.Size;

   for (uiter I = 0; I < Size; I++) {Message[I] = obj.Message[I];}

   return *this;
   }

/*---------------------------------------------------------------------------
   Destructor.
  ---------------------------------------------------------------------------*/
Debug::~Debug(void) throw()
   {
   Destroy();
   }

/*---------------------------------------------------------------------------
   Clears the structure.
  ---------------------------------------------------------------------------*/
void Debug::Clear(void) throw()
   {
   Message = nullptr;
   Size = 0;
   }

/*---------------------------------------------------------------------------
   Destroys the structure.
  ---------------------------------------------------------------------------*/
void Debug::Destroy(void) throw()
   {
   delete[] Message;

   Clear();
   }

/*---------------------------------------------------------------------------
   This function is used exclusively by the constructors to format and log
   message strings. A new line character will be automatically appended.
  ---------------------------------------------------------------------------*/
void Debug::Construct(const char* Function, const char* Format, va_list ArgList) throw()
   {
   if (Format == nullptr) {return;}

   try {
      char Buffer[Debug::BufferLength];
      Buffer[0] = 0;

      #if defined (WINDOWS)
         int Length = vsnprintf_s(Buffer, Debug::BufferLast, _TRUNCATE, Format, ArgList);
      #else
         int Length = vsnprintf(Buffer, Debug::BufferLast, Format, ArgList);
      #endif

      if (Length < 0) 
         {
         Length = strnlen(Buffer, Debug::BufferLast);
         if (Length < 1) {return;}
         }
      
      Size = (usize)Length + 1; //Include terminating 0 character

      Buffer[Debug::BufferLast] = 0;
      
      //Echo to debug file
      Debug::Time();
    
      if (Function != nullptr)
         {
         Debug::File << " - " << Function << ": " << Buffer << std::endl;
         }
      else
         {
         Debug::File << " - " << Buffer << std::endl;
         }

      Message = new(std::nothrow) char[Size];
      if (Message == nullptr) {return;}

      for (uiter I = 0; I < Size; I++) {Message[I] = Buffer[I];}
      }
   catch(...) {return;}
   }

/*---------------------------------------------------------------------------
   Returns the message string.
  ---------------------------------------------------------------------------*/
const char* Debug::what(void) const throw()
   {
   return Message != nullptr ? Message : "";
   }

/*---------------------------------------------------------------------------
   Opens a debug file stream for writing.
  ---------------------------------------------------------------------------*/
void Debug::Open(const std::string &FileName) throw()
   {
   Debug::File.close();
   Debug::File.open(FileName.c_str());

   if (Debug::File.bad() || !Debug::File.is_open()) {return;}

   Debug::File.width(80); 
   Debug::File.fill('-'); 
   Debug::File << "\n";
   
   Debug::File << "\t" << AppName << " " << AppVersion << " - " << AppAuthor << std::endl;

   Debug::File.width(80); 
   Debug::File.fill('-'); 
   Debug::File << "\n";

   Debug::Log("File created.\n");

   Debug::File.flush();
   }

/*---------------------------------------------------------------------------
   Closes the debug file.
  ---------------------------------------------------------------------------*/
void Debug::Close(void) throw()
   {
   if (Debug::File.bad() || !Debug::File.is_open()) {return;}

   Debug::Log("Closing file.\n");

   Debug::File.close();
   }

/*---------------------------------------------------------------------------
   Prints message to the file using variadic argument lists. This is a 
   helper function that is invoked by other methods in this class. Also 
   accepts a function name string for debugging purposes. Note: This 
   function will not append a new line character.
  ---------------------------------------------------------------------------*/
void Debug::Print(const char* Function, const char* Format, va_list ArgList) throw()
   {
   if (Format == nullptr) {return;}

   if (Debug::File.bad() || !Debug::File.is_open()) {return;}

   char Buffer[Debug::BufferLength];
   Buffer[0] = 0;

   try {
      #if defined (WINDOWS)
         int Length = vsnprintf_s(Buffer, Debug::BufferLast, _TRUNCATE, Format, ArgList);
      #else
         int Length = vsnprintf(Buffer, Debug::BufferLast, Format, ArgList);
      #endif

      if (Length < 0) 
         {
         Length = strnlen(Buffer, Debug::BufferLast);
         if (Length < 1) {return;}
         }

      Buffer[Debug::BufferLast] = 0;
      }
   
   catch(...) {return;}

   if (Function != nullptr)
      {
      Debug::File << Function << ": " << Buffer;
      }
   else
      {
      Debug::File << Buffer;
      }
   }

/*---------------------------------------------------------------------------
   Prints message to the file using C style string formatting. Note: This 
   function will not apped a new line character.
  ---------------------------------------------------------------------------*/
void Debug::Print(const char* Format, ...) throw()
   {
   if (Format == nullptr) {return;}

   if (Debug::File.bad() || !Debug::File.is_open()) {return;}

   try {
      va_list ArgList;
      va_start(ArgList, Format);
      Debug::Print(nullptr, Format, ArgList);
      va_end(ArgList);
      }
   
   catch(...) {return;}
   }

/*---------------------------------------------------------------------------
   Prints a simple C++ string to the file. Note: This function will not apped 
   a new line character.
  ---------------------------------------------------------------------------*/
void Debug::Print(const std::string &Message) throw()
   {
   if (Message.size() < 1) {return;}

   if (Debug::File.bad() || !Debug::File.is_open()) {return;}

   Debug::File << Message;
   }

/*---------------------------------------------------------------------------
   Prints a message log to the file using C style string formatting, 
   including the time. Also accepts a function name string for debugging
   purposes. Note: This function will not apped a new line character.
  ---------------------------------------------------------------------------*/
void Debug::Log(const char* Function, const char* Format, ...) throw()
   {
   if (Format == nullptr) {return;}

   if (Debug::File.bad() || !Debug::File.is_open()) {return;}

   Debug::Time();
   Debug::File << " - ";

   try {
      va_list ArgList;
      va_start(ArgList, Format);
      Debug::Print(Function, Format, ArgList);
      va_end(ArgList);
      }
   
   catch(...) {return;}
   }

/*---------------------------------------------------------------------------
   Prints a message log to the file using C style string formatting, 
   including the time. Note: This function will not apped a new line 
   character.
  ---------------------------------------------------------------------------*/
void Debug::Log(const char* Format, ...) throw()
   {
   if (Format == nullptr) {return;}

   if (Debug::File.bad() || !Debug::File.is_open()) {return;}

   Debug::Time();
   Debug::File << " - ";

   try {
      va_list ArgList;
      va_start(ArgList, Format);
      Debug::Print(nullptr, Format, ArgList);
      va_end(ArgList);
      }
   
   catch(...) {return;}
   }

/*---------------------------------------------------------------------------
   Prints a simple C++ message log to the file, including the time. Also 
   accepts a function name string for debugging purposes. Note: This function 
   will not apped a new line character.
  ---------------------------------------------------------------------------*/
void Debug::Log(const std::string &Function, const std::string &Message) throw()
   {
   if (Message.size() < 1) {return;}

   if (Debug::File.bad() || !Debug::File.is_open()) {return;}

   Debug::Time();
   Debug::File << " - ";

   Debug::File << Function << ": " << Message << std::endl;
   }

/*---------------------------------------------------------------------------
   Prints a simple C++ message log to the file, including the time. Note: 
   This function will not apped a new line character.
  ---------------------------------------------------------------------------*/
void Debug::Log(const std::string &Message) throw()
   {
   if (Message.size() < 1) {return;}

   if (Debug::File.bad() || !Debug::File.is_open()) {return;}

   Debug::Time();
   Debug::File << " - ";

   Debug::File << Message << std::endl;
   }

/*---------------------------------------------------------------------------
   Prints the current time to the log file. Note: This function will not 
   apped a new line character.
  ---------------------------------------------------------------------------*/
void Debug::Time(void) throw()
   {
   if (Debug::File.bad() || !Debug::File.is_open()) {return;}

   char Buffer[Debug::BufferLength];
   Buffer[0] = 0;

   try {
      time_t UnixTime;
      time(&UnixTime);

      #if defined (WINDOWS)
         struct tm TimeInfo;
         if (localtime_s(&TimeInfo, &UnixTime) != 0) {return;}
         size_t Length = strftime(Buffer, Debug::BufferLast, "%H:%M:%S", &TimeInfo);
      #else
         const struct tm* TimeInfo = localtime(&UnixTime);
         if (TimeInfo == nullptr) {return;}
         size_t Length = strftime(Buffer, Debug::BufferLast, "%H:%M:%S", TimeInfo);
      #endif

      if (Length < 1) 
         {
         Length = strnlen(Buffer, Debug::BufferLast);
         if (Length < 1) {return;}
         }

      Buffer[Debug::BufferLast] = 0;
      }
   
   catch(...) {return;}

   Debug::File << Buffer;
   }

/*---------------------------------------------------------------------------
   Returns an OpenGL error string for a given error code.
  ---------------------------------------------------------------------------*/
const char* Debug::ErrorGL(GLenum Error) throw()
   {
   switch (Error)
      {
      case GL_NO_ERROR : 
         return "GL_NO_ERROR: No error has been recorded."; break;
      
      case GL_INVALID_ENUM : 
         return "GL_INVALID_ENUM: An unacceptable value is specified for an enumerated argument. The offending command is ignored."; break;
      
      case GL_INVALID_VALUE : 
         return "GL_INVALID_VALUE: A numeric argument is out of range. The offending command is ignored."; break;
      
      case GL_INVALID_OPERATION : 
         return "GL_INVALID_OPERATION: The specified operation is not allowed in the current state. The offending command is ignored."; break;
      
      case GL_STACK_OVERFLOW : 
         return "GL_STACK_OVERFLOW: This command would cause a stack overflow. The offending command is ignored."; break;
      
      case GL_STACK_UNDERFLOW : 
         return "GL_STACK_UNDERFLOW: This command would cause a stack underflow. The offending command is ignored."; break;
      
      case GL_OUT_OF_MEMORY : 
         return "GL_OUT_OF_MEMORY: There is not enough memory left to execute the command. The state of the GL is undefined."; break;
      
      case GL_TABLE_TOO_LARGE : 
         return "GL_TABLE_TOO_LARGE: The specified table exceeds the implementation's maximum supported table size. The offending command is ignored."; break;
      
      default : break;
      }
   
   return "Unknown error code.";
   }

/*---------------------------------------------------------------------------
   Returns an OpenGL frame buffer object status string for a given 
   status code.
  ---------------------------------------------------------------------------*/
const char* Debug::StatusFBO(GLenum Status) throw()
   {
   switch (Status)
      {
      case GL_FRAMEBUFFER_COMPLETE : 
         return "GL_FRAMEBUFFER_COMPLETE: The framebuffer bound to the specified target is complete."; break;
      
      case GL_FRAMEBUFFER_UNDEFINED : 
         return "GL_FRAMEBUFFER_UNDEFINED: Target is the default framebuffer, but the default framebuffer does not exist."; break;
      
      case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT : 
         return "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: One of the framebuffer attachment points is incomplete."; break;
      
      case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT : 
         return "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: The framebuffer does not have at least one image attached to it."; break;
      
      case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER : 
         return "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER."; break;
      
      case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER : 
         return "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER."; break;
      
      case GL_FRAMEBUFFER_UNSUPPORTED : 
         return "GL_FRAMEBUFFER_UNSUPPORTED."; break;
      
      case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE : 
         return "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE."; break;
      
      //case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS : 
      //   return "GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS."; break;
      
      default : break;
      }
   
   return "Unknown frame buffer object status code.";
   }


//Close namespaces
NAMESPACE_END(NAMESPACE_PROJECT)


//==== End of file ===========================================================
#endif
