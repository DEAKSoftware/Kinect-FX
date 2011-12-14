//-- Vertex shader for rendering solids with coloured texture mapping --

#version 120

//Uniforms
uniform sampler2D Depth;
uniform sampler2D Video;
uniform vec2 TexCoord;
uniform float Range;

//Varying objects
varying vec3 Normal;
varying vec3 LightVector;
varying vec3 ViewVector;
varying vec4 Texel;
varying float SpecularAtten;

//Depth luminance
const vec3 DepthLum = vec3(0.2990, 0.5870, 0.1140);

//Blur level (mip map LOD bias)
const float Blur = 2.0;

void main(void)
   {
   gl_TexCoord[0] = gl_TextureMatrix[0] * vec4(TexCoord, 0.0, 1.0);
   gl_TexCoord[1] = gl_TextureMatrix[1] * vec4(TexCoord, 0.0, 1.0);

   Texel = texture2DLod(Depth, gl_TexCoord[0].st, Blur);
   float Z = dot(Texel.rgb, DepthLum);
   SpecularAtten = clamp(1.0 - Z, 0.0, 1.0);

   //Vertex offset
   vec4 Vertex = gl_Vertex;
   Vertex.z -= Z * Range;

   //Base diffuse colour
   Texel.rgb = vec3(1.0) - Texel.rgb;
   Texel *= texture2DLod(Video, gl_TexCoord[1].st, Blur);

   //Final vertex position
   gl_Position = gl_ModelViewProjectionMatrix * Vertex;

   //Need vertex in model view space
   Vertex = gl_ModelViewMatrix * Vertex;

   //Find transformed normal
   Normal = normalize(gl_NormalMatrix * gl_Normal);

   //Light related vectors
   LightVector = normalize(gl_LightSource[0].position.xyz - Vertex.xyz);
   ViewVector = -normalize(Vertex.xyz);
   }
