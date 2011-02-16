Shader{name = "hair",

-- Medium quality program.
medium = {
pass1_alpha_to_coverage = true,
pass1_color_write = false,
pass1_depth_func = "lequal",
pass1_vertex = [[
out vec2 var_texcoord;
void main()
{
	vec4 tmp = LOS_matrix_modelview * vec4(LOS_coord,1.0);
	var_texcoord = LOS_texcoord;
	gl_Position = LOS_matrix_projection * tmp;
}]],
pass1_fragment = [[
in vec2 var_texcoord;
void main()
{
	LOS_output_0.rgb = vec3(0.0);
	LOS_output_0.a = texture(LOS_diffuse_texture_0, var_texcoord).a;
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
} OUT;
void main()
{
	vec4 tmp = LOS_matrix_modelview * vec4(LOS_coord,1.0);
	]] .. Shader.los_lighting_vectors("OUT.lightvector", "tmp.xyz") .. [[
	OUT.coord = tmp.xyz;
	OUT.normal = LOS_matrix_normal * LOS_normal;
	OUT.tangent = LOS_matrix_normal * LOS_tangent;
	OUT.texcoord = LOS_texcoord;
	gl_Position = LOS_matrix_projection * tmp;
}]],
pass4_fragment = Shader.los_normal_mapping .. [[
in fragvar
{
	vec3 coord;
	vec3 lightvector[LOS_LIGHT_MAX];
	vec3 normal;
	vec3 tangent;
	vec2 texcoord;
} IN;
void main()
{
	vec3 tangent = normalize(IN.tangent);
	vec3 normal = los_normal_mapping(IN.normal, tangent, IN.texcoord, LOS_diffuse_texture_1);
	vec4 diffuse = texture(LOS_diffuse_texture_0, IN.texcoord);
	if(normal.z < 0)
		normal = -normal;
	]] .. Shader.los_lighting_hair("IN.coord", "normal", "tangent", "IN.lightvector") .. [[
	LOS_output_0 = LOS_material_diffuse * diffuse * lighting;
}]]}}
