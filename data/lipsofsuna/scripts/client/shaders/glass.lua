Shader{
name = "glass",
transparent_pass1_vertex = [[
out vec3 var_coord;
out vec3 var_normal;
out vec2 var_texcoord;
out vec3 var_lightdir;
void main()
{
	vec4 tmp = LOS.matrix_modelview * vec4(LOS_coord,1.0);
	var_coord = tmp.xyz;
	var_normal = LOS.matrix_normal * LOS_normal;
	var_texcoord = LOS_texcoord;
	var_lightdir = LOS.light_position_premult - var_coord;
	gl_Position = LOS.matrix_projection * tmp;
}]],
transparent_pass1_fragment = [[
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
	float fattn = los_light_attenuation(var_lightdir, LOS.light_equation);
	float fdiff = los_light_diffuse(var_lightdir, normal);
	float fspec = los_light_specular(var_coord, normal, LOS.material_shininess);
	vec4 light = los_light_combine(fattn, fdiff, fspec, LOS.light_ambient,
		LOS.light_diffuse, LOS.light_specular * LOS.material_specular);
	gl_FragColor = LOS.material_diffuse * diffuse * light;
	gl_FragColor.a = max(LOS.material_diffuse.a * diffuse.a, fattn * fspec);
}]]}
