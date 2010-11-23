Shader{
name = "default",
deferred_pass1_vertex = [[
out vec3 var_normal;
out vec2 var_texcoord;
void main()
{
	vec4 tmp = LOS.matrix_modelview * vec4(LOS_coord,1.0);
	var_normal = LOS.matrix_normal * LOS_normal;
	var_texcoord = LOS_texcoord;
	gl_Position = LOS.matrix_projection * tmp;
}]],
deferred_pass1_fragment = [[
in vec3 var_normal;
in vec2 var_texcoord;
void main()
{
	vec3 normal = normalize(var_normal);
	vec4 diffuse = texture(LOS_diffuse_texture_0, var_texcoord);
	if(diffuse.a < 0.5)
		discard;
	/* Diffuse. */
	gl_FragData[0] = LOS.material_diffuse * diffuse;
	/* Specular. */
	gl_FragData[1].rgb = LOS.material_specular.xyz * LOS.material_specular.a;
	gl_FragData[1].a = LOS.material_shininess / 128.0;
	/* Normal. */
	gl_FragData[2].xyz = 0.5 * normal + vec3(0.5);
}]]}
