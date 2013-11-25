#version 120

uniform mat4 LOS_matrix_modelviewproj;

attribute vec3 vertex;

varying float F_scale;

void main()
{
	gl_Position = LOS_matrix_modelviewproj * vec4(vertex, 1.0);
}
