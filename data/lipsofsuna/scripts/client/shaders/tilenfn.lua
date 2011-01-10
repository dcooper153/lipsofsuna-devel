Shader{
name = "tilenfn",
pass2_vertex = [[
out vec3 var_coord;
out vec3 var_normal;
out vec3 var_tangent;
out vec2 var_texcoord;
void main()
{
	vec4 tmp = LOS_matrix_modelview * vec4(LOS_coord,1.0);
	var_coord = tmp.xyz;
	var_normal = LOS_matrix_normal * LOS_normal;
	var_tangent = LOS_matrix_normal * LOS_tangent;
	gl_Position = LOS_matrix_projection * tmp;
	/* Texture translation. */
	vec3 tex = (LOS_matrix_model * vec4(LOS_coord,1.0)).xyz;
	vec3 nml = mat3(LOS_matrix_model) * LOS_normal;
	float lx = abs(dot(nml,vec3(1.0,0.0,0.0)));
	float ly = abs(dot(nml,vec3(0.0,1.0,0.0)));
	float lz = abs(dot(nml,vec3(0.0,0.0,1.0)));
	if(lx > ly && lx > lz)
		var_texcoord.xy = tex.yz;
	else if(ly > lx && ly > lz)
		var_texcoord.xy = tex.xz;
	else
		var_texcoord.xy = tex.xy;
	var_texcoord *= (1.0 - LOS_material_param_0.x);
}]],
pass2_fragment = [[
in vec3 var_normal;
in vec3 var_tangent;
in vec2 var_texcoord;
]] .. Shader.los_normal_mapping .. [[
void main()
{
	vec3 normal = los_normal_mapping(var_normal, var_tangent, var_texcoord, LOS_diffuse_texture_1);
	vec4 texture = texture2D(LOS_diffuse_texture_0, var_texcoord);
	/* Diffuse. */
	LOS_output_0 = LOS_material_diffuse * texture;
	/* Specular. */
	LOS_output_1.rgb = LOS_material_specular.xyz * LOS_material_specular.a;
	LOS_output_1.a = LOS_material_shininess / 128.0;
	/* Normal. */
	LOS_output_2.xyz = 0.5 * normal + vec3(0.5);
}]]}
