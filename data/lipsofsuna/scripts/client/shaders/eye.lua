Shader{
name = "eye",
pass1_color_write = false,
pass1_depth_func = "lequal",
pass1_vertex = [[
void main()
{
	vec4 tmp = LOS_matrix_modelview * vec4(LOS_coord,1.0);
	gl_Position = LOS_matrix_projection * tmp;
}]],
pass1_fragment = [[
in vec2 var_texcoord;
void main()
{
}]],
pass2_depth_func = "lequal",
pass2_depth_write = true,
pass2_vertex = [[
out vec3 var_normal;
out vec2 var_texcoord;
void main()
{
	vec4 tmp = LOS_matrix_modelview * vec4(LOS_coord,1.0);
	var_normal = LOS_matrix_normal * LOS_normal;
	var_texcoord = LOS_texcoord;
	gl_Position = LOS_matrix_projection * tmp;
}]],
pass2_fragment = [[
in vec3 var_normal;
in vec2 var_texcoord;
void main()
{
	vec3 normal = normalize(var_normal);
	vec4 diffuse = texture(LOS_diffuse_texture_0, var_texcoord);
	/* Diffuse. */
	LOS_output_0 = mix(LOS_material_diffuse, vec4(diffuse.rgb,1.0), diffuse.a);
	/* Specular. */
	LOS_output_1.rgb = LOS_material_specular.xyz * LOS_material_specular.a;
	LOS_output_1.a = LOS_material_shininess / 128.0;
	/* Normal. */
	LOS_output_2.xyz = 0.5 * normal + vec3(0.5);
}]]}
