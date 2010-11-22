Shader{
name = "widget",

vertex = [[
out vec2 var_texcoord;
void main()
{
	gl_Position = LOS.matrix_projection * vec4(LOS_coord, 1.0);
	var_texcoord = LOS_texcoord;
}]],

fragment = [[
in vec2 var_texcoord;
void main()
{
	gl_FragColor = LOS.material_diffuse * texture(LOS_diffuse_texture_0, var_texcoord);
}]]}
