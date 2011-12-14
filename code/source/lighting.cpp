/*===========================================================================
   Lighting Object

   Dominik Deak
  ===========================================================================*/

#ifndef ___LIGHTING_CPP___
#define ___LIGHTING_CPP___


/*---------------------------------------------------------------------------
   Header files
  ---------------------------------------------------------------------------*/
#include "common.h"
#include "debug.h"
#include "lighting.h"


//Namespaces
NAMESPACE_BEGIN(NAMESPACE_PROJECT)


/*---------------------------------------------------------------------------
   Constructor.
  ---------------------------------------------------------------------------*/
Lighting::Lighting(void)
   {
   Clear();
   }

/*---------------------------------------------------------------------------
  Copy constructor, invoked when the current object is instantiated. This
  method performs a deep copy of the specified object. The current object is
  unitialised, which must be cleared.
  ---------------------------------------------------------------------------*/
Lighting::Lighting(const Lighting &obj) : MutexHandle()
   {
   Clear();

   ID = obj.ID;

   Ambient = obj.Ambient;
   Diffuse = obj.Diffuse;
   Specular = obj.Specular;

   Position = obj.Position;

   SpotDir = obj.SpotDir;
   SpotExp = obj.SpotExp;
   SpotCutoff = obj.SpotCutoff;

   AttenConst = obj.AttenConst;
   AttenLinear = obj.AttenLinear;
   AttenQuad = obj.AttenQuad;
   }

/*---------------------------------------------------------------------------
  Assignment operator, invoked only when the current object already exist.
  This method performs a deep copy of the specified object. The current
  object may have allocated data which must be destroyed.
  ---------------------------------------------------------------------------*/
Lighting &Lighting::operator = (const Lighting &obj)
   {
   //No action on self assignment
   if (this == &obj) {return *this;}

   Destroy();

   ID = obj.ID;

   Ambient = obj.Ambient;
   Diffuse = obj.Diffuse;
   Specular = obj.Specular;

   Position = obj.Position;

   SpotDir = obj.SpotDir;
   SpotExp = obj.SpotExp;
   SpotCutoff = obj.SpotCutoff;

   AttenConst = obj.AttenConst;
   AttenLinear = obj.AttenLinear;
   AttenQuad = obj.AttenQuad;

   return *this;
   }

/*---------------------------------------------------------------------------
   Destructor.
  ---------------------------------------------------------------------------*/
Lighting::~Lighting(void)
   {
   Destroy();
   }

/*---------------------------------------------------------------------------
   Dafaults the structure.
  ---------------------------------------------------------------------------*/
void Lighting::Clear(void)
   {
   ID = GL_LIGHT0;

   Ambient.Set4(0.1f, 0.2f, 0.3f, 1.0f);
   Diffuse.Set4(1.0f, 0.953f, 0.918f, 1.0f);
   Specular.Set4(1.0f, 1.0f, 1.0f, 1.0f);

   Position.Set4(0.0f, 0.0f, 1.0f, 0.0f);

   SpotDir.Set4(0.0f, 0.0f, -1.0f, 1.0f);
   SpotExp = 0.0f;
   SpotCutoff = 180.0f;

   AttenConst = 1.0f;
   AttenLinear = 0.0f;
   AttenQuad = 0.0f;
   }

/*---------------------------------------------------------------------------
   Destroys the structure.
  ---------------------------------------------------------------------------*/
void Lighting::Destroy(void)
   {
   Clear();
   }

/*---------------------------------------------------------------------------
   Buffers the light to the specidied ID. If the ID out of range, the 
   function does nothing.
  ---------------------------------------------------------------------------*/
void Lighting::Buffer(uint ID)
   {
   if (ID >= GL_MAX_LIGHTS) {debug("Lighting ID %d was out of range.", ID); return;}

   Lighting::ID = GL_LIGHT0 + (GLenum)ID;

   glEnable(Lighting::ID);

   glLightfv(Lighting::ID, GL_AMBIENT, &Ambient.C0);
   glLightfv(Lighting::ID, GL_DIFFUSE, &Diffuse.C0);
   glLightfv(Lighting::ID, GL_SPECULAR, &Specular.C0);

   glLightfv(Lighting::ID, GL_POSITION, &Position.C0);

   glLightfv(Lighting::ID, GL_SPOT_DIRECTION, &SpotDir.C0);
   glLightf(Lighting::ID, GL_SPOT_EXPONENT, SpotExp);
   glLightf(Lighting::ID, GL_SPOT_CUTOFF, SpotCutoff);

   glLightf(Lighting::ID, GL_CONSTANT_ATTENUATION, AttenConst);
   glLightf(Lighting::ID, GL_LINEAR_ATTENUATION, AttenLinear);
   glLightf(Lighting::ID, GL_QUADRATIC_ATTENUATION, AttenQuad);

   GLenum Error = glGetError();
   if (Error != GL_NO_ERROR) {throw dexception("OpenGL generated an error: %s", Debug::ErrorGL(Error));}
   }

/*---------------------------------------------------------------------------
   Enables light.
  ---------------------------------------------------------------------------*/
void Lighting::Bind(void) const
   {
   glEnable(ID);
   }

/*---------------------------------------------------------------------------
   Disables light.
  ---------------------------------------------------------------------------*/
void Lighting::Unbind(void) const
   {
   glDisable(ID);
   }

/*---------------------------------------------------------------------------
   Updates the position of the light. During the update, the new position 
   will be automatically multiplied with the model view matrix. Therefore, 
   make sure the current model view matrix is in the desired configuration
   before calling this function. It is also assumed that Bind( ) was called 
   prior.
  ---------------------------------------------------------------------------*/
void Lighting::Update(void) const
   {
   glLightfv(ID, GL_POSITION, &Position.C0);
   }

/*---------------------------------------------------------------------------
   Assigns the light to a particular ID.
  ---------------------------------------------------------------------------*/
void Lighting::SetID(uint ID) 
   {
   if (ID >= GL_MAX_LIGHTS) {debug("Lighting ID %d was out of range.", ID); return;}
   Lighting::ID = GL_LIGHT0 + (GLenum)ID;
   }


//Close namespaces
NAMESPACE_END(NAMESPACE_PROJECT)


//==== End of file ===========================================================
#endif
