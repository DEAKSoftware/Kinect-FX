/*===========================================================================
   Lighting Object

   Dominik Deak
  ===========================================================================*/

#ifndef ___LIGHTING_H___
#define ___LIGHTING_H___


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
class Lighting : public MutexHandle
   {
   //---- Member data ----
   private:

   GLenum ID;                                      //Lighting ID, must be GL_LIGHT0, ..., GL_LIGHT7
   
   vector4f Ambient;                               //Ambient RGBA intensity
   vector4f Diffuse;                               //Diffuse RGBA intensity
   vector4f Specular;                              //Specular RGBA intensity
   vector4f Position;                              //Light position
   vector4f SpotDir;                               //Spotlight direction (significant only when SpotCutoff is not 180)
   float SpotExp;                                  //Intensity distribution of the light spot, range [0, 128]
   float SpotCutoff;                               //Maximum spread angle (cutoff) of a light source, range [0, 90] and 180 for uniform light distribution
   float AttenConst;                               //Constant attenuation value
   float AttenLinear;                              //Linear attenuation value
   float AttenQuad;                                //Quadratic attenuation value

   //---- Methods ----
   public:

   Lighting(void);
   Lighting(const Lighting &obj);
   Lighting &operator = (const Lighting &obj);
   ~Lighting(void);

   private:

   //Data allocation
   void Clear(void);
   void Destroy(void);

   public:

   //Rendering related
   void Buffer(uint ID);
   void Bind(void) const;
   void Unbind(void) const;
   void Update(void) const;

   //Data access
   void SetID(uint ID);
   inline void SetAmbient(const vector4f &Ambient) {Lighting::Ambient = Ambient;}
   inline void SetDiffuse(const vector4f &Diffuse) {Lighting::Diffuse = Diffuse;}
   inline void SetSpecular(const vector4f &Specular) {Lighting::Specular = Specular;}
   inline void SetPosition(const vector4f &Position) {Lighting::Position = Position;}
   inline void SetSpotDir(const vector4f &SpotDir) {Lighting::SpotDir = SpotDir;}
   inline void SetSpotExp(float SpotExp) {Lighting::SpotExp = SpotExp;}
   inline void SetSpotCutoff(float SpotCutoff) {Lighting::SpotCutoff = SpotCutoff;}
   inline void SetAttenConst(float AttenConst) {Lighting::AttenConst = AttenConst;}
   inline void SetAttenLinear(float AttenLinear) {Lighting::AttenLinear = AttenLinear;}
   inline void SetAttenQuad(float AttenQuad) {Lighting::AttenQuad = AttenQuad;}
   };


//Close namespaces
NAMESPACE_END(NAMESPACE_PROJECT)


//==== End of file ===========================================================
#endif
