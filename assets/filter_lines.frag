//-- Fragment shader for the line filter --

#version 120

uniform sampler2D Texture;
uniform sampler2D Lines;
uniform float Scale;

const vec3 LumConst = vec3(0.2990, 0.5870, 0.1140);
const float DepthBlur = 1.0;
const float LineBlur = 1.0;
const float ZMax = 1.0 - 1.0 / 65535.0;

void main(void)
   {
   vec2 Coord[3], d, Normal;
   vec4 Texel;

   //Depth value
   float Z = dot(texture2D(Texture, gl_TexCoord[0].st, DepthBlur).rgb, LumConst);
   if (Z > ZMax) {discard;}

   //Scale the line texture sampler
   float t = Scale * Z + Scale;

   //Kernel coordinates
   d = vec2(dFdx(gl_TexCoord[0].s), dFdy(gl_TexCoord[0].t));
   Coord[1] = gl_TexCoord[0].st * t;
   Coord[0] = Coord[1] - d * t;
   Coord[2] = Coord[1] + d * t;

   //Over sample line texture
   Texel  = texture2D(Lines, Coord[0]);
   Texel += texture2D(Lines, vec2(Coord[1].s, Coord[0].t));
   Texel += texture2D(Lines, vec2(Coord[2].s, Coord[0].t));

   Texel += texture2D(Lines, vec2(Coord[0].s, Coord[1].t));
   Texel += texture2D(Lines, Coord[1]);
   Texel += texture2D(Lines, vec2(Coord[2].s, Coord[1].t));

   Texel += texture2D(Lines, vec2(Coord[0].s, Coord[2].t));
   Texel += texture2D(Lines, vec2(Coord[1].s, Coord[2].t));
   Texel += texture2D(Lines, Coord[2]);

   Texel *= vec4(1.0 / 9.0);

   //Sharpen if luminance is above threshold
   if (dot(Texel.rgb, LumConst) > 0.25)
      {
      Texel += texture2D(Lines, Coord[1], LineBlur);
      }

   //Dodge filter
   gl_FragColor = Texel / vec4(1.0 - Z);
   }