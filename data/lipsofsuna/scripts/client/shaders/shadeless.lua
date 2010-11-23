Shader{
name = "shadeless",
forward_pass1_vertex = [[
out vec2 var_texcoord;
void main()
{
	vec4 tmp = LOS.matrix_modelview * vec4(LOS_coord,1.0);
	gl_Position = LOS.matrix_projection * tmp;
	var_texcoord = LOS_texcoord;
}]],
forward_pass1_fragment = [[
in vec2 var_texcoord;
void main()
{
	gl_FragColor = texture(LOS_diffuse_texture_0, var_texcoord);
}]]}
