/*===========================================================================
   Material Object

   Dominik Deak
  ===========================================================================*/

#ifndef ___MATERIAL_H___
#define ___MATERIAL_H___


/*---------------------------------------------------------------------------
   Header files
  ---------------------------------------------------------------------------*/
#include "common.h"
#include "mutex.h"
#include "vector.h"


//Namespaces
NAMESPACE_BEGIN(NAMESPACE_PROJECT)


/*---------------------------------------------------------------------------
   Classes
  ---------------------------------------------------------------------------*/
class Material : public MutexHandle
   {
   //---- Constants and definitions ----
   public:

   enum MatSide                                    //Material side enumeration
      {
      SideFront = GL_FRONT,                        //Applies property to front face
      SideBack = GL_BACK,                          //Applies property to back face
      SideBoth = GL_FRONT_AND_BACK                 //Applies property to both sides
      };

   //---- Member data ----
   private:

   MatSide Side;                                   //Specifies which side of the face to apply the material property
   vector4f Ambient;                               //Ambient RGBA colour
   vector4f Diffuse;                               //Diffuse RGBA colour
   vector4f Specular;                              //Specular RGBA colour
   vector4f Emission;                              //RGBA emitted light intensity
   float Shininess;                                //Specular exponent of the material, valid range [0.0, 128.0]

   //---- Methods ----
   public:

   Material(void);
   Material(const Material &obj);
   Material &operator = (const Material &obj);
   ~Material(void);

   private:

   //Data allocation
   void Clear(void);
   void Destroy(void);

   public:

   //Rendering related
   void Bind(void) const;
   void Unbind(void) const;

   //Data access
   inline void SetSide(MatSide Side) {Material::Side = Side;}
   inline void SetAmbient(const vector4f &Ambient) {Material::Ambient = Ambient;}
   inline void SetDiffuse(const vector4f &Diffuse) {Material::Diffuse = Diffuse;}
   inline void SetSpecular(const vector4f &Specular) {Material::Specular = Specular;}
   inline void SetSEmission(const vector4f &Emission) {Material::Emission = Emission;}
   inline void SetShininess(float Shininess) {Material::Shininess = Shininess;}

   inline MatSide GetSide(void) const {return Side;}
   inline vector4f GetAmbient(void) const {return Ambient;}
   inline vector4f GetDiffuse(void) const {return Diffuse;}
   inline vector4f GetSpecular(void) const {return Specular;}
   inline vector4f GetSEmission(void) const {return Emission;}
   inline float GetShininess(void) const {return Shininess;}
   };


//Close namespaces
NAMESPACE_END(NAMESPACE_PROJECT)


//==== End of file ===========================================================
#endif
