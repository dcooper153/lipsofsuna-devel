//-------------------------------
//BlurV_ps20.glsl
// Vertical Gaussian-Blur pass
//-------------------------------
#version 120

uniform sampler2D Blur0;

varying vec2 F_uv;

void main()
{
	vec2 pos[11];
	pos[0] = vec2(0.0, -5.0 * SAMPLING);
	pos[1] = vec2(0.0, -4.0 * SAMPLING);
	pos[2] = vec2(0.0, -3.0 * SAMPLING);
	pos[3] = vec2(0.0, -2.0 * SAMPLING);
	pos[4] = vec2(0.0, -1.0 * SAMPLING);
	pos[5] = vec2(0.0, 0.0);
	pos[6] = vec2(0.0, 1.0 * SAMPLING);
	pos[7] = vec2(0.0, 2.0 * SAMPLING);
	pos[8] = vec2(0.0, 3.0 * SAMPLING);
	pos[9] = vec2(0.0, 4.0 * SAMPLING);
	pos[10] = vec2(0.0, 5.0 * SAMPLING);
	float samples[11];
	samples[0] = 0.01222447;
	samples[1] = 0.02783468;
	samples[2] = 0.06559061;
	samples[3] = 0.12097757;
	samples[4] = 0.17466632;
	samples[5] = 0.19741265;
	samples[6] = 0.17466632;
	samples[7] = 0.12097757;
	samples[8] = 0.06559061;
	samples[9] = 0.02783468;
	samples[10] = 0.01222447;
	vec4 sum = vec4(0.0);
	for(int i = 0 ; i < 11 ; i++)
	{
		sum += samples[i] * texture2D(Blur0, F_uv + pos[i]);
	}
	gl_FragColor = sum;
}
