Shader{
name = "widget",
config = [[
uniform uni_texture DIFFUSETEXTURE0
uniform uni_materialdiffuse MATERIALDIFFUSE
uniform uni_matrixprojection MATRIXPROJECTION]],

vertex = [[
out vec2 var_texcoord;
uniform mat4 uni_matrixprojection;
void main()
{
	gl_Position = uni_matrixprojection * vec4(LOS_coord, 1.0);
	var_texcoord = LOS_texcoord;
}]],

fragment = [[
in vec2 var_texcoord;
uniform vec4 uni_materialdiffuse;
uniform sampler2D uni_texture;
void main()
{
	gl_FragColor = uni_materialdiffuse * texture(uni_texture, var_texcoord);
}]]}
