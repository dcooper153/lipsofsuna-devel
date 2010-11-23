Shader{
name = "shadowmap",
forward_pass1_vertex = [[
void main()
{
	vec4 tmp = LOS.matrix_modelview * vec4(LOS_coord,1.0);
	gl_Position = LOS.matrix_projection * tmp;
}]],
forward_pass1_fragment = [[
void main()
{
}]]}
