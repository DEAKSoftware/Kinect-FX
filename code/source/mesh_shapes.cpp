/*===========================================================================
   Geometric Primitive Generator for Mesh Objects

   Dominik Deak
  ===========================================================================*/

#ifndef ___MESH_SHAPES_CPP___
#define ___MESH_SHAPES_CPP___


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
   Generates a plane model.

   Div  : Number of divisions in two dimensions.
   Mode : Specifies how to render the model.
  ---------------------------------------------------------------------------*/
void Mesh::Plane(vector2u Div, MeshMode Mode)
   {
   Div = Div.Clamp((uint)Mesh::DivMin, (uint)Mesh::DivMax);

   Destroy();

   Mesh::Mode = Mode;

   //Compute vertex count in each dimension
   vector2u Count;
   Count.X = Div.X + 1; 
   Count.Y = Div.Y + 1;

   //Increment size of coordinates
   vector2f dP, P;
   dP.X = 2.0f / (float)Div.X; 
   dP.Y = 2.0f / (float)Div.Y;

   //Reserve some space on the array
   usize Size = Count.X * Count.Y;
   Verts.Reserve(Size);

   //Build vertex list
   P.Y = -1.0f;
   for (uiter Y = 0; Y < Count.Y; Y++, P.Y += dP.Y)
      {
      P.X = -1.0f;
      for (uiter X = 0; X < Count.X; X++, P.X += dP.X)
         {
         Mesh::Vert V;

         V.Coord.X = P.X;
         V.Coord.Y = P.Y;
      
         V.TexCoords(P.X * 0.5f + 0.5f, P.Y * 0.5f + 0.5f);
         
         Verts += V;
         }
      }

   //Build point list
   if (Mode == Mesh::ModePoint)
      {
      Points.Reserve(Size);
      Mesh::Point P;

      for (uiter I = 0; I < Size; I++)
         {
         P.Set(0, I);
         Points += P;
         }
      }

   //Build edge list
   else if (Mode == Mesh::ModeWire)
      {
      Edges.Reserve(2 * Div.X * Div.Y + Div.X + Div.Y);
      Mesh::Edge E;
      uiter I[4];

      for (uiter Y = 0; Y < Div.Y; Y++)
         {
         for (uiter X = 0; X < Div.X; X++)
            {
            I[0] = Y * Count.X + X;
            I[1] = I[0] + 1;
            I[2] = I[1] + Count.X;
            I[3] = I[0] + Count.X;
            
            E.Set(0, I[0]);
            E.Set(1, I[1]);
            Edges += E;
            
            E.Set(0, I[0]);
            E.Set(1, I[3]);
            Edges += E;
            
            if (Div.X - X == 1)
               {
               E.Set(0, I[1]);
               E.Set(1, I[2]);
               Edges += E;
               }

            if (Div.Y - Y == 1)
               {
               E.Set(0, I[2]);
               E.Set(1, I[3]);
               Edges += E;
               }
            }
         }
      }

   //Build face list
   else if (Mode == Mesh::ModeSolid)
      {
      Faces.Reserve(Div.X * Div.Y * 2);
      Mesh::Face F;
      uiter I[4];

      for (uiter Y = 0; Y < Div.Y; Y++)
         {
         for (uiter X = 0; X < Div.X; X++)
            {
            I[0] = Y * Count.X + X;
            I[1] = I[0] + 1;
            I[2] = I[1] + Count.X;
            I[3] = I[0] + Count.X;
            
            F.Set(0, I[0]);
            F.Set(1, I[1]);
            F.Set(2, I[3]);
            Faces += F;
            
            F.Set(0, I[1]);
            F.Set(1, I[2]);
            F.Set(2, I[3]);
            Faces += F;
            }
         }
      }
   }

/*---------------------------------------------------------------------------
   Constructs a planar model consisting of horizontal lines only.

   Div  : Number of divisions in two dimensions.
   Mode : Specifies how to render the model.
  ---------------------------------------------------------------------------*/
