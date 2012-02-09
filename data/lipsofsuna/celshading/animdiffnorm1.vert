#version 120

uniform mat4 LOS_matrix_modelviewproj;
uniform mat4 LOS_matrix_world_inverse;
uniform vec3 LOS_camera_position;
uniform vec4 LOS_light_position[LIGHTS];
uniform mat4x3 LOS_skeletal_matrix[BONES];

attribute vec3 vertex;
attribute vec3 normal;
attribute vec3 tangent;
attribute vec2 uv0;
attribute vec4 blendIndices;
attribute vec4 blendWeights;

varying vec3 F_normal;
varying vec3 F_tangent;
varying vec2 F_texcoord;
varying vec3 F_eyev;
varying vec3 F_lightv[LIGHTS];

void LOS_skeletal_animation(
	in vec3 vertex, in vec3 normal, in vec3 tangent, in mat4 inverse,
	out vec3 vertex_res, out vec3 normal_res, out vec3 tangent_res)
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
		t += weights[i] * LOS_skeletal_matrix[bones[i]] * vec4(tangent, 0.0);
		total += weights[i];
	}
	vertex_res = (inverse * vec4(v / total, 1.0)).xyz;
	normal_res = (inverse * vec4(normalize(n), 0.0)).xyz;
	tangent_res = (inverse * vec4(normalize(t), 0.0)).xyz;
}

void main()
{
	vec3 t_vertex;
	LOS_skeletal_animation(vertex, normal, tangent,
		LOS_matrix_world_inverse, t_vertex, F_normal, F_tangent);
	F_texcoord = uv0;
	gl_Position = LOS_matrix_modelviewproj * vec4(t_vertex,1.0);
	F_eyev = normalize(LOS_camera_position - vertex.xyz);
	for(int i = 0 ; i < LIGHTS ; i++)
	{
		F_lightv[i] = LOS_light_position[i].xyz - (vertex.xyz * LOS_light_position[i].w);
	}
}
