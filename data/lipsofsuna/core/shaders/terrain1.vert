#version 130
uniform mat4 LOS_matrix_modelviewproj;
uniform vec3 LOS_camera_position;
uniform vec4 LOS_light_position[LIGHTS];
uniform mat4 LOS_matrix_shadow[LIGHTS];
in vec3 vertex;
in vec3 normal;
in vec3 tangent;
in vec4 colour;
in vec2 uv0;
out vec3 F_normal;
out vec3 F_tangent;
out vec2 F_texcoord;
out vec4 F_color;
out float F_splatting;
out vec3 F_lightv[LIGHTS];
out vec3 F_lighthv[LIGHTS];
out vec4 F_shadow[LIGHTS];
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