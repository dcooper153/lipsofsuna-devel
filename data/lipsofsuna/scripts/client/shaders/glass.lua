Shader{
name = "glass",
sort = true,
pass6_depth_func = "lequal",
pass6_depth_write = false,
pass6_vertex = [[
out vec3 var_coord;
out vec3 var_normal;
out vec2 var_texcoord;
out vec3 var_lightdir;
void main()
{
	vec4 tmp = LOS_matrix_modelview * vec4(LOS_coord,1.0);
	var_coord = tmp.xyz;
	var_normal = LOS_matrix_normal * LOS_normal;
	var_texcoord = LOS_texcoord;
	var_lightdir = LOS_light_position_premult - var_coord;
	gl_Position = LOS_matrix_projection * tmp;
}]],
pass6_blend = true,
pass6_blend_src = "one",
pass6_blend_dst = "one",
pass6_depth_func = "lequal",
pass6_depth_write = false,
pass6_fragment = [[
in vec3 var_coord;
in vec3 var_normal;
in vec2 var_texcoord;
in vec3 var_lightdir;]]
.. Shader.los_light_attenuation
.. Shader.los_light_combine
.. Shader.los_light_diffuse
.. Shader.los_light_specular .. [[
void main()
{
	vec3 normal = normalize(var_normal);
	vec4 diffuse = texture2D(LOS_diffuse_texture_0, var_texcoord);
	float fattn = los_light_attenuation(var_lightdir, LOS_light_equation);
	float fdiff = los_light_diffuse(var_lightdir, normal);
	float fspec = los_light_specular(var_coord, normal, LOS_material_shininess);
	vec4 light = los_light_combine(fattn, fdiff, fspec, LOS_light_ambient,
		LOS_light_diffuse, LOS_light_specular * LOS_material_specular);
	LOS_output_0 = LOS_material_diffuse * diffuse * light;
	LOS_output_0.a = max(LOS_material_diffuse.a * diffuse.a, fattn * fspec);
}]]}
