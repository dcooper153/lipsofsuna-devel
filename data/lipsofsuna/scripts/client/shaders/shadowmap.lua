Shader{
name = "shadowmap",
config = [[
uniform uni_matrixmodelview MATRIXMODELVIEW
uniform uni_matrixprojection MATRIXPROJECTION]],

vertex = [[
uniform mat4 uni_matrixmodelview;
uniform mat4 uni_matrixprojection;
void main()
{
	vec4 tmp = uni_matrixmodelview * vec4(LOS_coord,1.0);
	gl_Position = uni_matrixprojection * tmp;
}]],

fragment = [[
void main()
{
}]]}
