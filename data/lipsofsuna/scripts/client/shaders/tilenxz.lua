Shader{
	name = "deferred-tilenxz",
	config = [[
attribute att_coord COORD
attribute att_normal NORMAL
attribute att_tangent TANGENT
uniform uni_param PARAM0
uniform uni_materialdiffuse MATERIALDIFFUSE
uniform uni_materialspecular MATERIALSPECULAR
uniform uni_materialshininess MATERIALSHININESS
uniform uni_matrixmodel MATRIXMODEL
uniform uni_matrixmodelview MATRIXMODELVIEW
uniform uni_matrixnormal MATRIXNORMAL
uniform uni_matrixprojection MATRIXPROJECTION
uniform uni_texturemap[0] DIFFUSETEXTURE0
uniform uni_texturemap[1] DIFFUSETEXTURE1]],

	vertex = [[
in vec3 att_coord;
in vec3 att_normal;
in vec3 att_tangent;
out vec3 var_coord;
out vec3 var_normal;
out vec3 var_tangent;
out vec2 var_texcoord;
uniform vec4 uni_materialdiffuse;
uniform vec4 uni_param;
uniform mat4 uni_matrixmodel;
uniform mat4 uni_matrixmodelview;
uniform mat3 uni_matrixnormal;
uniform mat4 uni_matrixprojection;
void main()
{
	vec4 tmp = uni_matrixmodelview * vec4(att_coord,1.0);
	var_coord = tmp.xyz;
	var_normal = uni_matrixnormal * att_normal;
	var_tangent = uni_matrixnormal * att_tangent;
	gl_Position = uni_matrixprojection * tmp;
	/* Texture translation. */
	vec3 tex = (uni_matrixmodel * vec4(att_coord,1.0)).xyz;
	vec3 u = mat3(uni_matrixmodel) * vec3(1.0,0.0,0.0);
	vec3 v = mat3(uni_matrixmodel) * vec3(0.0,0.0,1.0);
	var_texcoord.xy = vec2(abs(dot(tex, u)), abs(dot(tex,v)));
	var_texcoord *= (1.0 - uni_param.x);
}]],

	fragment = [[
in vec3 var_coord;
in vec3 var_normal;
in vec3 var_tangent;
in vec2 var_texcoord;
uniform vec4 uni_materialdiffuse;
uniform float uni_materialshininess;
uniform vec4 uni_materialspecular;
uniform sampler2D uni_texturemap[2];
]] .. Shader.normalmapping .. [[
void main()
{
	vec3 normal = normalmapping();
	vec4 texture = texture2D(uni_texturemap[0], var_texcoord);
	/* Diffuse. */
	gl_FragData[0] = uni_materialdiffuse * texture;
	/* Specular. */
	gl_FragData[1].rgb = uni_materialspecular.xyz * uni_materialspecular.a;
	gl_FragData[1].a = uni_materialshininess / 128.0;
	/* Normal. */
	gl_FragData[2].xyz = 0.5 * normal + vec3(0.5);
}]]}
