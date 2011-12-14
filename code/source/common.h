/*===========================================================================
   Common Definitions and Includes

   Dominik Deak
  ===========================================================================*/

#ifndef ___COMMON_H___
#define ___COMMON_H___


/*---------------------------------------------------------------------------
   Include libraries
  ---------------------------------------------------------------------------*/

//Disable min( ) and max( ) macros in windows.h
#if defined (WINDOWS)
   #define NOMINMAX
   #pragma warning(push, 3)                        //Set warning to /W3 for libs
#endif

//GL Easy Wrangler library, must be included before OpenGL headers
#include <GL/glew.h>

//Qt API, must be included before Windows specific stuff
#include <QtCore/QDir>
#include <QtGui/QApplication>
#include <QtGui/QColorDialog>
#include <QtGui/QDesktopServices>
#include <QtGui/QMessageBox>
#include <QtGui/QPushButton>
#include <QtGui/QWidget>
#include <QtOpenGL/QtOpenGL>
#include <QtUiTools/QtUiTools>

//Other libraries
#include <libfreenect.h>
#include <zlib.h>
#include <png.h>

//Windows
#if defined (WINDOWS)
   #pragma warning(pop)                         //Restore warning
   #if defined (DEBUG)
      #define _CRTDBG_MAP_ALLOC                 //Use debug versions of malloc, realloc, etc
      #include <crtdbg.h>                       //Memory leak detection library
   #endif

//Mac OS X
#elif defined (MACOSX)

//POSIX
#else

#endif

//Platform neutral C/C++ libraries
#include <cctype>
#include <cfloat>
#include <cmath>
#include <cstddef>
#include <cstring>
#include <ctime>
#include <exception>
#include <fstream>
#include <vector>

//Global program config
#include "config.h"


//Namespaces
NAMESPACE_BEGIN(NAMESPACE_PROJECT)


/*---------------------------------------------------------------------------
   Type definitions
  ---------------------------------------------------------------------------*/
typedef unsigned int uint;
typedef ptrdiff_t iter;                         //Signed iterator for arrays, memory adressing and pointer manipulation
typedef size_t uiter;                           //Unsigned iterator 
typedef ptrdiff_t size;                         //Signed integral type for defining block sizes, array lengths, etc
typedef size_t usize;                           //Unsigned integral size type

#if !defined (nullptr)
   #define nullptr 0
#endif

//Windows specific
#if defined (WINDOWS)
   typedef signed char int8;
   typedef signed __int16 int16;
   typedef signed __int32 int32;
   typedef signed __int64 int64;
   typedef unsigned char uint8;
   typedef unsigned __int16 uint16;
   typedef unsigned __int32 uint32;
   typedef unsigned __int64 uint64;

   #define cdeclare __cdecl

   #define FUNCTION_NAME __FUNCTION__

//Mac OS X, POSIX (assumes LP64 data model)
#else
   typedef int8_t int8;
   typedef int16_t int16;
   typedef int32_t int32;
   typedef int64_t int64;
   typedef uint8_t uint8;
   typedef uint16_t uint16;
   typedef uint32_t uint32;
   typedef uint64_t uint64;

   //#define cdeclare __attribute__((cdecl))
   #define cdeclare

   #if !defined (strnlen)
      #define strnlen(S, L) strlen(S)
   #endif
   
   #define FUNCTION_NAME __PRETTY_FUNCTION__
   //#define FUNCTION_NAME __FUNCTION__
#endif


//Close namespaces
NAMESPACE_END(NAMESPACE_PROJECT)


//==== End of file ===========================================================
#endif
