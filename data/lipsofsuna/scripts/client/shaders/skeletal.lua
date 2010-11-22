Shader{
	name = "skeletal",
	transform_feedback = true,

	vertex = [[
out vec3 geo_coord;
out vec3 geo_normal;
out vec2 geo_texcoord;
vec4 los_quat_mul(in vec4 a, in vec4 b)
{
	return vec4(
		(a.w * b.x) + (a.x * b.w) + (a.y * b.z) - (a.z * b.y),
		(a.w * b.y) - (a.x * b.z) + (a.y * b.w) + (a.z * b.x),
		(a.w * b.z) + (a.x * b.y) - (a.y * b.x) + (a.z * b.w),
		(a.w * b.w) - (a.x * b.x) - (a.y * b.y) - (a.z * b.z));
}
vec3 los_quat_xform(in vec4 q, in vec3 v)
{
	vec4 a = los_quat_mul(q, vec4(v, 0.0));
	vec4 b = q * vec4(-1.0, -1.0, -1.0, 1.0);
	return los_quat_mul(a, b).xyz;
}
void main()
{
	int i;
	int bone[8] = int[](
		int(LOS_bones1.x), int(LOS_bones1.y), int(LOS_bones1.z), int(LOS_bones1.w),
		int(LOS_bones2.x), int(LOS_bones2.y), int(LOS_bones2.z), int(LOS_bones2.w));
	float weight[8] = float[](
		LOS_weights1.x, LOS_weights1.y, LOS_weights1.z, LOS_weights1.w,
		LOS_weights2.x, LOS_weights2.y, LOS_weights2.z, LOS_weights2.w);
	vec3 vtx = vec3(0.0);
	vec3 nml = vec3(0.0);
	for (i = 0 ; i < 8 ; i++)
	{
		int offset = 3 * bone[i];
		vec3 restpos = texelFetch(LOS_buffer_texture, offset).xyz;
		vec4 posepos = texelFetch(LOS_buffer_texture, offset + 1);
		vec4 poserot = texelFetch(LOS_buffer_texture, offset + 2);
		vtx += weight[i] * (los_quat_xform(poserot, (LOS_coord - restpos) * posepos.w) + posepos.xyz);
		nml += weight[i] * (los_quat_xform(poserot, LOS_normal));
	}
	geo_coord = vtx;
	geo_normal = normalize(nml);
	geo_texcoord = LOS_texcoord;
	gl_Position = vec4(LOS_coord,1.0);
}]],

	geometry = [[
layout(triangles) in;
layout(triangle_strip, max_vertices=3) out;
in vec3 geo_coord[3];
in vec3 geo_normal[3];
in vec2 geo_texcoord[3];
out vec3 LOS_out_coord;
out vec3 LOS_out_normal;
out vec3 LOS_out_tangent;
out vec2 LOS_out_texcoord;
vec3 los_triangle_tangent(in vec3 co0, in vec3 co1, in vec3 co2, in vec2 uv0, in vec2 uv1, in vec2 uv2)
{
	vec3 ed0 = co1 - co0;
	vec3 ed1 = co2 - co0;
	return normalize(ed1 * (uv1.y - uv0.y) - ed0 * (uv2.y - uv0.y));
}
void main()
{
	int i;
	LOS_out_tangent = los_triangle_tangent(
		geo_coord[0], geo_coord[1], geo_coord[2],
		geo_texcoord[0], geo_texcoord[1], geo_texcoord[2]);
	for(i = 0 ; i < gl_VerticesIn ; i++)
	{
		LOS_out_coord = geo_coord[i];
		LOS_out_texcoord = geo_texcoord[i];
		LOS_out_normal = geo_normal[i];
		gl_Position = gl_PositionIn[i];
		EmitVertex();
	}
	EndPrimitive();
}]],

	fragment = [[
void main()
{
}]]}
