//-------------------------------
//Bloom_ps20.glsl
// Blends using weights the blurred image with the sharp one
// Params:
//   OriginalImageWeight
//   BlurWeight
//-------------------------------
#version 130

uniform sampler2D RT;
uniform sampler2D Blur1;
uniform float OriginalImageWeight;
uniform float BlurWeight;

in vec2 F_uv;

out vec4 colour;

void main()
{
	vec4 orig = texture(RT, F_uv);
	vec4 blur = texture(Blur1, F_uv);
	colour = (blur * BlurWeight) + (orig * OriginalImageWeight);
}
