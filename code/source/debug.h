/*===========================================================================
   Exception Handling and Debug Logging

   Dominik Deak
  ===========================================================================*/

#ifndef ___DEBUG_H___
#define ___DEBUG_H___


/*---------------------------------------------------------------------------
   Header files
  ---------------------------------------------------------------------------*/
#include "common.h"


//Namespaces
NAMESPACE_BEGIN(NAMESPACE_PROJECT)


/*---------------------------------------------------------------------------
   Definitions
  ---------------------------------------------------------------------------*/
#if defined (WINDOWS)
   #define debug(Format, ...)          NAMESPACE_PROJECT::Debug::Log(FUNCTION_NAME, Format, __VA_ARGS__)
   #define dprint(...)                 NAMESPACE_PROJECT::Debug::Print(__VA_ARGS__)
   #define dout                        NAMESPACE_PROJECT::Debug::File
   #define dexception(Format, ...)     NAMESPACE_PROJECT::Debug(FUNCTION_NAME, Format, __VA_ARGS__)
#else
   #define debug(Format, Args...)      NAMESPACE_PROJECT::Debug::Log(FUNCTION_NAME, Format , ##Args)
   #define dprint(Args...)             NAMESPACE_PROJECT::Debug::Print(Args)
   #define dout                        NAMESPACE_PROJECT::Debug::File
   #define dexception(Format, Args...) NAMESPACE_PROJECT::Debug(FUNCTION_NAME, Format , ##Args)
#endif


/*---------------------------------------------------------------------------
   Classes
  ---------------------------------------------------------------------------*/
class Debug : public std::exception
   {
   //---- Constants and definitions ----
   public:

   static const char* const FileName;              //Default file name for debug logging
   static const usize BufferLength = 4096;         //Temporary string buffer length
   static const usize BufferLast = BufferLength - 1; //Last character index of the temp buffer

   //---- Member data ----
   private:
   
   char* Message;                                  //Message string
   usize Size;                                     //Allocated size of the string, including the 0 terminating character

   public:

   static std::ofstream File;                      //Debug file stream

   //---- Methods ----
   public:

   Debug(const char* Function, const char* Format, ...) throw();
   Debug(const char* Format, ...) throw();
   Debug(const Debug &obj) throw();
   Debug &operator = (const Debug &obj) throw();
   ~Debug(void) throw();

   private:

   //Data allocation
   void Clear(void) throw();
   void Destroy(void) throw();
   void Construct(const char* Function, const char* Format, va_list ArgList) throw();

   public:

   //Data access
   const char* what(void) const throw();

   //Static debug stream logging
   static void Open(const std::string &FileName = Debug::FileName) throw();
   static void Close(void) throw();
   static void Print(const char* Function, const char* Format, va_list ArgList) throw();
   static void Print(const char* Format, ...) throw();
   static void Print(const std::string &Message) throw();
   static void Log(const char* Function, const char* Format, ...) throw();
   static void Log(const char* Format, ...) throw();
   static void Log(const std::string &Function, const std::string &Message) throw();
   static void Log(const std::string &Message) throw();
   static void Time(void) throw();

   //Static helper functions
   static const char* ErrorGL(GLenum Error) throw();
   static const char* StatusFBO(GLenum Status) throw();
   };


//Close namespaces
NAMESPACE_END(NAMESPACE_PROJECT)


//==== End of file ===========================================================
#endif
