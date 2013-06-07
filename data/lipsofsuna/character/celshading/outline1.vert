#version 120

uniform float LOS_outline_width1;
uniform float LOS_outline_width2;
uniform float LOS_outline_distance_factor;
uniform mat4 LOS_matrix_proj;
uniform mat4 LOS_matrix_modelview;
uniform mat4 LOS_matrix_modelviewproj;

attribute vec3 vertex;
attribute vec3 normal;

float los_outline_width()
{
	vec4 v = LOS_matrix_modelview * vec4(vertex, 1.0);
	float d = length(v);
	float s = d * LOS_matrix_proj[0].x;
	float f = max(0.0, 1.0 - LOS_outline_distance_factor * d);
	return max(LOS_outline_width1, LOS_outline_width2 * f) * s;
}

void main()
{
	vec3 width = normal * los_outline_width();
	gl_Position = LOS_matrix_modelviewproj * vec4(vertex + width, 1.0);
}
