/*===========================================================================
   Effects for Rendering Geometric Shapes

   Dominik Deak
  ===========================================================================*/

#ifndef ___FILTER_SOLIDS_H___
#define ___FILTER_SOLIDS_H___


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
class FilterSolids : public Filter
   {
   //---- Constants and definitions ----
   public:

   enum SolidType                                  //Solid type to render
      {
      Cubes = 0,                                   //Render cubes
      Spheres = 1,                                 //Render spheres
      CubesTinted = 2,                             //Render cubes with video tint
      SpheresTinted = 3,                           //Render spheres with video tint
      CubesFar = 4,                                //Render far cubes
      SpheresFar = 5                               //Render far spheres
      };

   static const uint PolyDivPixels = 8;            //Number for pixels to render per polygon
   static const uint PolyDivMin = 2;               //Minimum geometry division
   static const uint PolyDivMax = 8;               //Maximum geometry division
   static const uint GridDiv = 10;                 //Grid divident size
   static const uint GridMin = 9;                  //Least divisions per dimension
   static const uint GridMax = 127;                //Most divisions per dimension

   //---- Member data ----
   private:

   SolidType Type;
   Lighting Light;                                 //Light colour
   vector2u Grid;                                  //Cube grid divisions in each dimension
   vector2f GridNorm;                              //Reciprocal of grids
   vector2f Offset;                                //Half grid offset
   GLint UID;                                      //Uniform ID for updating texture coordinates

   //---- Methods ----
   public:

   FilterSolids(SolidType Type = FilterSolids::Cubes);
   ~FilterSolids(void);

   private:

   FilterSolids(const FilterSolids &obj);            //Disable
   FilterSolids &operator = (const FilterSolids &obj); //Disable

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
