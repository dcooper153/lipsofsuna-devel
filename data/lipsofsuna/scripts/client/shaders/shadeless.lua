Shader{
name = "forward-shadeless",
config = [[
uniform uni_matrixmodelview MATRIXMODELVIEW
uniform uni_matrixprojection MATRIXPROJECTION
uniform uni_texskin DIFFUSETEXTURE0]],

vertex = [[
out vec2 var_texcoord;
uniform mat4 uni_matrixmodelview;
uniform mat4 uni_matrixprojection;
void main()
{
	vec4 tmp = uni_matrixmodelview * vec4(LOS_coord,1.0);
	gl_Position = uni_matrixprojection * tmp;
	var_texcoord = LOS_texcoord;
}]],

fragment = [[
in vec2 var_texcoord;
uniform sampler2D uni_texskin;
void main()
{
	gl_FragColor = texture(uni_texskin, var_texcoord);
}]]}
