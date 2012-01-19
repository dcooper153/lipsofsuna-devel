//-------------------------------
//BlurH_ps20.glsl
// Horizontal Gaussian-Blur pass
//-------------------------------
#version 130

uniform sampler2D Blur0;

in vec2 F_uv;

vec2 pos[11] = vec2[11](
	vec2(-5.0 * SAMPLING, 0.0),
	vec2(-4.0 * SAMPLING, 0.0),
	vec2(-3.0 * SAMPLING, 0.0),
	vec2(-2.0 * SAMPLING, 0.0),
	vec2(-1.0 * SAMPLING, 0.0),
	vec2(0.0, 0.0),
	vec2(1.0 * SAMPLING, 0.0),
	vec2(2.0 * SAMPLING, 0.0),
	vec2(3.0 * SAMPLING, 0.0),
	vec2(4.0 * SAMPLING, 0.0),
	vec2(5.0 * SAMPLING, 0.0));
float samples[11] = float[11](
	0.01222447,
	0.02783468,
	0.06559061,
	0.12097757,
	0.17466632,
	0.19741265,
	0.17466632,
	0.12097757,
	0.06559061,
	0.02783468,
	0.01222447);

void main()
{
    vec4 sum = vec4(0.0);
    for(int i = 0 ; i < 11 ; i++)
    {
        sum += samples[i] * texture(Blur0, F_uv + pos[i]);
    }
    gl_FragColor = sum;
}
