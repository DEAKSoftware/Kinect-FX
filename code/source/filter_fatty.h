/*===========================================================================
   Fatty Filter Class

   Dominik Deak
  ===========================================================================*/

#ifndef ___FILTER_FATTY_H___
#define ___FILTER_FATTY_H___


/*---------------------------------------------------------------------------
   Header files
  ---------------------------------------------------------------------------*/
#include "filter.h"
#include "lighting.h"


//Namespaces
NAMESPACE_BEGIN(NAMESPACE_PROJECT)


/*---------------------------------------------------------------------------
   Classes
  ---------------------------------------------------------------------------*/
class FilterFatty : public Filter
   {
   //---- Constants and definitions ----
   public:

   //---- Member data ----
   private:

   Lighting Light;                                 //Light colour

   //---- Methods ----
   public:

   FilterFatty(void);
   ~FilterFatty(void);

   private:

   FilterFatty(const FilterFatty &obj);            //Disable
   FilterFatty &operator = (const FilterFatty &obj); //Disable

   //Data allocation
   void Clear(void);
   void Destroy(void);
   void Assets(Buffers &Buffer);

   public:

   //Rendering
   void Render(void);
   };


//Close namespaces
NAMESPACE_END(NAMESPACE_PROJECT)


//==== End of file ===========================================================
#endif
