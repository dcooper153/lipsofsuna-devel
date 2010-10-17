Shader{
name = "widget",
config = [[
attribute att_coord COORD
attribute att_texcoord TEXCOORD
uniform uni_texture DIFFUSETEXTURE0
uniform uni_materialdiffuse MATERIALDIFFUSE
uniform uni_matrixprojection MATRIXPROJECTION]],

vertex = [[
in vec2 att_coord;
in vec2 att_texcoord;
out vec2 var_texcoord;
uniform mat4 uni_matrixprojection;
void main()
{
	gl_Position = uni_matrixprojection * vec4(att_coord, 0.0, 1.0);
	var_texcoord = att_texcoord;
}]],

fragment = [[
in vec2 var_texcoord;
uniform vec4 uni_materialdiffuse;
uniform sampler2D uni_texture;
void main()
{
	gl_FragColor = uni_materialdiffuse * texture(uni_texture, var_texcoord);
}]]}
