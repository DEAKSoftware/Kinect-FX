//-- Vertex shader for the fatty filter --

#version 120

//Uniforms
uniform sampler2D Depth;
uniform vec2 d[2];         //Texture coordinate step sizes
uniform vec3 K[10];        //Kernel constants

//Varying objects
varying float Z;
varying vec3 Normal;
varying vec3 LightVector;
varying vec3 ViewVector;

//Controls how fat the normal map should be
float Fatness = length(d[0]) * 10.0;

//Depth luminance
const vec3 DepthLum = vec3(0.2990, 0.5870, 0.1140);
const float DepthScale = 0.1;

//Blur level (mip map LOD bias)
const float Blur = 0.0;

void main(void)
   {
   //Transform texture coords
   gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
   gl_TexCoord[1] = gl_TextureMatrix[1] * gl_MultiTexCoord1;

   //5x5 kernel offsets
   vec2 Coord[5];
   Coord[0] = gl_TexCoord[0].st - d[1];
   Coord[1] = gl_TexCoord[0].st - d[0];
   Coord[2] = gl_TexCoord[0].st;
   Coord[3] = gl_TexCoord[0].st + d[0];
   Coord[4] = gl_TexCoord[0].st + d[1];

   //Edge operator, U direction
   vec4 Texel[20];
   Texel[0] = texture2DLod(Depth, vec2(Coord[4].s, Coord[0].t), Blur) - texture2DLod(Depth, vec2(Coord[0].s, Coord[0].t), Blur);
   Texel[1] = texture2DLod(Depth, vec2(Coord[4].s, Coord[1].t), Blur) - texture2DLod(Depth, vec2(Coord[0].s, Coord[1].t), Blur);
   Texel[2] = texture2DLod(Depth, vec2(Coord[4].s, Coord[2].t), Blur) - texture2DLod(Depth, vec2(Coord[0].s, Coord[2].t), Blur);
   Texel[3] = texture2DLod(Depth, vec2(Coord[4].s, Coord[3].t), Blur) - texture2DLod(Depth, vec2(Coord[0].s, Coord[3].t), Blur);
   Texel[4] = texture2DLod(Depth, vec2(Coord[4].s, Coord[4].t), Blur) - texture2DLod(Depth, vec2(Coord[0].s, Coord[4].t), Blur);

   Texel[5] = texture2DLod(Depth, vec2(Coord[3].s, Coord[0].t), Blur) - texture2DLod(Depth, vec2(Coord[1].s, Coord[0].t), Blur);
   Texel[6] = texture2DLod(Depth, vec2(Coord[3].s, Coord[1].t), Blur) - texture2DLod(Depth, vec2(Coord[1].s, Coord[1].t), Blur);
   Texel[7] = texture2DLod(Depth, vec2(Coord[3].s, Coord[2].t), Blur) - texture2DLod(Depth, vec2(Coord[1].s, Coord[2].t), Blur);
   Texel[8] = texture2DLod(Depth, vec2(Coord[3].s, Coord[3].t), Blur) - texture2DLod(Depth, vec2(Coord[1].s, Coord[3].t), Blur);
   Texel[9] = texture2DLod(Depth, vec2(Coord[3].s, Coord[4].t), Blur) - texture2DLod(Depth, vec2(Coord[1].s, Coord[4].t), Blur);

   //Edge operator, V direction
   Texel[10] = texture2DLod(Depth, vec2(Coord[0].s, Coord[0].t), Blur) - texture2DLod(Depth, vec2(Coord[0].s, Coord[4].t), Blur);
   Texel[11] = texture2DLod(Depth, vec2(Coord[1].s, Coord[0].t), Blur) - texture2DLod(Depth, vec2(Coord[1].s, Coord[4].t), Blur);
   Texel[12] = texture2DLod(Depth, vec2(Coord[2].s, Coord[0].t), Blur) - texture2DLod(Depth, vec2(Coord[2].s, Coord[4].t), Blur);
   Texel[13] = texture2DLod(Depth, vec2(Coord[3].s, Coord[0].t), Blur) - texture2DLod(Depth, vec2(Coord[3].s, Coord[4].t), Blur);
   Texel[14] = texture2DLod(Depth, vec2(Coord[4].s, Coord[0].t), Blur) - texture2DLod(Depth, vec2(Coord[4].s, Coord[4].t), Blur);

   Texel[15] = texture2DLod(Depth, vec2(Coord[0].s, Coord[1].t), Blur) - texture2DLod(Depth, vec2(Coord[0].s, Coord[3].t), Blur);
   Texel[16] = texture2DLod(Depth, vec2(Coord[1].s, Coord[1].t), Blur) - texture2DLod(Depth, vec2(Coord[1].s, Coord[3].t), Blur);
   Texel[17] = texture2DLod(Depth, vec2(Coord[2].s, Coord[1].t), Blur) - texture2DLod(Depth, vec2(Coord[2].s, Coord[3].t), Blur);
   Texel[18] = texture2DLod(Depth, vec2(Coord[3].s, Coord[1].t), Blur) - texture2DLod(Depth, vec2(Coord[3].s, Coord[3].t), Blur);
   Texel[19] = texture2DLod(Depth, vec2(Coord[4].s, Coord[1].t), Blur) - texture2DLod(Depth, vec2(Coord[4].s, Coord[3].t), Blur);

   //Compute luminance from each texel, apply kernel weights, and sum them all
   Normal.s  = dot(Texel[0].rgb, K[0]);
   Normal.s += dot(Texel[1].rgb, K[1]);
   Normal.s += dot(Texel[2].rgb, K[2]);
   Normal.s += dot(Texel[3].rgb, K[3]);
   Normal.s += dot(Texel[4].rgb, K[4]);

   Normal.s += dot(Texel[5].rgb, K[5]);
   Normal.s += dot(Texel[6].rgb, K[6]);
   Normal.s += dot(Texel[7].rgb, K[7]);
   Normal.s += dot(Texel[8].rgb, K[8]);
   Normal.s += dot(Texel[9].rgb, K[9]);

   Normal.t  = dot(Texel[10].rgb, K[0]);
   Normal.t += dot(Texel[11].rgb, K[1]);
   Normal.t += dot(Texel[12].rgb, K[2]);
   Normal.t += dot(Texel[13].rgb, K[3]);
   Normal.t += dot(Texel[14].rgb, K[4]);

   Normal.t += dot(Texel[15].rgb, K[5]);
   Normal.t += dot(Texel[16].rgb, K[6]);
   Normal.t += dot(Texel[17].rgb, K[7]);
   Normal.t += dot(Texel[18].rgb, K[8]);
   Normal.t += dot(Texel[19].rgb, K[9]);

   Z = dot(texture2DLod(Depth, Coord[2], Blur).rgb, DepthLum);
   Normal.p = Z * DepthScale;

   Normal = normalize(Normal);

   //Final vertex position
   vec4 Vertex;
   Vertex.x = gl_Vertex.x + Normal.s * Fatness;
   Vertex.y = gl_Vertex.y - Normal.t * Fatness; //Subtract, as kinect textures are inverted with respect to the OpenGL coordinate system
   Vertex.z = gl_Vertex.z - Z;
   Vertex.w = 1.0;

   gl_Position = gl_ModelViewProjectionMatrix * Vertex;

   //Need vertex in model view space
   Vertex = gl_ModelViewMatrix * gl_Vertex;

   //Combine vertex normal with normal map
   Normal = normalize(gl_NormalMatrix * gl_Normal + Normal);

   //Light related vectors
   LightVector = normalize(gl_LightSource[0].position.xyz - Vertex.xyz);
   ViewVector = -normalize(Vertex.xyz);
   }
