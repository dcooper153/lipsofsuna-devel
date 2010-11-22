Shader{
name = "shadowmap",

vertex = [[
void main()
{
	vec4 tmp = LOS.matrix_modelview * vec4(LOS_coord,1.0);
	gl_Position = LOS.matrix_projection * tmp;
}]],

fragment = [[
void main()
{
}]]}
