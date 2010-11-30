Shader{
name = "shadeless",
pass3_vertex = [[
out vec2 var_texcoord;
void main()
{
	vec4 tmp = LOS_matrix_modelview * vec4(LOS_coord,1.0);
	gl_Position = LOS_matrix_projection * tmp;
	var_texcoord = LOS_texcoord;
}]],
pass3_fragment = [[
in vec2 var_texcoord;
void main()
{
	gl_FragColor = texture(LOS_diffuse_texture_0, var_texcoord);
}]]}
