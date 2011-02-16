Shader{name = "terrain",

-- Low quality program.
-- No lighting.
low = {
pass1_color_write = false,
pass1_depth_func = "lequal",
pass1_vertex = [[
void main()
{
	vec4 tmp = LOS_matrix_modelview * vec4(LOS_coord,1.0);
	gl_Position = LOS_matrix_projection * tmp;
}]],
pass1_fragment = [[
void main()
{
}]],
pass4_depth_func = "equal",
pass4_depth_write = false,
pass4_vertex = [[
out fragvar
{
	vec2 texcoord;
} OUT;
void main()
{
	vec4 tmp = LOS_matrix_modelview * vec4(LOS_coord,1.0);
	OUT.texcoord = LOS_texcoord;
	gl_Position = LOS_matrix_projection * tmp;
}]],
pass4_fragment = [[
in fragvar
{
	vec2 texcoord;
} IN;
void main()
{
	vec4 diffuse = texture(LOS_diffuse_texture_0, IN.texcoord);
	LOS_output_0 = LOS_material_diffuse * diffuse;
}]]},

-- Medium quality program.
-- Normal mapping, texture splatting, fragment lighting.
medium = {
pass1_color_write = false,
pass1_depth_func = "lequal",
pass1_vertex = [[
void main()
{
	vec4 tmp = LOS_matrix_modelview * vec4(LOS_coord,1.0);
	gl_Position = LOS_matrix_projection * tmp;
}]],
pass1_fragment = [[
void main()
{
}]],
pass4_depth_func = "equal",
pass4_depth_write = false,
pass4_vertex = [[
out fragvar
{
	vec3 coord;
	vec3 lightvector[LOS_LIGHT_MAX];
	vec3 normal;
	vec3 tangent;
	vec2 texcoord;
	float splatting;
} OUT;
void main()
{
	vec4 tmp = LOS_matrix_modelview * vec4(LOS_coord,1.0);
	]] .. Shader.los_lighting_vectors("OUT.lightvector", "tmp.xyz") .. [[
	OUT.coord = tmp.xyz;
	OUT.normal = LOS_matrix_normal * LOS_normal;
	OUT.tangent = LOS_matrix_normal * LOS_tangent;
	OUT.texcoord = LOS_texcoord;
	OUT.splatting = length(LOS_normal) - 1.0;
	gl_Position = LOS_matrix_projection * tmp;
}]],
pass4_fragment = [[
in fragvar
{
	vec3 coord;
	vec3 lightvector[LOS_LIGHT_MAX];
	vec3 normal;
	vec3 tangent;
	vec2 texcoord;
	float splatting;
} IN;]]
.. Shader.los_normal_mapping .. [[
void main()
{
	vec3 tangent = normalize(IN.tangent);
	vec3 normal = los_normal_mapping(IN.normal, tangent, IN.texcoord, LOS_diffuse_texture_1);
	vec4 diffuse0 = texture(LOS_diffuse_texture_0, IN.texcoord);
	vec4 diffuse1 = texture(LOS_diffuse_texture_2, IN.texcoord);
	vec4 diffuse = LOS_material_diffuse * mix(diffuse0, diffuse1, IN.splatting);
	]] .. Shader.los_lighting_default("IN.coord", "normal", "IN.lightvector") .. [[
	LOS_output_0 = LOS_material_diffuse * diffuse * lighting;
}]]}}
