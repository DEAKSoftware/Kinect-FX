/*===========================================================================
   Application Configuration

   Dominik Deak
  ===========================================================================*/

#ifndef ___CONFIG_H___
#define ___CONFIG_H___


/*---------------------------------------------------------------------------
   Definitions
  ---------------------------------------------------------------------------*/
#define NAMESPACE_PROJECT  K

#define NAMESPACE_BEGIN(N) namespace N {
#define NAMESPACE_END(N)   }
#define NAMESPACE_USE(N)   using namespace N


//Namespaces
NAMESPACE_BEGIN(NAMESPACE_PROJECT)
   

/*---------------------------------------------------------------------------
   Constants
  ---------------------------------------------------------------------------*/
static const char* const AppName        = "Kinect FX";
static const char* const AppVersion     = "1.0";
static const char* const AppVersionFull = "1.0.0.0";
static const char* const AppAuthor      = "Dominik Deak";
static const char* const AppAuthorWeb   = "dominikdeak.com";
static const char* const AppOrg         = "Bjango";
static const char* const AppOrgWeb      = "bjango.com";


//Close namespaces
NAMESPACE_END(NAMESPACE_PROJECT)


//==== End of file ===========================================================
#endif
