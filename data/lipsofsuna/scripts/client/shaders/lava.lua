Shader{name = "lava",

-- High quality program.
-- Fragment lighting, procedural diffuse, alpha to coverage.
high = {
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
	vec3 coord;
	vec3 halfvector[LOS_LIGHT_MAX];
	vec3 lightvector[LOS_LIGHT_MAX];
	vec3 normal;
	vec2 texcoord;
} OUT;
void main()
{
	vec4 tmp = LOS_matrix_modelview * vec4(LOS_coord,1.0);
	]] .. Shader.los_lighting_vectors("OUT.lightvector", "OUT.halfvector", "tmp.xyz") .. [[
	OUT.coord = LOS_coord.xyz;
	OUT.normal = LOS_matrix_normal * LOS_normal;
	OUT.texcoord = LOS_texcoord;
	gl_Position = LOS_matrix_projection * tmp;
}]],
pass4_fragment = [[
in fragvar
{
	vec3 coord;
	vec3 halfvector[LOS_LIGHT_MAX];
	vec3 lightvector[LOS_LIGHT_MAX];
	vec3 normal;
	vec2 texcoord;
} IN;
float swirlynoise(in vec3 p)
{
	vec4 x = vec4(0.1,0.2,0.3,0.4);
	vec4 v1 = texture(LOS_diffuse_texture_0, 0.09*p.xy*vec2(1.0,1.5));
	vec4 v2 = texture(LOS_diffuse_texture_0, 0.11*p.xz*vec2(0.5,1.0));
	vec4 v3 = texture(LOS_diffuse_texture_0, 0.13*p.yz*vec2(1.5,0.5));
	vec4 c1 = texture(LOS_diffuse_texture_0, v1.ra + v2.gb);
	vec4 c2 = texture(LOS_diffuse_texture_0, v1.gb + v2.ra);
	return dot(vec4(c1.r,c1.g,c1.b,c1.a), x) *
	       dot(vec4(c2.r,c1.g,c1.b,c2.a), x) *
	       dot(vec4(c1.r,c2.g,c1.b,c1.a), x) *
	       dot(vec4(c2.r,c2.g,c1.b,c2.a), x) *
	       dot(vec4(c1.r,c1.g,c2.b,c1.a), x) *
	       dot(vec4(c2.r,c1.g,c2.b,c2.a), x) *
	       dot(vec4(c1.r,c2.g,c2.b,c1.a), x) *
	       dot(vec4(c2.r,c2.g,c2.b,c2.a), x) * 10.0;
}
void main()
{
	vec3 normal = normalize(IN.normal);
	vec3 x = 1.5*IN.coord.xyz + vec3(0.0, 0.05*LOS_time, 0.0);
	vec3 y = 1.5*IN.coord.yzx + vec3(0.0, -0.15*LOS_time, 0.0);
	float a = swirlynoise(1.0 * x) + swirlynoise(2.0 * y);
	a = pow(2.7, a);
	x = IN.coord.xyz + vec3(0.0, -0.15*LOS_time, -0.15*LOS_time);
	y = IN.coord.yzx + vec3(0.0, -0.15*LOS_time, -0.15*LOS_time);
	float b = swirlynoise(2.5 * vec3(x.x, 0.8*x.y, x.z)) + swirlynoise(2.5 * vec3(x.x+0.05, 0.8*x.y, x.z+0.05));
	b = pow(4.0, b);
	vec4 diffuse = vec4(0.1+0.7*a,0.1+0.3*a,0.2*a,1.0) * vec4(0.1+b,0.7*b,0.7*b,1.0);
	]] .. Shader.los_lighting_default("IN.coord", "normal", "IN.lightvector", "IN.halfvector") .. [[
	lighting = vec4(0.1) + lighting * 0.9;
	LOS_output_0 = LOS_material_diffuse * diffuse * lighting * vec4(1.0,1.0,1.0,1.0);
}]]}}
