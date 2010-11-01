Shader{
	name = "forward-glass",
	config = [[
uniform uni_lightambient LIGHTAMBIENT0
uniform uni_lightdiffuse LIGHTDIFFUSE0
uniform uni_lightequation LIGHTEQUATION0
uniform uni_lightposition LIGHTPOSITIONPREMULT0
uniform uni_lightspecular LIGHTSPECULAR0
uniform uni_matrixnormal MATRIXNORMAL
uniform uni_materialdiffuse MATERIALDIFFUSE
uniform uni_materialspecular MATERIALSPECULAR
uniform uni_materialshininess MATERIALSHININESS
uniform uni_matrixmodel MATRIXMODEL
uniform uni_matrixmodelview MATRIXMODELVIEW
uniform uni_matrixnormal MATRIXNORMAL
uniform uni_matrixprojection MATRIXPROJECTION
uniform uni_shadowmap SHADOWTEXTURE0
uniform uni_texturemap DIFFUSETEXTURE0]],

	vertex = [[
out vec3 var_coord;
out vec3 var_normal;
out vec2 var_texcoord;
out vec3 var_lightdir;
uniform vec3 uni_lightposition;
uniform vec4 uni_materialdiffuse;
uniform mat4 uni_matrixmodelview;
uniform mat3 uni_matrixnormal;
uniform mat4 uni_matrixprojection;
void main()
{
	vec4 tmp = uni_matrixmodelview * vec4(LOS_coord,1.0);
	var_coord = tmp.xyz;
	var_normal = uni_matrixnormal * LOS_normal;
	var_texcoord = LOS_texcoord;
	var_lightdir = uni_lightposition - var_coord;
	gl_Position = uni_matrixprojection * tmp;
}]],

	fragment = [[
in vec3 var_coord;
in vec3 var_normal;
in vec2 var_texcoord;
in vec3 var_lightdir;
uniform vec4 uni_lightambient;
uniform vec4 uni_lightdiffuse;
uniform vec3 uni_lightequation;
uniform vec3 uni_lightposition;
uniform vec4 uni_lightspecular;
uniform vec4 uni_materialdiffuse;
uniform vec4 uni_materialspecular;
uniform float uni_materialshininess;
uniform sampler2D uni_texturemap;
uniform sampler2DShadow uni_shadowmap;]]
.. Shader.los_light_attenuation
.. Shader.los_light_combine
.. Shader.los_light_diffuse
.. Shader.los_light_specular .. [[
void main()
{
	vec3 normal = normalize(var_normal);
	vec4 diffuse = texture2D(uni_texturemap, var_texcoord);
	float fattn = los_light_attenuation(var_lightdir, uni_lightequation);
	float fdiff = los_light_diffuse(var_lightdir, normal);
	float fspec = los_light_specular(var_coord, normal, uni_materialshininess);
	vec4 light = los_light_combine(fattn, fdiff, fspec, uni_lightambient,
		uni_lightdiffuse, uni_lightspecular * uni_materialspecular);
	gl_FragColor = uni_materialdiffuse * diffuse * light;
	gl_FragColor.a = max(uni_materialdiffuse.a * diffuse.a, fattn * fspec);
}]]}
