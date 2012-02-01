//-------------------------------
//Bloom_ps20.glsl
// Blends using weights the blurred image with the sharp one
// Params:
//   OriginalImageWeight
//   BlurWeight
//-------------------------------
#version 120

uniform sampler2D RT;
uniform sampler2D Blur1;
uniform float OriginalImageWeight;
uniform float BlurWeight;

varying vec2 F_uv;

void main()
{
	vec4 orig = texture2D(RT, F_uv);
	vec4 blur = texture2D(Blur1, F_uv);
	gl_FragColor = (blur * BlurWeight) + (orig * OriginalImageWeight);
}
