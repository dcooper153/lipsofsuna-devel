Shader{
name = "adamantium",
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
pass4_blend = true,
pass4_blend_src = "one",
pass4_blend_dst = "one",
pass4_depth_func = "equal",
pass4_depth_write = false,
pass4_vertex = [[
out fragvar
{
	vec3 coord;
	vec3 lightvector;
	vec3 normal;
	vec2 texcoord;
	vec2 texcoord1;
	float splatting;
} OUT;
void main()
{
	vec4 tmp = LOS_matrix_modelview * vec4(LOS_coord,1.0);
	OUT.coord = tmp.xyz;
	OUT.lightvector = LOS_light_position_premult - tmp.xyz;
	OUT.normal = LOS_matrix_normal * LOS_normal;
	vec3 refr = normalize(reflect(normalize(OUT.coord), normalize(OUT.normal)));
	OUT.texcoord = LOS_texcoord;
	OUT.texcoord1 = 0.99 * LOS_texcoord + 0.88 * (refr.xy + refr.zz);
	OUT.splatting = length(LOS_normal) - 1.0;
	gl_Position = LOS_matrix_projection * tmp;
}]],
pass4_fragment = [[
in fragvar
{
	vec3 coord;
	vec3 lightvector;
	vec3 normal;
	vec2 texcoord;
	vec2 texcoord1;
	float splatting;
} IN;]]
.. Shader.los_light_attenuation
.. Shader.los_light_combine
.. Shader.los_light_diffuse
.. Shader.los_light_specular .. [[
void main()
{
	vec3 normal = normalize(IN.normal);
	vec4 diffuse0 = mix(texture(LOS_diffuse_texture_0, IN.texcoord), texture(LOS_diffuse_texture_0, IN.texcoord1), 0.5);
	vec4 diffuse1 = texture(LOS_diffuse_texture_1, IN.texcoord);
	vec4 diffuse = LOS_material_diffuse * mix(diffuse0, diffuse1, IN.splatting);
	float fattn = los_light_attenuation(IN.lightvector, LOS_light_equation);
	float fdiff = los_light_diffuse(IN.lightvector, normal);
	float fspec = los_light_specular(IN.coord, normal, LOS_material_shininess);
	vec4 light = los_light_combine(fattn, fdiff, fspec, LOS_light_ambient,
		LOS_light_diffuse, LOS_light_specular * LOS_material_specular);
	LOS_output_0 = LOS_material_diffuse * diffuse * light;
}]]}
