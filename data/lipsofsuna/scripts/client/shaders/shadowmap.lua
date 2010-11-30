Shader{
name = "shadowmap",
pass1_vertex = [[
void main()
{
	vec4 tmp = LOS_matrix_modelview * vec4(LOS_coord,1.0);
	gl_Position = LOS_matrix_projection * tmp;
}]],
pass1_fragment = [[
void main()
{
}]]}