void Mesh::Lines(vector2u Div, MeshMode Mode)
   {
   Div = Div.Clamp((uint)Mesh::DivMin, (uint)Mesh::DivMax);

   Destroy();

   Mesh::Mode = Mode;

   //Compute vertex count in each dimension
   vector2u Count;
   Count.X = Div.X + 1; 
   Count.Y = Div.Y + 1;

   //Increment size of coordinates
   vector2f dP, P;
   dP.X = 2.0f / (float)Div.X; 
   dP.Y = 2.0f / (float)Div.Y;

   //Reserve some space on the array
   usize Size = Count.X * Count.Y;
   Verts.Reserve(Size);

   //Build vertex list
   P.Y = -1.0f;
   for (uiter Y = 0; Y < Count.Y; Y++, P.Y += dP.Y)
      {
      P.X = -1.0f;
      for (uiter X = 0; X < Count.X; X++, P.X += dP.X)
         {
         Mesh::Vert V;

         V.Coord.X = P.X;
         V.Coord.Y = P.Y;
      
         V.TexCoords(P.X * 0.5f + 0.5f, P.Y * 0.5f + 0.5f);
         
         Verts += V;
         }
      }


   //Build point list
   if (Mode == Mesh::ModePoint)
      {
      Points.Reserve(Size);
      Mesh::Point P;

      for (uiter I = 0; I < Size; I++)
         {
         P.Set(0, I);
         Points += P;
         }
      }

   //Build edge list
   else
      {
      Mode = Mesh::ModeWire; //Make sure it's wireframe

      Edges.Reserve(Div.X * Div.Y + Div.X);
      Mesh::Edge E;
      uiter I[2];

      for (uiter Y = 0; Y < Div.Y; Y++)
         {
         for (uiter X = 0; X < Div.X; X++)
            {
            I[0] = Y * Count.X + X;
            I[1] = I[0] + 1;
            
            E.Set(0, I[0]);
            E.Set(1, I[1]);
            Edges += E;
            
            if (Div.Y - Y == 1)
               {
               E.Set(0, I[1] + Count.X);
               E.Set(1, I[0] + Count.X);
               Edges += E;
               }
            }
         }
      }
   }

/*---------------------------------------------------------------------------
   Generates a cube model. Corner and edge vertices are not shared between
   faces and edge elements beloning to different sides of the cube. This is
   done to preserve direction of the normal vector on each side, and to 
   maintain a separate texture coordinate mapping along the cube edges.

   Div  : Number of divisions in three dimensions.
   Mode : Specifies how to render the model.
  ---------------------------------------------------------------------------*/
void Mesh::Cube(vector4u Div, MeshMode Mode)
   {
   Div = Div.Clamp3((uint)Mesh::DivMin, (uint)Mesh::DivMax);

   Destroy();

   Mesh::Mode = Mode;

   //-- Z-Y plane, normal along X-axis --
   vector2u NewDiv;
   NewDiv.X = Div.Z;
   NewDiv.Y = Div.Y;
   Mesh New;
   New.Plane(NewDiv, Mode);

   //Faces positive direction
   matrix16f M;
   M = M.Identity();
   M = M.Translate(1.0f, 0.0f, 0.0f);
   M = M.Rotate(90.0f, 0.0f, 1.0f, 0.0f);
   New.Transform(M);
   Merge(New);

   //Faces opposite direction
   M = M.Identity();
   M = M.Scale(-1.0f, -1.0f, 1.0f);
   New.Transform(M);
   Merge(New);

   //-- X-Z plane, normal along Y-axis --
   NewDiv.X = Div.X;
   NewDiv.Y = Div.Z;
   New.Plane(NewDiv, Mode);

   //Faces positive direction
   M = M.Identity();
   M = M.Translate(0.0f, 1.0f, 0.0f);
   M = M.Rotate(-90.0f, 1.0f, 0.0f, 0.0f);
   New.Transform(M);
   Merge(New);

   //Faces opposite direction
   M = M.Identity();
   M = M.Scale(-1.0f, -1.0f, 1.0f);
   New.Transform(M);
   Merge(New);

   //-- X-Y plane, normal along Z-axis --
   NewDiv.X = Div.X;
   NewDiv.Y = Div.Y;
   New.Plane(NewDiv, Mode);

   //Faces positive direction
   M = M.Identity();
   M = M.Translate(0.0f, 0.0f, 1.0f);
   New.Transform(M);
   Merge(New);

   //Faces opposite direction
   M = M.Identity();
   M = M.Scale(1.0f, -1.0f, -1.0f);
   New.Transform(M);
   Merge(New);
   }

