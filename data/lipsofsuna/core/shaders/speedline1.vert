#version 130

uniform mat4 LOS_matrix_modelviewproj;

in vec3 vertex;
in vec3 normal;

out float F_alpha;

void main()
{
	F_alpha = 0.5 * normal.x;
	gl_Position = LOS_matrix_modelviewproj * vec4(vertex,1.0);
}
