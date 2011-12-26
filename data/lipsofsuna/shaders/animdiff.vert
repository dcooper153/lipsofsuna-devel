#version 130
uniform mat4 LOS_matrix_modelviewproj;
uniform mat4 LOS_matrix_world_inverse;
uniform vec3 LOS_camera_position;
uniform vec4 LOS_light_position[LIGHTS];
uniform mat4x3 LOS_skeletal_matrix[64];
in vec3 vertex;
in vec3 normal;
in vec2 uv0;
in vec4 blendIndices;
in vec4 blendWeights;
out vec3 F_normal;
out vec2 F_texcoord;
out vec3 F_lightv[LIGHTS];
out vec3 F_lighthv[LIGHTS];
void LOS_skeletal_animation_notan(
	in vec3 vertex, in vec3 normal, in mat4 inverse,
	out vec3 vertex_res, out vec3 normal_res)
{
	vec3 v = vec3(0.0);
	vec3 n = vec3(0.0);
	vec3 t = vec3(0.0);
	float total = 0.0;
	int bones[4] = int[](int(blendIndices.x), int(blendIndices.y), int(blendIndices.z), int(blendIndices.w));
	float weights[4] = float[](blendWeights.x, blendWeights.y, blendWeights.z, blendWeights.w);
	for(int i = 0 ; i < 4 ; i++)
	{
		v += weights[i] * LOS_skeletal_matrix[bones[i]] * vec4(vertex, 1.0);
		n += weights[i] * LOS_skeletal_matrix[bones[i]] * vec4(normal, 0.0);
		total += weights[i];
	}
	vertex_res = (inverse * vec4(v / total, 1.0)).xyz;
	normal_res = (inverse * vec4(normalize(n), 0.0)).xyz;
}
void main()
{
	vec3 t_vertex;
	LOS_skeletal_animation_notan(vertex, normal, LOS_matrix_world_inverse, t_vertex, F_normal);
	F_texcoord = uv0;
	gl_Position = LOS_matrix_modelviewproj * vec4(t_vertex,1.0);
	vec3 eye_dir = normalize(LOS_camera_position - t_vertex.xyz);
	for(int i = 0 ; i < LIGHTS ; i++)
	{
		F_lightv[i] = LOS_light_position[i].xyz - (t_vertex.xyz * LOS_light_position[i].w);
		F_lighthv[i] = normalize(F_lightv[i]) + eye_dir.xyz;
	}
}