/*---------------------------------------------------------------------------
   Generates a minimal equilateral tetrahedron with adjacent face and edge 
   elements sharing corner vertices.

   Mode : Specifies how to render the model.
  ---------------------------------------------------------------------------*/
void Mesh::Tetrahedron(MeshMode Mode)
   {
   Destroy();

   Mesh::Mode = Mode;

   //Construct the tetrahedron
   Verts.Reserve(4);
   Mesh::Vert V;

   V.Coord.Set3(1.0f, 1.0f, 1.0f);
   V.Norm = V.Coord.Unit3();
   V.Norm.W = 0.0f;
   Verts += V;
   
   V.Coord.Set3(-1.0f, -1.0f, 1.0f);
   V.Norm = V.Coord.Unit3();
   V.Norm.W = 0.0f;
   Verts += V;

   V.Coord.Set3(-1.0f, 1.0f, -1.0f);
   V.Norm = V.Coord.Unit3();
   V.Norm.W = 0.0f;
   Verts += V;

   V.Coord.Set3(1.0f, -1.0f, -1.0f);
   V.Norm = V.Coord.Unit3();
   V.Norm.W = 0.0f;
   Verts += V;

   //Build point list
   if (Mode == Mesh::ModePoint)
      {
      Points.Reserve(4);
      Mesh::Point P;

      for (uiter I = 0; I < 4; I++)
         {
         P.Set(0, I);
         Points += P;
         }
      }

   //Build edge list
   else if (Mode == Mesh::ModeWire)
      {
      Edges.Reserve(6);
      Mesh::Edge E;

      E.Set(0, 0); E.Set(1, 1); Edges += E;
      E.Set(0, 0); E.Set(1, 2); Edges += E;
      E.Set(0, 1); E.Set(1, 2); Edges += E;
      E.Set(0, 1); E.Set(1, 3); Edges += E;
      E.Set(0, 2); E.Set(1, 3); Edges += E;
      E.Set(0, 3); E.Set(1, 0); Edges += E;
      }

   //Build face list
   else if (Mode == Mesh::ModeSolid)
      {
      Faces.Reserve(4);
      Mesh::Face F;

      F.Set(0, 0);
      F.Set(1, 1);
      F.Set(2, 3);
      Faces += F;

      F.Set(0, 1);
      F.Set(1, 2);
      F.Set(2, 3);
      Faces += F;

      F.Set(0, 2);
      F.Set(1, 0);
      F.Set(2, 3);
      Faces += F;

      F.Set(0, 0);
      F.Set(1, 2);
      F.Set(2, 1);
      Faces += F;
      }
   }

/*---------------------------------------------------------------------------
   Generates a circle model.

   Div  : Number of divisions along the circumference.
   Mode : Specifies how to render the model.
  ---------------------------------------------------------------------------*/
