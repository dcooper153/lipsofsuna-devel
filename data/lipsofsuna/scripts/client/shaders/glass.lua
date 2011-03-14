Shader{name = "glass",

-- Medium quality program.
medium = {
sort = true,
pass6_blend = true,
pass6_blend_src = "src_alpha",
pass6_blend_dst = "one_minus_src_alpha",
pass6_depth_func = "lequal",
pass6_depth_write = false,
pass6_vertex = [[
out fragvar
{
	vec3 coord;
	vec3 halfvector[LOS_LIGHT_MAX];
	vec3 lightvector[LOS_LIGHT_MAX];
	vec3 normal;
	vec2 texcoord;
} OUT;
void main()
{
	vec4 tmp = LOS_matrix_modelview * vec4(LOS_coord,1.0);
	]] .. Shader.los_lighting_vectors("OUT.lightvector", "OUT.halfvector", "tmp.xyz") .. [[
	OUT.coord = tmp.xyz;
	OUT.normal = LOS_matrix_normal * LOS_normal;
	OUT.texcoord = LOS_texcoord;
	gl_Position = LOS_matrix_projection * tmp;
}]],
pass6_fragment = [[
in fragvar
{
	vec3 coord;
	vec3 halfvector[LOS_LIGHT_MAX];
	vec3 lightvector[LOS_LIGHT_MAX];
	vec3 normal;
	vec2 texcoord;
} IN;
void main()
{
	vec3 normal = normalize(IN.normal);
	vec4 diffuse = texture2D(LOS_diffuse_texture_0, IN.texcoord);
	]] .. Shader.los_lighting_default("IN.coord", "normal", "IN.lightvector", "IN.halfvector") .. [[
	LOS_output_0 = LOS_material_diffuse * diffuse * lighting;
}]]}}
