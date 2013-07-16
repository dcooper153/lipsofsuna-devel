#version 120

uniform mat4 LOS_matrix_modelviewproj;
uniform mat4 LOS_matrix_world_inverse;
uniform mat4x3 LOS_skeletal_matrix[BONES];

attribute vec3 vertex;
attribute vec4 blendIndices;
attribute vec4 blendWeights;

varying vec2 F_depth;

void main()
{
	vec3 skel_v = blendWeights.x * LOS_skeletal_matrix[int(blendIndices.x)] * vec4(vertex, 1.0) +
		blendWeights.y * LOS_skeletal_matrix[int(blendIndices.y)] * vec4(vertex, 1.0) +
		blendWeights.z * LOS_skeletal_matrix[int(blendIndices.z)] * vec4(vertex, 1.0) +
		blendWeights.w * LOS_skeletal_matrix[int(blendIndices.w)] * vec4(vertex, 1.0);
	float skel_w = blendWeights.x + blendWeights.y + blendWeights.z + blendWeights.w;
	vec3 t_vertex = (LOS_matrix_world_inverse * vec4(skel_v / skel_w, 1.0)).xyz;

	gl_Position = LOS_matrix_modelviewproj * vec4(t_vertex,1.0);
	F_depth = gl_Position.zw;
}
