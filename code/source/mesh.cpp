/*===========================================================================
   Mesh Geometry

   Dominik Deak
  ===========================================================================*/

#ifndef ___MESH_CPP___
#define ___MESH_CPP___


/*---------------------------------------------------------------------------
   Header files
  ---------------------------------------------------------------------------*/
#include "common.h"
#include "debug.h"
#include "math.h"
#include "matrix.h"
#include "mesh.h"


//Namespaces
NAMESPACE_BEGIN(NAMESPACE_PROJECT)


/*---------------------------------------------------------------------------
   Constructor.
  ---------------------------------------------------------------------------*/
Mesh::Mesh(void)
   {
   Clear();
   }

/*---------------------------------------------------------------------------
  Copy constructor, invoked when the current object is instantiated. This
  method performs a deep copy of the specified object. The current object is 
  unitialised, which must be cleared.
  ---------------------------------------------------------------------------*/
Mesh::Mesh(const Mesh &obj) : MutexHandle()
   {
   Clear();

   Mode = obj.Mode;

   //Arrays use assginment operators for deep copying
   Verts = obj.Verts;
   Points = obj.Points;
   Edges = obj.Edges;
   Faces = obj.Faces;
   }

/*---------------------------------------------------------------------------
  Assignment operator, invoked only when the current object already exist.
  This method performs a deep copy of the specified object. The current 
  object may have allocated data which must be destroyed.
  ---------------------------------------------------------------------------*/
Mesh &Mesh::operator = (const Mesh &obj)
   {
   //No action on self assignment
   if (this == &obj) {return *this;}

   Destroy();

   Mode = obj.Mode;

   //Arrays use assginment operators for deep copying
   Verts = obj.Verts;
   Points = obj.Points;
   Edges = obj.Edges;
   Faces = obj.Faces;
   
   return *this;
   }

/*---------------------------------------------------------------------------
   Destructor.
  ---------------------------------------------------------------------------*/
Mesh::~Mesh(void)
   {
   Destroy();
   }

/*---------------------------------------------------------------------------
   Clears the structure.
  ---------------------------------------------------------------------------*/
void Mesh::Clear(void)
   {
   Mode = Mesh::ModePoint;
   
   VBOID = 0;
   IBOID = 0;
   ICount = 0;
   }

/*---------------------------------------------------------------------------
   Deletes the mesh from the OpenGL subsystem and also destroys internal 
   allocated data.
  ---------------------------------------------------------------------------*/
void Mesh::Destroy(void)
   {
   if (VBOID > 0) {glDeleteBuffers(1, &VBOID);}
   if (IBOID > 0) {glDeleteBuffers(1, &IBOID);}
   
   Verts.Destroy();
   Points.Destroy();
   Edges.Destroy();
   Faces.Destroy();

   Clear();
   }

/*---------------------------------------------------------------------------
   Merges an extrenal mesh model with this one. The method retains the 
   current rendering mode of the mesh.
  ---------------------------------------------------------------------------*/
void Mesh::Merge(const Mesh &obj)
   {
   //Merge vertices
   usize Last = Verts.Size();
   Verts += obj.Verts;

   //Merge points
   usize Size = obj.Points.Size(); //Freeze Size in case merging with self
   Points.Reserve(Size);

   for (uiter I = 0; I < Size; I++)
      {
      Mesh::Point P = obj.Points[I];
      P.Add(0, Last);
      Points += P;
      }

   //Merge edges
   Size = obj.Edges.Size(); 
   Edges.Reserve(Size);

   for (uiter I = 0; I < Size; I++)
      {
      Mesh::Edge E = obj.Edges[I];
      E.Add(0, Last);
      E.Add(1, Last);
      Edges += E;
      }

   //Merge faces
   Size = obj.Faces.Size();
   Faces.Reserve(Size);

   for (uiter I = 0; I < Size; I++)
      {
      Mesh::Face F = obj.Faces[I];
      F.Add(0, Last);
      F.Add(1, Last);
      F.Add(2, Last);
      Faces += F;
      }
   }

/*---------------------------------------------------------------------------
   Applies model transformation on the mesh vertices and vertex normals.

   M : Specifies the transformation matrix.
  ---------------------------------------------------------------------------*/
void Mesh::Transform(const matrix16f &M)
   {
   matrix16f Mn = M.Normal();

   for (uiter I = 0; I < Verts.Size(); I++)
      {
      Verts[I].Coord = M * Verts[I].Coord;
      Verts[I].Norm = Mn * Verts[I].Norm;
      Verts[I].Norm = Verts[I].Norm.Unit3();
      }
   }

/*---------------------------------------------------------------------------
   Generates OpenGL buffer objects from mesh data.

   Keep : If set true, the original mesh data will be preserved.
  ---------------------------------------------------------------------------*/
