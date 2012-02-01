#version 120

uniform mat4 LOS_matrix_modelviewproj;

attribute vec3 vertex;
attribute vec3 normal;

varying float F_alpha;

void main()
{
	F_alpha = 0.5 * normal.x;
	gl_Position = LOS_matrix_modelviewproj * vec4(vertex,1.0);
}
