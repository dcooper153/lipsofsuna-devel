Shader{
name = "deferred",
config = [[
light-count 1
uniform uni_lightambient LIGHTAMBIENT0
uniform uni_lightdiffuse LIGHTDIFFUSE0
uniform uni_lightequation LIGHTEQUATION0
uniform uni_lightposition LIGHTPOSITIONPREMULT0
uniform uni_lightspecular LIGHTSPECULAR0
uniform uni_matrixprojectioninverse MATRIXPROJECTIONINVERSE
uniform uni_texturemap[0] DIFFUSETEXTURE0
uniform uni_texturemap[1] DIFFUSETEXTURE1
uniform uni_texturemap[2] DIFFUSETEXTURE2
uniform uni_depthmap DIFFUSETEXTURE3]],

vertex = [[
out vec2 var_texcoord;
void main()
{
	var_texcoord = LOS_texcoord;
	gl_Position = vec4(LOS_coord, 1.0);
}]],

fragment = [[
in vec2 var_texcoord;
uniform mat4 uni_matrixprojectioninverse;
uniform vec4 uni_lightambient;
uniform vec4 uni_lightdiffuse;
uniform vec3 uni_lightequation;
uniform vec3 uni_lightposition;
uniform vec4 uni_lightspecular;
uniform sampler2D uni_depthmap;
uniform sampler2D uni_texturemap[3];]]
.. Shader.los_light_attenuation
.. Shader.los_light_combine
.. Shader.los_light_diffuse
.. Shader.los_light_specular .. [[
vec3 unpackcoord()
{
	float d = texture(uni_depthmap, var_texcoord).r * 2.0 - 1.0;
	vec2 f = var_texcoord * 2.0 - vec2(1.0);
	vec4 v = uni_matrixprojectioninverse * vec4(f.xy, d, 1.0);
	return v.xyz / v.w;
}
vec3 unpacknormal()
{
	return normalize(texture(uni_texturemap[2], var_texcoord).xyz * 2.0 - vec3(1.0));
}
vec4 unpackspecular()
{
	vec4 spec = texture(uni_texturemap[1], var_texcoord);
	spec.a *= 128.0;
	return spec;
}
void main()
{
	vec4 diffuse = texture(uni_texturemap[0], var_texcoord);
	vec3 coord = unpackcoord();
	vec3 normal = unpacknormal();
	vec4 spec = unpackspecular();
	vec3 lightvector = uni_lightposition - coord;
	float fattn = los_light_attenuation(lightvector, uni_lightequation);
	float fdiff = los_light_diffuse(lightvector, normal);
	float fspec = los_light_specular(lightvector, normal, spec.a);
	vec4 light = los_light_combine(fattn, fdiff, fspec, uni_lightambient,
		uni_lightdiffuse, uni_lightspecular * vec4(spec.rgb, 1.0));
	gl_FragColor = diffuse * light;
}]]}

------------------------------------------------------------------------------

Shader{
name = "deferred-spotlight",
config = [[
light-count 1
uniform uni_lightambient LIGHTAMBIENT0
uniform uni_lightdiffuse LIGHTDIFFUSE0
uniform uni_lightdirection LIGHTDIRECTIONPREMULT0
uniform uni_lightequation LIGHTEQUATION0
uniform uni_lightmatrix LIGHTMATRIX0
uniform uni_lightposition LIGHTPOSITIONPREMULT0
uniform uni_lightspecular LIGHTSPECULAR0
uniform uni_lightspot LIGHTSPOT0
uniform uni_matrixprojectioninverse MATRIXPROJECTIONINVERSE
uniform uni_texturemap[0] DIFFUSETEXTURE0
uniform uni_texturemap[1] DIFFUSETEXTURE1
uniform uni_texturemap[2] DIFFUSETEXTURE2
uniform uni_depthmap DIFFUSETEXTURE3
uniform uni_shadowmap SHADOWTEXTURE0]],

vertex = [[
out vec2 var_texcoord;
void main()
{
	var_texcoord = LOS_texcoord;
	gl_Position = vec4(LOS_coord, 1.0);
}]],

fragment = [[
in vec2 var_texcoord;
uniform mat4 uni_matrixprojectioninverse;
uniform vec4 uni_lightambient;
uniform vec4 uni_lightdiffuse;
uniform vec3 uni_lightdirection;
uniform vec3 uni_lightequation;
uniform mat4 uni_lightmatrix;
uniform vec3 uni_lightposition;
uniform vec4 uni_lightspecular;
uniform vec3 uni_lightspot;
uniform sampler2D uni_depthmap;
uniform sampler2D uni_texturemap[3];
uniform sampler2DShadow uni_shadowmap;]]
.. Shader.los_light_attenuation
.. Shader.los_light_combine
.. Shader.los_light_diffuse
.. Shader.los_light_specular
.. Shader.los_light_spot
.. Shader.los_shadow_mapping
.. Shader.los_shadow_mapping_pcf .. [[
vec3 unpackcoord()
{
	float d = texture(uni_depthmap, var_texcoord).r * 2.0 - 1.0;
	vec2 f = var_texcoord * 2.0 - vec2(1.0);
	vec4 v = uni_matrixprojectioninverse * vec4(f.xy, d, 1.0);
	return v.xyz / v.w;
}
vec3 unpacknormal()
{
	return normalize(texture(uni_texturemap[2], var_texcoord).xyz * 2.0 - vec3(1.0));
}
vec4 unpackspecular()
{
	vec4 spec = texture(uni_texturemap[1], var_texcoord);
	spec.a *= 128.0;
	return spec;
}
void main()
{
	vec4 diffuse = texture(uni_texturemap[0], var_texcoord);
	vec3 coord = unpackcoord();
	vec3 normal = unpacknormal();
	vec4 spec = unpackspecular();
	vec3 lightvector = uni_lightposition - coord;
	float fshad = los_shadow_mapping_pcf(uni_lightmatrix * vec4(coord, 1.0), uni_shadowmap);
	float fattn = los_light_attenuation(lightvector, uni_lightequation);
	float fdiff = los_light_diffuse(lightvector, normal);
	float fspot = los_light_spot(lightvector, uni_lightdirection, uni_lightspot);
	float fspec = los_light_specular(lightvector, normal, spec.a);
	vec4 light = los_light_combine(fattn, fspot * fdiff * fshad, fspec, uni_lightambient,
		uni_lightdiffuse, uni_lightspecular * vec4(spec.rgb, 1.0));
	gl_FragColor = diffuse * light;
}]]}
