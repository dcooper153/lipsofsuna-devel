Shader{
name = "particle",
config = [[
uniform uni_matrixmodelview MATRIXMODELVIEW
uniform uni_matrixprojection MATRIXPROJECTION
uniform uni_diffusetexture DIFFUSETEXTURE0]],

vertex = [[
out geomvar
{
	vec4 color;
	vec2 size;
} OUT;
uniform mat4 uni_matrixmodelview;
void main()
{
	OUT.color = vec4(LOS_normal,LOS_texcoord.x);
	OUT.size = LOS_texcoord.yy;
	gl_Position = uni_matrixmodelview * vec4(LOS_coord,1.0);
}]],

geometry = [[
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
uniform mat4 uni_matrixprojection;
void main()
{
	vec3 ctr = gl_PositionIn[0].xyz;
	vec3 size = vec3(IN[0].size.xy, 0.0);
	vec4 vx = vec4(IN[0].size.x, 0.0, 0.0, 1.0);
	vec4 vy = vec4(0.0, IN[0].size.y, 0.0, 1.0);
	OUT.color = IN[0].color;
	OUT.texcoord = vec2(0.0, 0.0);
	gl_Position = uni_matrixprojection * vec4(ctr - size, 1.0);
	EmitVertex();
	OUT.texcoord = vec2(1.0, 0.0);
	gl_Position = uni_matrixprojection * vec4(ctr + size.xzz - size.zyz, 1.0);
	EmitVertex();
	OUT.texcoord = vec2(0.0, 1.0);
	gl_Position = uni_matrixprojection * vec4(ctr - size.xzz + size.zyz, 1.0);
	EmitVertex();
	OUT.texcoord = vec2(1.0, 1.0);
	gl_Position = uni_matrixprojection * vec4(ctr + size, 1.0);
	EmitVertex();
	EndPrimitive();
}]],

fragment = [[
in fragvar
{
	vec4 color;
	vec2 texcoord;
} IN;
uniform sampler2D uni_diffusetexture;
void main()
{
	gl_FragColor = IN.color * texture(uni_diffusetexture, IN.texcoord);
}]]}
