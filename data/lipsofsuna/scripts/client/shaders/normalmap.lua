Shader{
	name = "deferred-normalmap",
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
uniform uni_diffusetexture DIFFUSETEXTURE0
uniform uni_normaltexture DIFFUSETEXTURE1]],

	vertex = [[
#version 150
in vec3 att_coord;
in vec3 att_normal;
in vec2 att_texcoord;
out geomvar
{
	vec3 coord;
	vec3 normal;
	vec2 texcoord;
} OUT;
uniform mat4 uni_matrixmodelview;
uniform mat3 uni_matrixnormal;
uniform mat4 uni_matrixprojection;
void main()
{
	vec4 tmp = uni_matrixmodelview * vec4(att_coord,1.0);
	OUT.coord = tmp.xyz;
	OUT.normal = uni_matrixnormal * att_normal;
	OUT.texcoord = att_texcoord;
	gl_Position = uni_matrixprojection * tmp;
}]],

	geometry = [[
#version 150
#extension GL_EXT_geometry_shader4 : enable
layout(triangles) in;
layout(triangle_strip, max_vertices=3) out;
in geomvar
{
	vec3 coord;
	vec3 normal;
	vec2 texcoord;
} IN[3];
out fragvar
{
	vec3 coord;
	vec3 normal;
	vec3 tangent;
	vec2 texcoord;
} OUT;
uniform mat3 uni_matrixnormal;]]
.. Shader.los_triangle_tangent .. [[
void main()
{
	int i;
	OUT.tangent = uni_matrixnormal * los_triangle_tangent(
		IN[0].coord, IN[1].coord, IN[2].coord,
		IN[0].texcoord, IN[1].texcoord, IN[2].texcoord);
	for(i = 0 ; i < gl_VerticesIn ; i++)
	{
		OUT.coord = IN[i].coord;
		OUT.normal = IN[i].normal;
		OUT.texcoord = IN[i].texcoord;
		gl_Position = gl_PositionIn[i];
		EmitVertex();
	}
	EndPrimitive();
}]],

	fragment = [[
#version 150
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
uniform sampler2D uni_diffusetexture;
uniform sampler2D uni_normaltexture;
]] .. Shader.los_normal_mapping .. [[
void main()
{
	vec3 tangent = normalize(IN.tangent);
	vec3 normal = los_normal_mapping(IN.normal, tangent, IN.texcoord, uni_normaltexture);
	vec4 diffuse = texture(uni_diffusetexture, IN.texcoord);
	/* Diffuse. */
	gl_FragData[0] = uni_materialdiffuse * diffuse;
	/* Specular. */
	gl_FragData[1].rgb = uni_materialspecular.xyz * uni_materialspecular.a;
	gl_FragData[1].a = uni_materialshininess / 128.0;
	/* Normal. */
	gl_FragData[2].xyz = 0.5 * normal + vec3(0.5);
}]]}
