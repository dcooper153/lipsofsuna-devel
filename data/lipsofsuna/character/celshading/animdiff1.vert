#version 120

uniform mat4 LOS_matrix_modelviewproj;
uniform mat4 LOS_matrix_world_inverse;
uniform vec3 LOS_camera_position;
uniform vec4 LOS_light_position[LIGHTS];
uniform mat4x3 LOS_skeletal_matrix[BONES];

attribute vec3 vertex;
attribute vec3 normal;
attribute vec2 uv0;
attribute vec4 blendIndices;
attribute vec4 blendWeights;

varying vec3 F_normal;
varying vec2 F_texcoord;
varying vec3 F_eyev;
varying vec3 F_lightv[LIGHTS];

void main()
{
	vec3 skel_v = blendWeights.x * LOS_skeletal_matrix[int(blendIndices.x)] * vec4(vertex, 1.0) +
		blendWeights.y * LOS_skeletal_matrix[int(blendIndices.y)] * vec4(vertex, 1.0) +
		blendWeights.z * LOS_skeletal_matrix[int(blendIndices.z)] * vec4(vertex, 1.0) +
		blendWeights.w * LOS_skeletal_matrix[int(blendIndices.w)] * vec4(vertex, 1.0);
	vec3 skel_n = blendWeights.x * LOS_skeletal_matrix[int(blendIndices.x)] * vec4(normal, 0.0) +
		blendWeights.y * LOS_skeletal_matrix[int(blendIndices.y)] * vec4(normal, 0.0) +
		blendWeights.z * LOS_skeletal_matrix[int(blendIndices.z)] * vec4(normal, 0.0) +
		blendWeights.w * LOS_skeletal_matrix[int(blendIndices.w)] * vec4(normal, 0.0);
	float skel_w = blendWeights.x + blendWeights.y + blendWeights.z + blendWeights.w;
	vec3 t_vertex = (LOS_matrix_world_inverse * vec4(skel_v / skel_w, 1.0)).xyz;
	F_normal = (LOS_matrix_world_inverse * vec4(normalize(skel_n), 0.0)).xyz;

	F_texcoord = uv0;
	gl_Position = LOS_matrix_modelviewproj * vec4(t_vertex,1.0);
	F_eyev = normalize(LOS_camera_position - vertex.xyz);
	for(int i = 0 ; i < LIGHTS ; i++)
	{
		F_lightv[i] = LOS_light_position[i].xyz - (vertex.xyz * LOS_light_position[i].w);
	}
}
