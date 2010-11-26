Shader{
name = "widget",
pass1_blend = true,
pass1_blend_src = "src_alpha",
pass1_blend_dst = "one_minus_src_alpha",
pass1_depth_test = false,
pass1_depth_write = false,
pass1_vertex = [[
out vec2 var_texcoord;
void main()
{
	gl_Position = LOS.matrix_projection * vec4(LOS_coord, 1.0);
	var_texcoord = LOS_texcoord;
}]],
pass1_fragment = [[
in vec2 var_texcoord;
void main()
{
	gl_FragColor = LOS.material_diffuse * texture(LOS_diffuse_texture_0, var_texcoord);
}]]}
