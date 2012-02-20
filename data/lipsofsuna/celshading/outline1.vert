#version 120

uniform float LOS_outline_width;
uniform float LOS_outline_distance_factor;
uniform mat4 LOS_matrix_modelviewproj;
uniform vec3 LOS_camera_position;

attribute vec3 vertex;
attribute vec3 normal;

void main()
{
	float dist = length(vertex - LOS_camera_position);
	vec3 width = normal * (dist * LOS_outline_distance_factor) * LOS_outline_width;
	gl_Position = LOS_matrix_modelviewproj * vec4(vertex + width, 1.0);
}