void Mesh::Circle(usize Div, MeshMode Mode)
   {
   Div = Math::Clamp(Div, Mesh::DivCirleMin, Mesh::DivMax);

   Destroy();

   Mesh::Mode = Mode;

   float dA = Math::DegToRad(360.0f) / (float)Div;
   float A  = 0.0f;

   usize Count = Mode == Mesh::ModeSolid ? Div + 1 : Div;
   Verts.Reserve(Count);

   //Build vertex list
   for (uiter I = 0; I < Count; I++)
      {
      Mesh::Vert V;

      //First point will be central for solids
      if (Mode == Mesh::ModeSolid && I == 0) 
         {
         V.Coord.X = 0.0f;
         V.Coord.Y = 0.0f;
         }
      else
         {
         V.Coord.X = cos(A);
         V.Coord.Y = sin(A);
         A += dA;
         }

      V.TexCoords(V.Coord.X * 0.5f + 0.5f, V.Coord.Y * 0.5f + 0.5f);
         
      Verts += V;
      }

   //Build point list
   if (Mode == Mesh::ModePoint)
      {
      Points.Reserve(Count);
      Mesh::Point P;

      for (uiter I = 0; I < Count; I++)
         {
         P.Set(0, I);
         Points += P;
         }
      }

   //Build edge list
   else if (Mode == Mesh::ModeWire)
      {
      Edges.Reserve(Div);
      Mesh::Edge E;

      for (usize I = 0, V = 0; I < Div; I++)
         {
         E.Set(0, V);
         V = (V + 1) % Count;
         E.Set(1, V);
         Edges += E;
         }
      }

   //Build face list
   else if (Mode == Mesh::ModeSolid)
      {
      Faces.Reserve(Div);
      Mesh::Face F;
      
      for (usize I = 0, V = 1; I < Div; I++)
         {
         F.Set(0, 0);
         F.Set(1, V);
         V++;
         F.Set(2, (V < Count) ? V : 1);
         Faces += F;
         }
      }
   }

/*---------------------------------------------------------------------------
   Generates a semicircle model.

   Div  : Number of divisions along the circumference.
   Mode : Specifies how to render the model.
  ---------------------------------------------------------------------------*/
void Mesh::SemiCircle(usize Div, MeshMode Mode)
   {
   Div = Math::Clamp(Div, Mesh::DivSemiCirleMin, Mesh::DivMax);

   Destroy();

   Mesh::Mode = Mode;

   float dA = Math::DegToRad(180.0f) / (float)Div;
   float A  = 0.0f;

   usize Count = Div + (Mode == Mesh::ModeSolid ? 2 : 1);
   Verts.Reserve(Count);

   //Build vertex list
   for (uiter I = 0; I < Count; I++)
      {
      Mesh::Vert V;

      //First point will be central for solids
      if (Mode == Mesh::ModeSolid && I == 0) 
         {
         V.Coord.X = 0.0f;
         V.Coord.Y = 0.0f;
         }
      else
         {
         V.Coord.X = cos(A);
         V.Coord.Y = sin(A);
         A += dA;
         }

      V.TexCoords(V.Coord.X * 0.5f + 0.5f, V.Coord.Y * 0.5f + 0.5f);

      Verts += V;
      }

   //Build point list
   if (Mode == Mesh::ModePoint)
      {
      Points.Reserve(Count);
      Mesh::Point P;

      for (uiter I = 0; I < Count; I++)
         {
         P.Set(0, I);
         Points += P;
         }
      }

   //Build edge list
   else if (Mode == Mesh::ModeWire)
      {
      Edges.Reserve(Count);
      Mesh::Edge E;

      for (usize I = 0, V = 0; I < Count; I++)
         {
         E.Set(0, V);
         V = (V + 1) % Count;
         E.Set(1, V);
         Edges += E;
         }
      }

   //Build face list
   else if (Mode == Mesh::ModeSolid)
      {
      Faces.Reserve(Div);
      Mesh::Face F;
      
      for (usize I = 0, V = 1; I < Div; I++)
         {
         F.Set(0, 0);
         F.Set(1, V);
         V++;
         F.Set(2, (V < Count) ? V : 1);
         Faces += F;
         }
      }
   }

/*---------------------------------------------------------------------------
   Generates a cubic sphere model.

   Div  : Number of divisions.
   Mode : Specifies how to render the model.
  ---------------------------------------------------------------------------*/
void Mesh::Sphere(usize Div, MeshMode Mode)
   {
   Cube(Div, Mode);

   for (uiter I = 0; I < Verts.Size(); I++)
      {
      Verts[I].Coord = Verts[I].Coord.Unit3();
      Verts[I].Norm = Verts[I].Coord;
      Verts[I].Norm.W = 0.0f;
      }
   }


//Close namespaces
NAMESPACE_END(NAMESPACE_PROJECT)


//==== End of file ===========================================================
#endif
