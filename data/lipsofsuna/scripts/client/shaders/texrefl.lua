Shader{
	name = "deferred-texrefl",
	config = [[
uniform uni_materialdiffuse MATERIALDIFFUSE
uniform uni_materialspecular MATERIALSPECULAR
uniform uni_materialshininess MATERIALSHININESS
uniform uni_matrixmodelview MATRIXMODELVIEW
uniform uni_matrixnormal MATRIXNORMAL
uniform uni_matrixprojection MATRIXPROJECTION
uniform uni_texturemap[0] DIFFUSETEXTURE0]],

	vertex = [[
out vec3 var_coord;
out vec3 var_normal;
out vec2 var_texcoord;
uniform mat4 uni_matrixmodelview;
uniform mat3 uni_matrixnormal;
uniform mat4 uni_matrixprojection;
void main()
{
	vec4 tmp = uni_matrixmodelview * vec4(LOS_coord,1.0);
	var_coord = tmp.xyz;
	var_normal = uni_matrixnormal * LOS_normal;
	gl_Position = uni_matrixprojection * tmp;
	vec3 refr = normalize(reflect(normalize(var_coord), normalize(var_normal)));
	var_texcoord = LOS_texcoord + refr.xy + refr.zz;
}]],

	fragment = [[
in vec3 var_coord;
in vec3 var_normal;
in vec2 var_texcoord;
uniform vec4 uni_materialdiffuse;
uniform vec4 uni_materialspecular;
uniform float uni_materialshininess;
uniform sampler2D uni_texturemap[1];
void main()
{
	vec3 normal = normalize(var_normal);
	vec4 texture = texture2D(uni_texturemap[0], var_texcoord);
	/* Diffuse. */
	gl_FragData[0] = uni_materialdiffuse * texture;
	/* Specular. */
	gl_FragData[1].rgb = uni_materialspecular.xyz * uni_materialspecular.a;
	gl_FragData[1].a = uni_materialshininess / 128.0;
	/* Normal. */
	gl_FragData[2].xyz = 0.5 * normal + vec3(0.5);
}]]}
