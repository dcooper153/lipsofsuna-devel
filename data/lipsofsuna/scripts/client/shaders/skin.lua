Shader{
name = "skin",
pass2_depth_func = "lequal",
pass2_vertex = [[
out fragvar
{
	vec3 coord;
	vec3 normal;
	vec3 tangent;
	vec2 texcoord;
} OUT;
void main()
{
	vec4 tmp = LOS_matrix_modelview * vec4(LOS_coord,1.0);
	OUT.coord = tmp.xyz;
	OUT.normal = LOS_matrix_normal * LOS_normal;
	OUT.tangent = LOS_matrix_normal * LOS_tangent;
	OUT.texcoord = LOS_texcoord;
	gl_Position = LOS_matrix_projection * tmp;
}]],
pass2_fragment = [[
in fragvar
{
	vec3 coord;
	vec3 normal;
	vec3 tangent;
	vec2 texcoord;
} IN;]]
.. Shader.los_normal_mapping .. [[
void main()
{
	vec3 tangent = normalize(IN.tangent);
	vec3 normal = los_normal_mapping(IN.normal, tangent, IN.texcoord, LOS_diffuse_texture_1);
	vec4 diffuse = texture(LOS_diffuse_texture_0, IN.texcoord);
	if(diffuse.a < 0.5)
		discard;
	/* Diffuse. */
	float fake = abs(dot(normal, vec3(1.0,0.0,0.0))) + abs(dot(normal, vec3(0.0,1.0,0.0)));
	gl_FragData[0] = mix(LOS_material_diffuse * diffuse, vec4(0.5,0.0,0.0,1.0), 0.3 * fake - 0.15);
	/* Specular. */
	gl_FragData[1].rgb = LOS_material_specular.xyz * LOS_material_specular.a;
	gl_FragData[1].a = LOS_material_shininess / 128.0;
	/* Normal. */
	gl_FragData[2].xyz = 0.5 * normal + vec3(0.5);
}]]}
