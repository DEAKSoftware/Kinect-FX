/*===========================================================================
   Depth Filter Class With Palette

   Dominik Deak
  ===========================================================================*/

#ifndef ___FILTER_PALETTE_H___
#define ___FILTER_PALETTE_H___


/*---------------------------------------------------------------------------
   Header files
  ---------------------------------------------------------------------------*/
#include "filter.h"


//Namespaces
NAMESPACE_BEGIN(NAMESPACE_PROJECT)


/*---------------------------------------------------------------------------
  Classes
  ---------------------------------------------------------------------------*/
class FilterPalette : public Filter
   {
   //---- Constants and definitions ----
   public:

   enum PaletteType                                //Palette shader type
      {
      Grey = 0,                                    //Grey scale palette
      Thermal = 1,                                 //Thermal palette
      Spectrum = 2,                                //Rainbow or spectrum palette
      Saturate = 3                                 //Red-yellow-green gradient with sautration markers
      };

   //---- Member data ----
   private:

   PaletteType Type;                               //Controls the palette type
   Texture Palette;                                //Actual texture palette

   //---- Methods ----
   public:

   FilterPalette(PaletteType Type = FilterPalette::Grey);
   ~FilterPalette(void);

   private:

   FilterPalette(const FilterPalette &obj);        //Disable
   FilterPalette &operator = (const FilterPalette &obj); //Disable

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
