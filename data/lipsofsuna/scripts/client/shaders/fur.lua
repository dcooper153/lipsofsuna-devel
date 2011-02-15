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
pass6_blend = true,
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
	vec3 lightvector[LOS_LIGHT_MAX];
	vec3 normal;
	vec2 texcoord;
	vec2 texcoord2;
} OUT;
void main()
{
	int i;
	int j;
	for(j = 0 ; j < 10 ; j++)
	{
		OUT.layer = float(j) / 10.0;
		vec4 color = vec4(1.0, 1.0, 1.0, 1.0 - OUT.layer);
		for(i = 0 ; i < gl_VerticesIn ; i++)
		{
			OUT.color = color;
			OUT.coord = IN[i].coord + IN[i].normal * OUT.layer * 0.03;
			]] .. Shader.los_lighting_vectors("OUT.lightvector", "OUT.coord") .. [[
			OUT.normal = IN[i].normal + 0.2 * vec3(sin(OUT.layer), cos(OUT.layer), cos(OUT.layer-1.8));
			OUT.texcoord = IN[i].texcoord;
			OUT.texcoord2 = IN[i].texcoord + 0.1 * vec2(sin(OUT.layer), cos(OUT.layer));
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
	vec3 lightvector[LOS_LIGHT_MAX];
	vec3 normal;
	vec2 texcoord;
	vec2 texcoord2;
} IN;
void main()
{
	vec3 normal = normalize(IN.normal);
	vec4 diffuse = texture(LOS_diffuse_texture_0, IN.texcoord);
	vec4 mask = texture(LOS_diffuse_texture_1, IN.texcoord2);
	]] .. Shader.los_lighting_default("IN.coord", "normal", "IN.lightvector") .. [[
	if(IN.layer <= 0.0001)
		LOS_output_0 = LOS_material_diffuse * diffuse * lighting;
	else
		LOS_output_0 = LOS_material_diffuse * diffuse * IN.color * mask * lighting;
}]]}
