#version 130
out vec2 F_uv;

void main()
{
	vec2 pos = sign(gl_Vertex.xy);
	F_uv = 0.5 * vec2(pos.x, -pos.y) + vec2(0.5);
	gl_Position = ftransform();
}
