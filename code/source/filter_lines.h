/*===========================================================================
   Line Effects Filter Class

   Dominik Deak
  ===========================================================================*/

#ifndef ___FILTER_LINES_H___
#define ___FILTER_LINES_H___


/*---------------------------------------------------------------------------
   Header files
  ---------------------------------------------------------------------------*/
#include "filter.h"


//Namespaces
NAMESPACE_BEGIN(NAMESPACE_PROJECT)


/*---------------------------------------------------------------------------
  Classes
  ---------------------------------------------------------------------------*/
class FilterLines : public Filter
   {
   //---- Member data ----
   private:

   Texture Lines;                                  //Texture for storing lines

   //---- Methods ----
   public:

   FilterLines(void);
   ~FilterLines(void);

   private:

   FilterLines(const FilterLines &obj);            //Disable
   FilterLines &operator = (const FilterLines &obj); //Disable

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
