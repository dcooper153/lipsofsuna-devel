Shader{name = "fur",

-- Low quality program.
-- No lighting.
low = {
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
pass4_depth_func = "equal",
pass4_depth_write = false,
pass4_vertex = [[
out fragvar
{
	vec2 texcoord;
} OUT;
void main()
{
	vec4 tmp = LOS_matrix_modelview * vec4(LOS_coord,1.0);
	OUT.texcoord = LOS_texcoord;
	gl_Position = LOS_matrix_projection * tmp;
}]],
pass4_fragment = [[
in fragvar
{
	vec2 texcoord;
} IN;
void main()
{
	vec4 diffuse = texture(LOS_diffuse_texture_0, IN.texcoord);
	LOS_output_0 = LOS_material_diffuse * diffuse;
}]]},

-- Medium quality program.
-- Vertex lighting.
medium = {
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
pass4_depth_func = "equal",
pass4_depth_write = false,
pass4_vertex = [[
out fragvar
{
	vec4 light;
	vec2 texcoord;
} OUT;
void main()
{
	vec4 tmp = LOS_matrix_modelview * vec4(LOS_coord,1.0);
	vec3 normal = LOS_matrix_normal * LOS_normal;
	OUT.texcoord = LOS_texcoord;
	vec3 lightvector[LOS_LIGHT_MAX];
	]] .. Shader.los_lighting_vectors("lightvector", "tmp.xyz") .. [[
	]] .. Shader.los_lighting_default("tmp.xyz", "normal", "lightvector") .. [[
	OUT.light = lighting;
	gl_Position = LOS_matrix_projection * tmp;
}]],
pass4_fragment = [[
in fragvar
{
	vec4 light;
	vec2 texcoord;
} IN;
void main()
{
	vec4 diffuse = texture(LOS_diffuse_texture_0, IN.texcoord);
	LOS_output_0 = LOS_material_diffuse * diffuse * IN.light;
}]]},

-- High quality program.
-- Vertex lighting, geometry layers.
high = {
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
	vec4 light;
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
	vec3 lightvector[LOS_LIGHT_MAX];
	]] .. Shader.los_lighting_vectors("lightvector", "tmp.xyz") .. [[
	]] .. Shader.los_lighting_default("tmp.xyz", "OUT.normal", "lightvector") .. [[
	OUT.light = lighting;
	gl_Position = LOS_matrix_projection * tmp;
}]],
pass6_geometry = [[
layout(triangles) in;
layout(triangle_strip, max_vertices=30) out;
in geovar
{
	vec3 coord;
	vec4 light;
	vec3 normal;
	vec3 tangent;
	vec2 texcoord;
} IN[3];
out fragvar
{
	float layer;
	vec4 color;
	vec4 light;
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
			OUT.light = IN[i].light;
			OUT.texcoord = IN[i].texcoord;
			OUT.texcoord2 = IN[i].texcoord + 0.1 * vec2(sin(OUT.layer), cos(OUT.layer));
			vec3 coord = IN[i].coord + IN[i].normal * OUT.layer * 0.03;
			gl_Position = LOS_matrix_projection * vec4(coord, 1.0);
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
	vec4 light;
	vec2 texcoord;
	vec2 texcoord2;
} IN;
void main()
{
	vec4 diffuse = texture(LOS_diffuse_texture_0, IN.texcoord);
	vec4 mask = texture(LOS_diffuse_texture_1, IN.texcoord2);
	if(IN.layer <= 0.0001)
		LOS_output_0 = LOS_material_diffuse * diffuse * IN.light;
	else
		LOS_output_0 = LOS_material_diffuse * diffuse * IN.color * mask * IN.light;
}]]}}
