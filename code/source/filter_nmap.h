/*===========================================================================
   Normal Map Filter Class

   Dominik Deak
  ===========================================================================*/

#ifndef ___FILTER_NMAP_H___
#define ___FILTER_NMAP_H___


/*---------------------------------------------------------------------------
   Header files
  ---------------------------------------------------------------------------*/
#include "filter.h"


//Namespaces
NAMESPACE_BEGIN(NAMESPACE_PROJECT)


/*---------------------------------------------------------------------------
  Classes
  ---------------------------------------------------------------------------*/
class FilterNMap : public Filter
   {
   //---- Constants and definitions ----
   public:

   //---- Member data ----
   private:

   //---- Methods ----
   public:

   FilterNMap(void);
   ~FilterNMap(void);

   private:

   FilterNMap(const FilterNMap &obj);        //Disable
   FilterNMap &operator = (const FilterNMap &obj); //Disable

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
