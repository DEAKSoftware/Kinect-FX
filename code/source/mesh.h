/*===========================================================================
   Mesh Geometry

   Dominik Deak
  ===========================================================================*/

#ifndef ___MESH_H___
#define ___MESH_H___


/*---------------------------------------------------------------------------
   Header files
  ---------------------------------------------------------------------------*/
#include "array.h"
#include "common.h"
#include "element.h"
#include "matrix.h"
#include "mutex.h"
#include "vertex.h"


//Namespaces
NAMESPACE_BEGIN(NAMESPACE_PROJECT)


/*---------------------------------------------------------------------------
  The mesh class
  ---------------------------------------------------------------------------*/
class Mesh : public MutexHandle
   {
   //---- Constants and definitions ----
   public:

   typedef Vertex4tf Vert;                         //Vertex type used for meshes (has 4 texture coordinates)
   typedef Point1 Point;                           //Point type used for meshes
   typedef Edge2 Edge;                             //Edge type used for meshes
   typedef Face3 Face;                             //Face type used for meshes (triangles)

   static const usize DivMin = 1;                  //Minimum subdivisions for planar surfaces
   static const usize DivCirleMin = 3;             //Minimum subdivisions for cirlces
   static const usize DivSemiCirleMin = 2;         //Minimum subdivisions for semicirlces
   static const usize DivMax = 1024;               //Maximum subdivisions for any surface

   enum MeshMode                                   //Controls rendering mode
      {
      ModePoint = GL_POINTS,                       //Render as points
      ModeWire = GL_LINES,                         //Render as wireframe
      ModeSolid = GL_TRIANGLES                     //Render as a solid
      };

   //---- Member data ----
   private:

   MeshMode Mode;                                  //Mesh rendering mode
   
   Array<Mesh::Vert, 8> Verts;                     //Vertex array
   Array<Mesh::Point, 8> Points;                   //Point index array
   Array<Mesh::Edge, 8> Edges;                     //Edge index array
   Array<Mesh::Face, 8> Faces;                     //Face index array

   GLuint VBOID;                                   //Vertex buffer object
   GLuint IBOID;                                   //Index buffer object
   GLuint ICount;                                  //Number of indeces in the object

   //---- Methods ----
   public:

   //Constructor, assigment, and destructor
   Mesh(void);
   Mesh(const Mesh &obj);
   Mesh &operator = (const Mesh &obj);
   ~Mesh(void);

   //Data allocation
   void Clear(void);
   void Destroy(void);
   
   //Generate primitives
   void Plane(vector2u Div, MeshMode Mode = Mesh::ModePoint);
   void Lines(vector2u Div, MeshMode Mode = Mesh::ModeWire);
   void Cube(vector4u Div, MeshMode Mode = Mesh::ModePoint);
   void Tetrahedron(MeshMode Mode = Mesh::ModePoint);
   void Circle(usize Div, MeshMode Mode = Mesh::ModePoint);
   void SemiCircle(usize Div, MeshMode Mode = Mesh::ModePoint);
   void Sphere(usize Div, MeshMode Mode = Mesh::ModePoint);

   //Data manipulation
   void Merge(const Mesh &obj);
   void Transform(const matrix16f &M);
   
   //Rendering related
   void Buffer(bool Keep);
   void Bind(usize Count) const;
   void Unbind(usize Count) const;
   void Render(void) const;
   
   //Data access
   /*TO DO*/

   //Serialisation
   //void Serialise(void) const {}
   //void Unserialise(void) {}
   };


//Close namespaces
NAMESPACE_END(NAMESPACE_PROJECT)


//==== End of file ===========================================================
#endif
