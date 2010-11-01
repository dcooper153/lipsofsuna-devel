Shader{
	name = "deferred-skin",
	config = [[
uniform uni_materialdiffuse MATERIALDIFFUSE
uniform uni_materialspecular MATERIALSPECULAR
uniform uni_materialshininess MATERIALSHININESS
uniform uni_matrixmodelview MATRIXMODELVIEW
uniform uni_matrixnormal MATRIXNORMAL
uniform uni_matrixprojection MATRIXPROJECTION
uniform uni_normalmap DIFFUSETEXTURE1
uniform uni_texturemap DIFFUSETEXTURE0]],

	vertex = [[
out fragvar
{
	vec3 coord;
	vec3 normal;
	vec3 tangent;
	vec2 texcoord;
} OUT;
uniform mat4 uni_matrixmodelview;
uniform mat3 uni_matrixnormal;
uniform mat4 uni_matrixprojection;
void main()
{
	vec4 tmp = uni_matrixmodelview * vec4(LOS_coord,1.0);
	OUT.coord = tmp.xyz;
	OUT.normal = uni_matrixnormal * LOS_normal;
	OUT.tangent = uni_matrixnormal * LOS_tangent;
	OUT.texcoord = LOS_texcoord;
	gl_Position = uni_matrixprojection * tmp;
}]],

	fragment = [[
in fragvar
{
	vec3 coord;
	vec3 normal;
	vec3 tangent;
	vec2 texcoord;
} IN;
uniform vec4 uni_materialdiffuse;
uniform vec4 uni_materialspecular;
uniform float uni_materialshininess;
uniform sampler2D uni_normalmap;
uniform sampler2D uni_texturemap;]]
.. Shader.los_normal_mapping .. [[
void main()
{
	vec3 tangent = normalize(IN.tangent);
	vec3 normal = los_normal_mapping(IN.normal, tangent, IN.texcoord, uni_normalmap);
	vec4 diffuse = texture(uni_texturemap, IN.texcoord);
	/* Diffuse. */
	float fake = abs(dot(normal, vec3(1.0,0.0,0.0))) + abs(dot(normal, vec3(0.0,1.0,0.0)));
	gl_FragData[0] = mix(uni_materialdiffuse * diffuse, vec4(0.5,0.0,0.0,1.0), 0.3 * fake - 0.15);
	/* Specular. */
	gl_FragData[1].rgb = uni_materialspecular.xyz * uni_materialspecular.a;
	gl_FragData[1].a = uni_materialshininess / 128.0;
	/* Normal. */
	gl_FragData[2].xyz = 0.5 * normal + vec3(0.5);
}]]}
