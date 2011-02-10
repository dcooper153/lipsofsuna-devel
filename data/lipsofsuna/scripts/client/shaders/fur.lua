Shader{
name = "fur",
sort = true,
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
--pass4_alpha_to_coverage = true,
pass6_blend = true,
pass6_blend_src = "one",
pass6_blend_dst = "one",
pass6_depth_func = "lequal",
pass6_depth_write = false,
pass6_vertex = [[
out geovar
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
pass6_geometry = [[
layout(triangles) in;
layout(triangle_strip, max_vertices=30) out;
in geovar
{
	vec3 coord;
	vec3 normal;
	vec3 tangent;
	vec2 texcoord;
} IN[3];
out fragvar
{
	float layer;
	vec4 color;
	vec3 coord;
	vec3 lightvector;
	vec3 normal;
	vec3 tangent;
	vec2 texcoord;
	vec2 texcoord2;
} OUT;
void main()
{
	int i;
	int j;
	vec4 color = vec4(1.0, 1.0, 1.0, 1.0);
	for(j = 0 ; j < 10 ; j++)
	{
		OUT.layer = float(j) / 10.0;
		vec4 c = vec4(1.0, 1.0, 1.0, 1.0 - OUT.layer);
		for(i = 0 ; i < gl_VerticesIn ; i++)
		{
			OUT.color = c;
			OUT.coord = IN[i].coord + IN[i].normal * OUT.layer * 0.03;
			OUT.lightvector = LOS_light_position_premult - OUT.coord;
			OUT.normal = IN[i].normal + 0.2 * vec3(sin(OUT.layer), cos(OUT.layer), cos(OUT.layer-1.8));
			OUT.tangent = IN[i].tangent;
			OUT.texcoord = IN[i].texcoord;
			OUT.texcoord2 = IN[i].texcoord + 0.01 * vec2(sin(OUT.layer), cos(OUT.layer));
			gl_Position = LOS_matrix_projection * vec4(OUT.coord, 1.0);
			EmitVertex();
		}
		EndPrimitive();
	}
}]],
pass6_fragment = [[
in fragvar
{
	float layer;
	vec4 color;
	vec3 coord;
	vec3 lightvector;
	vec3 normal;
	vec3 tangent;
	vec2 texcoord;
	vec2 texcoord2;
} IN;]]
.. Shader.los_light_attenuation
.. Shader.los_light_combine
.. Shader.los_light_diffuse
.. Shader.los_light_specular
.. Shader.los_normal_mapping .. [[
void main()
{
	vec3 tangent = normalize(IN.tangent);
	vec3 normal = normalize(IN.normal);
	vec4 diffuse = texture(LOS_diffuse_texture_0, IN.texcoord);
	vec4 mask = texture(LOS_diffuse_texture_1, IN.texcoord2);
	float fattn = los_light_attenuation(IN.lightvector, LOS_light_equation);
	float fdiff = los_light_diffuse(IN.lightvector, normal);
	float fspec = los_light_specular(IN.coord, normal, LOS_material_shininess);
	vec4 light = los_light_combine(fattn, fdiff, fspec, LOS_light_ambient,
		LOS_light_diffuse, LOS_light_specular * LOS_material_specular);
	if(IN.layer <= 0.0001)
		LOS_output_0 = LOS_material_diffuse * diffuse * light;
	else
		LOS_output_0 = LOS_material_diffuse * diffuse * IN.color * mask * light;
	LOS_output_0.rgb *= 0.1;
}]]}
