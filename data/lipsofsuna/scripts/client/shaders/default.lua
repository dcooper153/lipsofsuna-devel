Shader{
	name = "deferred-default",
	config = [[
attribute att_coord COORD
attribute att_normal NORMAL
attribute att_texcoord TEXCOORD
uniform uni_materialdiffuse MATERIALDIFFUSE
uniform uni_materialspecular MATERIALSPECULAR
uniform uni_materialshininess MATERIALSHININESS
uniform uni_matrixmodelview MATRIXMODELVIEW
uniform uni_matrixnormal MATRIXNORMAL
uniform uni_matrixprojection MATRIXPROJECTION
uniform uni_texturemap[0] DIFFUSETEXTURE0]],

	vertex = [[
#version 150
in vec3 att_coord;
in vec3 att_normal;
in vec2 att_texcoord;
out vec3 var_normal;
out vec2 var_texcoord;
uniform mat4 uni_matrixmodelview;
uniform mat3 uni_matrixnormal;
uniform mat4 uni_matrixprojection;
void main()
{
	vec4 tmp = uni_matrixmodelview * vec4(att_coord,1.0);
	var_normal = uni_matrixnormal * att_normal;
	var_texcoord = att_texcoord;
	gl_Position = uni_matrixprojection * tmp;
}]],

	fragment = [[
#version 150
in vec3 var_normal;
in vec2 var_texcoord;
uniform vec4 uni_materialdiffuse;
uniform vec4 uni_materialspecular;
uniform float uni_materialshininess;
uniform sampler2D uni_texturemap[1];
void main()
{
	vec3 normal = normalize(var_normal);
	vec4 diffuse = texture(uni_texturemap[0], var_texcoord);
	if(diffuse.a < 0.5)
		discard;
	/* Diffuse. */
	gl_FragData[0] = uni_materialdiffuse * diffuse;
	/* Specular. */
	gl_FragData[1].rgb = uni_materialspecular.xyz * uni_materialspecular.a;
	gl_FragData[1].a = uni_materialshininess / 128.0;
	/* Normal. */
	gl_FragData[2].xyz = 0.5 * normal + vec3(0.5);
}]]}
