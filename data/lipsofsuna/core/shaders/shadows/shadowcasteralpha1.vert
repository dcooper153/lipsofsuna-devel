#version 120

uniform mat4 LOS_matrix_modelviewproj;

attribute vec3 vertex;
attribute vec2 uv0;

varying vec2 F_depth;
varying vec2 F_texcoord;

void main()
{
	gl_Position = LOS_matrix_modelviewproj * vec4(vertex,1.0);
	F_texcoord = uv0;
	F_depth = gl_Position.zw;
}
