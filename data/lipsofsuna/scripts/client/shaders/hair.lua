Shader{
name = "hair",
forward_pass1_vertex = [[
out fragvar
{
	vec2 texcoord;
} OUT;
void main()
{
	vec4 tmp = LOS.matrix_modelview * vec4(LOS_coord,1.0);
	OUT.texcoord = LOS_texcoord;
	gl_Position = LOS.matrix_projection * tmp;
}]],
forward_pass1_fragment = [[
in fragvar
{
	vec2 texcoord;
} IN;
void main()
{
	vec4 diffuse = texture(LOS_diffuse_texture_0, IN.texcoord);
	if (diffuse.a < 0.90)
		discard;
	gl_FragColor = vec4(0.0,0.0,0.0,0.0);
}]],
transparent_pass1_vertex = [[
out fragvar
{
	vec3 coord;
	vec3 lightvector;
	vec3 normal;
	vec3 tangent;
	vec2 texcoord;
} OUT;
void main()
{
	vec4 tmp = LOS.matrix_modelview * vec4(LOS_coord,1.0);
	OUT.coord = tmp.xyz;
	OUT.lightvector = LOS.light_position_premult - tmp.xyz;
	OUT.normal = LOS.matrix_normal * LOS_normal;
	OUT.tangent = LOS.matrix_normal * LOS_tangent;
	OUT.texcoord = LOS_texcoord;
	gl_Position = LOS.matrix_projection * tmp;
}]],
transparent_pass1_fragment = [[
in fragvar
{
	vec3 coord;
	vec3 lightvector;
	vec3 normal;
	vec3 tangent;
	vec2 texcoord;
} IN;]]
.. Shader.los_light_attenuation
.. Shader.los_light_combine
.. Shader.los_normal_mapping .. [[
float los_light_diffuse_hair(in vec3 coord, in vec3 normal)
{
	vec3 dir = normalize(coord);
	float coeff = dot(normal, dir);
	return max(0.0, 0.75 * coeff + 0.25);
}
float los_light_specular_hair(in vec3 coord, in vec3 normal, in vec3 tangent, in float shininess)
{
	vec3 refl = reflect(normalize(coord), normal);
	float a = dot(tangent, refl);
	float coeff1 = max(0.0, dot(normal, refl));
	float coeff2 = sqrt(1.0 - a * a);
	float coeff = mix(coeff1, coeff2, 0.6);
	return pow(coeff, shininess);
}
void main()
{
	vec3 tangent = normalize(IN.tangent);
	vec3 normal = los_normal_mapping(IN.normal, tangent, IN.texcoord, LOS_diffuse_texture_1);
	vec4 diffuse = texture(LOS_diffuse_texture_0, IN.texcoord);
	float fattn = los_light_attenuation(IN.lightvector, LOS.light_equation);
	float fdiff = los_light_diffuse_hair(IN.lightvector, normal);
	float fspec = los_light_specular_hair(IN.coord, normal, tangent, LOS.material_shininess);
	vec4 light = los_light_combine(fattn, fdiff, fspec, LOS.light_ambient,
		LOS.light_diffuse, LOS.light_specular * LOS.material_specular);
	gl_FragColor = LOS.material_diffuse * diffuse * light;
	gl_FragColor.a = diffuse.a;
}]]}
