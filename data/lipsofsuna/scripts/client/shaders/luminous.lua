Shader{
name = "luminous",
pass3_depth_func = "lequal",
pass3_vertex = [[
void main()
{
	vec4 tmp = LOS_matrix_modelview * vec4(LOS_coord,1.0);
	gl_Position = LOS_matrix_projection * tmp;
}]],
pass3_fragment = [[
void main()
{
	LOS_output_0 = vec4(2.0, 2.0, 2.0, 1.0);
}]]}

