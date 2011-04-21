Shader{name = "skin",

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
-- Vertex lighting, fake skin diffuse.
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
	vec3 normal;
	vec2 texcoord;
	vec4 light;
} OUT;
void main()
{
	vec4 tmp = LOS_matrix_modelview * vec4(LOS_coord,1.0);
	vec3 coord = tmp.xyz;
	OUT.normal = LOS_matrix_normal * LOS_normal;
	OUT.texcoord = LOS_texcoord;
	vec3 halfvector[LOS_LIGHT_MAX];
	vec3 lightvector[LOS_LIGHT_MAX];
	]] .. Shader.los_lighting_vectors("lightvector", "halfvector", "tmp.xyz") .. [[
	]] .. Shader.los_lighting_skin("coord", "OUT.normal", "lightvector", "halfvector") .. [[
	OUT.light = lighting;
	gl_Position = LOS_matrix_projection * tmp;
}]],
pass4_fragment = [[
in fragvar
{
	vec3 normal;
	vec2 texcoord;
	vec4 light;
} IN;
void main()
{
	vec3 normal = normalize(IN.normal);
	vec4 diffuse = texture(LOS_diffuse_texture_0, IN.texcoord);
	float fake = abs(dot(normal, vec3(1.0,0.0,0.0))) + abs(dot(normal, vec3(0.0,1.0,0.0)));
	diffuse = mix(LOS_material_diffuse * diffuse, vec4(0.5,0.0,0.0,1.0), 0.3 * fake - 0.15);
	LOS_output_0 = diffuse * IN.light;
}]]},

-- High quality program.
-- Normal mapping, fragment lighting, fake skin diffuse.
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
	vec3 tangent;
	vec2 texcoord;
} OUT;
void main()
{
	vec4 tmp = LOS_matrix_modelview * vec4(LOS_coord,1.0);
	]] .. Shader.los_lighting_vectors("OUT.lightvector", "OUT.halfvector", "tmp.xyz") .. [[
	OUT.coord = tmp.xyz;
	OUT.normal = LOS_matrix_normal * LOS_normal;
	OUT.tangent = LOS_matrix_normal * LOS_tangent;
	OUT.texcoord = LOS_texcoord;
	gl_Position = LOS_matrix_projection * tmp;
}]],
pass4_fragment = Shader.los_normal_mapping .. [[
in fragvar
{
	vec3 coord;
	vec3 halfvector[LOS_LIGHT_MAX];
	vec3 lightvector[LOS_LIGHT_MAX];
	vec3 normal;
	vec3 tangent;
	vec2 texcoord;
} IN;
void main()
{
	vec3 tangent = normalize(IN.tangent);
	vec3 normal = los_normal_mapping(IN.normal, tangent, IN.texcoord, LOS_diffuse_texture_1);
	vec4 diffuse = texture(LOS_diffuse_texture_0, IN.texcoord);
	]] .. Shader.los_lighting_skin("IN.coord", "normal", "IN.lightvector", "IN.halfvector") .. [[
	float fake1 = 0.5 * abs(dot(normal, vec3(1.0,0.0,0.0))) + abs(dot(normal, vec3(0.0,1.0,0.0)));
	float fake2 = 1.0 - abs(dot(normal, vec3(0.0,0.0,1.0)));
	float fake = mix(fake1, fake2, 0.7);
	diffuse = LOS_material_diffuse * mix(diffuse, vec4(1.0,0.0,0.0,1.0), 0.3 * fake);
	LOS_output_0 = diffuse * lighting;
}]]}}