void Mesh::Buffer(bool Keep)
   {
   if (VBOID > 0) {glDeleteBuffers(1, &VBOID);}
   if (IBOID > 0) {glDeleteBuffers(1, &IBOID);}

   VBOID = 0;
   IBOID = 0;
   ICount = 0;

   if (Verts.Size() < 1) {return;}

   switch (Mode)
      {
      case Mesh::ModePoint :

         if (Points.Size() < 1) {return;}
         glGenBuffers(1, &IBOID);
         glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBOID);
         glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Mesh::Point) * Points.Size(), Points.Pointer(), GL_STATIC_DRAW);
         ICount = Points.Size() * Mesh::Point::Count();
         break;
      
      case Mesh::ModeWire :

         if (Edges.Size() < 1) {return;}
         glGenBuffers(1, &IBOID);
         glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBOID);
         glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Mesh::Edge) * Edges.Size(), Edges.Pointer(), GL_STATIC_DRAW);
         ICount = Edges.Size() * Mesh::Edge::Count();
         break;
      
      case Mesh::ModeSolid :

         if (Faces.Size() < 1) {return;}
         glGenBuffers(1, &IBOID);
         glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBOID);
         glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Mesh::Face) * Faces.Size(), Faces.Pointer(), GL_STATIC_DRAW);
         ICount = Faces.Size() * Mesh::Face::Count();
         break;
      
      default : throw dexception("Undefined mesh mode.");
      }

   glGenBuffers(1, &VBOID);
   glBindBuffer(GL_ARRAY_BUFFER, VBOID);
   glBufferData(GL_ARRAY_BUFFER, sizeof(Mesh::Vert) * Verts.Size(), Verts.Pointer(), GL_STATIC_DRAW);

   GLenum Error = glGetError();
   if (Error != GL_NO_ERROR) {throw dexception("OpenGL generated an error: %s", Debug::ErrorGL(Error));}

   if (Keep) 
      {
      Verts.Compact();
      Points.Compact();
      Edges.Compact();
      Faces.Compact();
      }
   else
      {   
      Verts.Destroy();
      Points.Destroy();
      Edges.Destroy();
      Faces.Destroy();
      }

   debug("Committed mesh ID: %d.\n", VBOID);
   }

/*---------------------------------------------------------------------------
   Invokes buffer binding for the OpenGL subsystem and enables the vertex, 
   normal, and texture array client state. Used before rendering the mesh.

   Count : Number of texture coordinates to bind.
  ---------------------------------------------------------------------------*/
void Mesh::Bind(usize Count) const
   {
   if (VBOID < 1 || ICount < 1) {return;}
   
   glBindBuffer(GL_ARRAY_BUFFER, VBOID);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBOID);

   glEnableClientState(GL_VERTEX_ARRAY);
   glEnableClientState(GL_NORMAL_ARRAY);

   glVertexPointer(4, GL_FLOAT, sizeof(Mesh::Vert), (void*)Mesh::Vert::OffsetCoord());
   glNormalPointer(GL_FLOAT, sizeof(Mesh::Vert), (void*)Mesh::Vert::OffsetNorm());

   Count = Math::Min(Count, Mesh::Vert::TexCount());

   for (uiter I = 0; I < Count; I++)
      {
      glClientActiveTexture(GL_TEXTURE0 + I);
      glEnableClientState(GL_TEXTURE_COORD_ARRAY);
      glTexCoordPointer(2, GL_FLOAT, sizeof(Mesh::Vert), (void*)Mesh::Vert::OffsetTex(I));
      }
   }

/*---------------------------------------------------------------------------
   Resets buffer binding and the client state that was initially setup by 
   Bind( ). Used after the rendering the mesh.

   Count : Number of texture coordinates to unbind.
  ---------------------------------------------------------------------------*/
void Mesh::Unbind(usize Count) const
   {
   if (VBOID < 1 || ICount < 1) {return;}

   Count = Math::Min(Count, Mesh::Vert::TexCount());

   glDisableClientState(GL_VERTEX_ARRAY);
   glDisableClientState(GL_NORMAL_ARRAY);

   for (uiter I = 0; I < Count; I++)
      {
      glClientActiveTexture(GL_TEXTURE0 + I);
      glDisableClientState(GL_TEXTURE_COORD_ARRAY);
      }

   glBindBuffer(GL_ARRAY_BUFFER, 0);
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
   }
   
/*---------------------------------------------------------------------------
   Renders the model, assumes Bind( ) was called prior.
  ---------------------------------------------------------------------------*/
void Mesh::Render(void) const
   {
   if (VBOID < 1 || ICount < 1) {return;}
   glDrawElements((GLenum)Mode, ICount, GL_UNSIGNED_INT, nullptr);
   }
   

//Close namespaces
NAMESPACE_END(NAMESPACE_PROJECT)


//==== End of file ===========================================================
#endif
