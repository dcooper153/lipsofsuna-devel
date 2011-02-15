Shader{
name = "particle",
sort = true,
pass6_blend = true,
pass6_blend_src = "src_alpha",
pass6_blend_dst = "one",
pass6_depth_write = false,
pass6_vertex = [[
out geomvar
{
	vec4 color;
	vec2 size;
} OUT;
void main()
{
	OUT.color = vec4(LOS_normal,LOS_texcoord.x);
	OUT.size = LOS_texcoord.yy;
	gl_Position = LOS_matrix_modelview * vec4(LOS_coord,1.0);
}]],
pass6_geometry = [[
layout(triangles) in;
layout(triangle_strip, max_vertices=4) out;
in geomvar
{
	vec4 color;
	vec2 size;
} IN[3];
out fragvar
{
	vec4 color;
	vec2 texcoord;
} OUT;
void main()
{
	vec3 ctr = gl_PositionIn[0].xyz;
	vec3 size = vec3(IN[0].size.xy, 0.0);
	vec4 vx = vec4(IN[0].size.x, 0.0, 0.0, 1.0);
	vec4 vy = vec4(0.0, IN[0].size.y, 0.0, 1.0);
	OUT.color = IN[0].color;
	OUT.texcoord = vec2(0.0, 0.0);
	gl_Position = LOS_matrix_projection * vec4(ctr - size, 1.0);
	EmitVertex();
	OUT.texcoord = vec2(1.0, 0.0);
	gl_Position = LOS_matrix_projection * vec4(ctr + size.xzz - size.zyz, 1.0);
	EmitVertex();
	OUT.texcoord = vec2(0.0, 1.0);
	gl_Position = LOS_matrix_projection * vec4(ctr - size.xzz + size.zyz, 1.0);
	EmitVertex();
	OUT.texcoord = vec2(1.0, 1.0);
	gl_Position = LOS_matrix_projection * vec4(ctr + size, 1.0);
	EmitVertex();
	EndPrimitive();
}]],
pass6_fragment = [[
in fragvar
{
	vec4 color;
	vec2 texcoord;
} IN;
void main()
{
	LOS_output_0 = IN.color * texture(LOS_diffuse_texture_0, IN.texcoord);
}]]}
