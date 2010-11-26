Shader{
name = "deferred",
pass1_depth_test = false,
pass1_depth_write = false,
pass1_vertex = [[
out vec2 var_texcoord;
void main()
{
	var_texcoord = LOS_texcoord;
	gl_Position = vec4(LOS_coord, 1.0);
}]],
pass1_fragment = [[
in vec2 var_texcoord;]]
.. Shader.los_light_attenuation
.. Shader.los_light_combine
.. Shader.los_light_diffuse
.. Shader.los_light_specular .. [[
vec3 unpackcoord()
{
	float d = texture(LOS_diffuse_texture_3, var_texcoord).r * 2.0 - 1.0;
	vec2 f = var_texcoord * 2.0 - vec2(1.0);
	vec4 v = LOS.matrix_projection_inverse * vec4(f.xy, d, 1.0);
	return v.xyz / v.w;
}
vec3 unpacknormal()
{
	return normalize(texture(LOS_diffuse_texture_2, var_texcoord).xyz * 2.0 - vec3(1.0));
}
vec4 unpackspecular()
{
	vec4 spec = texture(LOS_diffuse_texture_1, var_texcoord);
	spec.a *= 128.0;
	return spec;
}
void main()
{
	vec4 diffuse = texture(LOS_diffuse_texture_0, var_texcoord);
	vec3 coord = unpackcoord();
	vec3 normal = unpacknormal();
	vec4 spec = unpackspecular();
	vec3 lightvector = LOS.light_position_premult - coord;
	float fattn = los_light_attenuation(lightvector, LOS.light_equation);
	float fdiff = los_light_diffuse(lightvector, normal);
	float fspec = los_light_specular(lightvector, normal, spec.a);
	vec4 light = los_light_combine(fattn, fdiff, fspec, LOS.light_ambient,
		LOS.light_diffuse, LOS.light_specular * vec4(spec.rgb, 1.0));
	gl_FragColor = diffuse * light;
}]]}

------------------------------------------------------------------------------

Shader{
name = "deferred-spotlight",
forward_pass1_vertex = [[
out vec2 var_texcoord;
void main()
{
	var_texcoord = LOS_texcoord;
	gl_Position = vec4(LOS_coord, 1.0);
}]],
forward_pass1_fragment = [[
in vec2 var_texcoord;]]
.. Shader.los_light_attenuation
.. Shader.los_light_combine
.. Shader.los_light_diffuse
.. Shader.los_light_specular
.. Shader.los_light_spot
.. Shader.los_shadow_mapping
.. Shader.los_shadow_mapping_pcf .. [[
vec3 unpackcoord()
{
	float d = texture(LOS_diffuse_texture_3, var_texcoord).r * 2.0 - 1.0;
	vec2 f = var_texcoord * 2.0 - vec2(1.0);
	vec4 v = LOS.matrix_projection_inverse * vec4(f.xy, d, 1.0);
	return v.xyz / v.w;
}
vec3 unpacknormal()
{
	return normalize(texture(LOS_diffuse_texture_2, var_texcoord).xyz * 2.0 - vec3(1.0));
}
vec4 unpackspecular()
{
	vec4 spec = texture(LOS_diffuse_texture_1, var_texcoord);
	spec.a *= 128.0;
	return spec;
}
void main()
{
	vec4 diffuse = texture(LOS_diffuse_texture_0, var_texcoord);
	vec3 coord = unpackcoord();
	vec3 normal = unpacknormal();
	vec4 spec = unpackspecular();
	vec3 lightvector = LOS.light_position_premult - coord;
	float fshad = los_shadow_mapping_pcf(LOS.light_matrix * vec4(coord, 1.0), LOS_shadow_texture);
	float fattn = los_light_attenuation(lightvector, LOS.light_equation);
	float fdiff = los_light_diffuse(lightvector, normal);
	float fspot = los_light_spot(lightvector, LOS.light_direction, LOS.light_spot);
	float fspec = los_light_specular(lightvector, normal, spec.a);
	vec4 light = los_light_combine(fattn, fspot * fdiff * fshad, fspec, LOS.light_ambient,
		LOS.light_diffuse, LOS.light_specular * vec4(spec.rgb, 1.0));
	gl_FragColor = diffuse * light;
}]]}
