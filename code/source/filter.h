/*===========================================================================
   Filter Base Class

   Dominik Deak
  ===========================================================================*/

#ifndef ___FILTER_H___
#define ___FILTER_H___


/*---------------------------------------------------------------------------
   Header files
  ---------------------------------------------------------------------------*/
#include "buffers.h"
#include "common.h"
#include "file.h"
#include "material.h"
#include "matrix.h"
#include "mesh.h"
#include "shader.h"
#include "texture.h"
#include "vector.h"


//Namespaces
NAMESPACE_BEGIN(NAMESPACE_PROJECT)


/*---------------------------------------------------------------------------
  Classes
  ---------------------------------------------------------------------------*/
class Filter
   {
   //---- Constants and definitions ----
   public:

   enum InputSelect                                //Enumeration for texture selection
      {
      SelectVideo = 0,                             //Select the video texture as the view port size
      SelectDepth = 1                              //Select the depth texture as the view port size
      };

   //---- Member data ----
   protected:

   vector4u ViewPort;                              //View port size
   vector2f Ratio;                                 //Frustum aspect ratio
   float Near;                                     //Near plane
   float Far;                                      //Far plane
   float Range;                                    //Frustrum range, Far - Near
   matrix16f MP;                                   //Projection matrix
   matrix16f MV;                                   //Model view matrix
   matrix16f MT;                                   //Texture matrix
   matrix16f MC;                                   //Texture calibration matrix
   vector4f Scale;                                 //Texture calibration scaling
   vector4f Trans;                                 //Texture calibration translation
   GLuint FBOID;                                   //Frame buffer object ID
   GLuint DBOID;                                   //Depth buffer object ID
   GLuint CBOID;                                   //Colour buffer object ID

   Mesh Model;                                     //Plane for texturing the video (may be used for something else, depending on filter)
   Texture Video;                                  //Video texture
   Texture Depth;                                  //Depth texture
   Shader Program;                                 //Shader program
   Material Mat;                                   //Material property

   InputSelect Select;                             //Specifies which texture to use for viewport size
   uiter VideoUpdateID;                            //Update ID for the video buffer
   uiter DepthUpdateID;                            //Update ID for the depth buffer
   bool EnableVideo;                               //If set, video texture will be updated
   bool EnableDepth;                               //If set, depth texture will be updated
   bool EnableCal;                                 //If set, texture aligment calibration will be enabled
   bool EnableColour;                              //If set, colour editing is enabled

   //---- Methods ----
   public:

   Filter(void);
   ~Filter(void);

   private:

   Filter(const Filter &obj);                      //Disable
   Filter &operator = (const Filter &obj);         //Disable

   protected:

   //Data allocation
   void virtual Clear(void);
   void virtual Destroy(void);

   public:

   //Data allocation
   bool Ready(void) const;
   void Setup(Buffers &Buffer);
   void virtual Assets(Buffers &Buffer);
   void Change(Buffers &Buffer);

   //Rendering
   void Bind(void);
   void Unbind(void);
   void virtual Render(void);
   bool Update(Buffers &Buffer);
   bool Capture(Texture &Frame, bool Wait);

   //Data access
   inline vector2u Resolution(void) const {return vector2u(ViewPort.C2, ViewPort.C3);}
   inline GLuint ID(void) const {return CBOID;}
   inline bool UsesVideo(void) const {return EnableVideo;}
   inline bool UsesDepth(void) const {return EnableDepth;}
   inline bool UsesCal(void) const {return EnableCal;}
   inline bool UsesColour(void) const {return EnableColour;}

   void AddScale(float X, float Y);
   void AddTrans(float X, float Y);
   inline vector4f GetColour(void) const {return Mat.GetDiffuse();}
   inline void SetColour(const vector4f &Colour) {Mat.SetDiffuse(Colour);}
   };


//Close namespaces
NAMESPACE_END(NAMESPACE_PROJECT)


//==== End of file ===========================================================
#endif
