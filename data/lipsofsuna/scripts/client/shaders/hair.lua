Shader{
	name = "forward-hair",
	config = [[
uniform uni_lightambient LIGHTAMBIENT0
uniform uni_lightdiffuse LIGHTDIFFUSE0
uniform uni_lightequation LIGHTEQUATION0
uniform uni_lightposition LIGHTPOSITIONPREMULT0
uniform uni_lightspecular LIGHTSPECULAR0
uniform uni_materialdiffuse MATERIALDIFFUSE
uniform uni_materialshininess MATERIALSHININESS
uniform uni_materialspecular MATERIALSPECULAR
uniform uni_matrixmodelview MATRIXMODELVIEW
uniform uni_matrixnormal MATRIXNORMAL
uniform uni_matrixprojection MATRIXPROJECTION
uniform uni_normalmap DIFFUSETEXTURE1
uniform uni_texturemap DIFFUSETEXTURE0]],

	vertex = [[
out fragvar
{
	vec3 coord;
	vec3 lightvector;
	vec3 normal;
	vec3 tangent;
	vec2 texcoord;
} OUT;
uniform vec3 uni_lightposition;
uniform mat4 uni_matrixmodelview;
uniform mat3 uni_matrixnormal;
uniform mat4 uni_matrixprojection;
void main()
{
	vec4 tmp = uni_matrixmodelview * vec4(LOS_coord,1.0);
	OUT.coord = tmp.xyz;
	OUT.lightvector = uni_lightposition - tmp.xyz;
	OUT.normal = uni_matrixnormal * LOS_normal;
	OUT.tangent = uni_matrixnormal * LOS_tangent;
	OUT.texcoord = LOS_texcoord;
	gl_Position = uni_matrixprojection * tmp;
}]],

	fragment = [[
in fragvar
{
	vec3 coord;
	vec3 lightvector;
	vec3 normal;
	vec3 tangent;
	vec2 texcoord;
} IN;
uniform vec4 uni_lightambient;
uniform vec4 uni_lightdiffuse;
uniform vec3 uni_lightequation;
uniform vec4 uni_lightspecular;
uniform vec4 uni_materialdiffuse;
uniform float uni_materialshininess;
uniform vec4 uni_materialspecular;
uniform sampler2D uni_normalmap;
uniform sampler2D uni_texturemap;]]
.. Shader.los_light_attenuation
.. Shader.los_light_combine
.. Shader.los_normal_mapping .. [[
float los_light_diffuse_hair(in vec3 coord, in vec3 normal)
{
	vec3 dir = normalize(coord);
	float coeff = dot(normal, dir);
	return max(0.0, 0.75 * coeff + 0.25);
}
float los_light_specular_hair(in vec3 coord, in vec3 normal, in vec3 tangent, in float shininess)
{
	vec3 refl = reflect(normalize(coord), normal);
	float a = dot(tangent, refl);
	float coeff1 = max(0.0, dot(normal, refl));
	float coeff2 = sqrt(1.0 - a * a);
	float coeff = mix(coeff1, coeff2, 0.6);
	return pow(coeff, shininess);
}
void main()
{
	vec3 tangent = normalize(IN.tangent);
	vec3 normal = los_normal_mapping(IN.normal, tangent, IN.texcoord, uni_normalmap);
	vec4 diffuse = texture(uni_texturemap, IN.texcoord);
	float fattn = los_light_attenuation(IN.lightvector, uni_lightequation);
	float fdiff = los_light_diffuse_hair(IN.lightvector, normal);
	float fspec = los_light_specular_hair(IN.coord, normal, tangent, uni_materialshininess);
	vec4 light = los_light_combine(fattn, fdiff, fspec, uni_lightambient,
		uni_lightdiffuse, uni_lightspecular * uni_materialspecular);
	gl_FragColor = uni_materialdiffuse * diffuse * light;
	gl_FragColor.a = diffuse.a;
}]]}
