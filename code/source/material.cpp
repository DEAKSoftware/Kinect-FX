/*===========================================================================
   Material Object

   Dominik Deak
  ===========================================================================*/

#ifndef ___MATERIAL_CPP___
#define ___MATERIAL_CPP___


/*---------------------------------------------------------------------------
   Header files
  ---------------------------------------------------------------------------*/
#include "common.h"
#include "debug.h"
#include "material.h"


//Namespaces
NAMESPACE_BEGIN(NAMESPACE_PROJECT)


/*---------------------------------------------------------------------------
   Constructor.
  ---------------------------------------------------------------------------*/
Material::Material(void)
   {
   Clear();
   }

/*---------------------------------------------------------------------------
  Copy constructor, invoked when the current object is instantiated. This
  method performs a deep copy of the specified object. The current object is
  unitialised, which must be cleared.
  ---------------------------------------------------------------------------*/
Material::Material(const Material &obj) : MutexHandle()
   {
   Clear();

   Side = obj.Side;
   Ambient = obj.Ambient;
   Diffuse = obj.Diffuse;
   Specular = obj.Specular;
   Shininess = obj.Shininess;
   Emission = obj.Emission;
   }

/*---------------------------------------------------------------------------
  Assignment operator, invoked only when the current object already exist.
  This method performs a deep copy of the specified object. The current
  object may have allocated data which must be destroyed.
  ---------------------------------------------------------------------------*/
Material &Material::operator = (const Material &obj)
   {
   //No action on self assignment
   if (this == &obj) {return *this;}

   Destroy();

   Side = obj.Side;
   Ambient = obj.Ambient;
   Diffuse = obj.Diffuse;
   Specular = obj.Specular;
   Shininess = obj.Shininess;
   Emission = obj.Emission;

   return *this;
   }

/*---------------------------------------------------------------------------
   Destructor.
  ---------------------------------------------------------------------------*/
Material::~Material(void)
   {
   Destroy();
   }

/*---------------------------------------------------------------------------
   Defaults the structure.
  ---------------------------------------------------------------------------*/
void Material::Clear(void)
   {
   Side = Material::SideBoth;
   Ambient.Set4(0.2f, 0.2f, 0.2f, 1.0f);
   Diffuse.Set4(0.8f, 0.8f, 0.8f, 1.0f);
   Specular.Set4(0.0f, 0.0f, 0.0f, 1.0f);
   Emission.Set4(0.0f, 0.0f, 0.0f, 1.0f);
   Shininess = 0.0f;
   }

/*---------------------------------------------------------------------------
   Destroys the structure.
  ---------------------------------------------------------------------------*/
void Material::Destroy(void)
   {
   Clear();
   }

/*---------------------------------------------------------------------------
   Sets the current material colour.
  ---------------------------------------------------------------------------*/
void Material::Bind(void) const
   {
   glMaterialfv(Side, GL_AMBIENT, &Ambient.C0);
   glMaterialfv(Side, GL_DIFFUSE, &Diffuse.C0);
   glMaterialfv(Side, GL_SPECULAR, &Specular.C0);
   glMaterialfv(Side, GL_EMISSION, &Emission.C0);
   glMaterialf(Side, GL_SHININESS, Shininess);
   }

/*---------------------------------------------------------------------------
   Resets the current material colour. Not really necessary to call, unless
   default material colour is required.
  ---------------------------------------------------------------------------*/
void Material::Unbind(void) const
   {
   const vector4f DefaultAmbient(0.2f, 0.2f, 0.2f, 1.0f);
   const vector4f DefaultDiffuse(0.8f, 0.8f, 0.8f, 1.0f);
   const vector4f DefaultSpecular(0.0f, 0.0f, 0.0f, 1.0f);
   const vector4f DefaultEmission(0.0f, 0.0f, 0.0f, 1.0f);

   glMaterialfv(Side, GL_AMBIENT, &DefaultAmbient.C0);
   glMaterialfv(Side, GL_DIFFUSE, &DefaultDiffuse.C0);
   glMaterialfv(Side, GL_SPECULAR, &DefaultSpecular.C0);
   glMaterialfv(Side, GL_EMISSION, &DefaultEmission.C0);
   glMaterialf(Side, GL_SHININESS, 0.0f);
   }


//Close namespaces
NAMESPACE_END(NAMESPACE_PROJECT)


//==== End of file ===========================================================
#endif
