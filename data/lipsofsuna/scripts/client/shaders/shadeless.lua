Shader{
name = "forward-shadeless",
config = [[
attribute att_coord COORD
attribute att_texcoord TEXCOORD
uniform uni_matrixmodelview MATRIXMODELVIEW
uniform uni_matrixprojection MATRIXPROJECTION
uniform uni_texskin DIFFUSETEXTURE0]],

vertex = [[
#version 150
in vec3 att_coord;
in vec2 att_texcoord;
out vec2 var_texcoord;
uniform mat4 uni_matrixmodelview;
uniform mat4 uni_matrixprojection;
void main()
{
	vec4 tmp = uni_matrixmodelview * vec4(att_coord,1.0);
	gl_Position = uni_matrixprojection * tmp;
	var_texcoord = att_texcoord;
}]],

fragment = [[
#version 150
in vec2 var_texcoord;
uniform sampler2D uni_texskin;
void main()
{
	gl_FragColor = texture(uni_texskin, var_texcoord);
}]]}
