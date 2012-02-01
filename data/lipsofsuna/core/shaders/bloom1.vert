#version 120

attribute vec4 vertex;

varying vec2 F_uv;

void main()
{
	vec2 pos = sign(vertex.xy);
	F_uv = 0.5 * vec2(pos.x, -pos.y) + vec2(0.5);
	gl_Position = vec4(pos.xy, 0.0, 1.0);
}
