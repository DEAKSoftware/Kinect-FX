//-- Fragment shader for the depth buffer with palette --

#version 120

uniform sampler2D Texture;
uniform sampler2D Palette;
uniform float Select;         //Palette table selection (in normalised texture coordinates)

void main(void)
   {
   vec4 Texel = texture2D(Texture, gl_TexCoord[0].st);
   float Lookup = 1.0 - dot(Texel.rgb, vec3(0.2990, 0.5870, 0.1140));

   Lookup = clamp(Lookup, 0.0, 1.0);

   gl_FragColor = texture2D(Palette, vec2(Lookup, Select));
   }