#version 120

uniform mat4 LOS_matrix_modelviewproj;
uniform vec3 LOS_camera_position;
uniform vec4 LOS_light_position[LIGHTS];
uniform mat4 LOS_matrix_shadow[LIGHTS];

attribute vec3 vertex;
attribute vec3 normal;
attribute vec3 tangent;
attribute vec4 colour;
attribute vec2 uv0;

varying vec3 F_normal;
varying vec3 F_tangent;
varying vec2 F_texcoord;
varying vec4 F_color;
varying float F_splatting;
varying vec3 F_lightv[LIGHTS];
varying vec3 F_lighthv[LIGHTS];
varying vec4 F_shadow[LIGHTS];

void main()
{
	F_normal = normal;
	F_tangent = tangent;
	F_texcoord = uv0;
	F_color = colour;
	F_splatting = 1.0 - colour.a;
	gl_Position = LOS_matrix_modelviewproj * vec4(vertex,1.0);
	vec3 eye_dir = normalize(LOS_camera_position - vertex.xyz);
	for(int i = 0 ; i < LIGHTS ; i++)
	{
		F_lightv[i] = LOS_light_position[i].xyz - (vertex.xyz * LOS_light_position[i].w);
		F_lighthv[i] = normalize(F_lightv[i]) + eye_dir.xyz;
		F_shadow[i] = LOS_matrix_shadow[i] * vec4(vertex,1.0);
	}
}
