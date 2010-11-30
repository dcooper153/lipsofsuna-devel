Shader{
name = "texrefl",
pass2_vertex = [[
out vec3 var_coord;
out vec3 var_normal;
out vec2 var_texcoord;
void main()
{
	vec4 tmp = LOS_matrix_modelview * vec4(LOS_coord,1.0);
	var_coord = tmp.xyz;
	var_normal = LOS_matrix_normal * LOS_normal;
	gl_Position = LOS_matrix_projection * tmp;
	vec3 refr = normalize(reflect(normalize(var_coord), normalize(var_normal)));
	var_texcoord = LOS_texcoord + refr.xy + refr.zz;
}]],
pass2_fragment = [[
in vec3 var_coord;
in vec3 var_normal;
in vec2 var_texcoord;
void main()
{
	vec3 normal = normalize(var_normal);
	vec4 texture = texture2D(LOS_diffuse_texture_0, var_texcoord);
	/* Diffuse. */
	gl_FragData[0] = LOS_material_diffuse * texture;
	/* Specular. */
	gl_FragData[1].rgb = LOS_material_specular.xyz * LOS_material_specular.a;
	gl_FragData[1].a = LOS_material_shininess / 128.0;
	/* Normal. */
	gl_FragData[2].xyz = 0.5 * normal + vec3(0.5);
}]]}
