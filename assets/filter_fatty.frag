//-- Fragment shader for the fatty filter --

#version 120

uniform sampler2D Video;

varying float Z;
varying vec3 Normal;
varying vec3 LightVector;
varying vec3 ViewVector;

const float ZMax = 1.0 - 1.0 / 65535.0;

void main(void)
   {
   if (Z > ZMax) {discard;}

   //gl_FragColor = vec4(vec3(0.5) + Normal * vec3(0.5), 1.0);

   //Normalise interpolated vectors
   vec3 N = normalize(Normal);
   vec3 L = normalize(LightVector);

   //Emission term
   vec4 Colour = gl_FrontMaterial.emission;

   //Ambient term
   Colour += gl_FrontLightModelProduct.sceneColor * gl_FrontMaterial.ambient;
   Colour += gl_LightSource[0].ambient * gl_FrontMaterial.ambient;

   float LdotN = dot(L, N);
   if (LdotN > 0.0)
      {
      //Diffuse term
      Colour += gl_LightSource[0].diffuse * gl_FrontMaterial.diffuse * texture2D(Video, gl_TexCoord[1].st) * LdotN;

      //Dot product between view and reflection vector
      vec3 V = normalize(ViewVector);
      vec3 R = reflect(-L, N);
      float RdotV = dot(R, V);

      //Specular term
      if (RdotV > 0.0 && gl_FrontMaterial.shininess > 0.0)
         {
         float Specular = pow(RdotV, gl_FrontMaterial.shininess);
         Colour += gl_LightSource[0].specular * gl_FrontMaterial.specular * Specular;
         }
      }

   gl_FragColor = Colour;
   }