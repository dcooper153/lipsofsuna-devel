#version 120

uniform sampler2D LOS_diffuse_texture_0;
uniform sampler2D LOS_diffuse_texture_1;
uniform sampler2D LOS_cel_texture;
uniform vec4 LOS_scene_ambient;
uniform vec4 LOS_material_diffuse;
uniform float LOS_material_shininess;
uniform vec3 LOS_light_direction[LIGHTS];
uniform vec4 LOS_light_diffuse[LIGHTS];
uniform vec4 LOS_light_specular[LIGHTS];
uniform vec4 LOS_light_equation[LIGHTS];
uniform vec4 LOS_light_spotparams[LIGHTS];

varying vec3 F_normal;
varying vec2 F_texcoord;
varying vec3 F_eyev;
varying vec3 F_lightv[LIGHTS];

vec3 los_blinn_phong(in vec3 lv, in vec3 ev, in vec3 ld, in vec4 eq, in vec3 normal, in float shininess);
vec3 los_cel_shading(in vec4 material, in vec4 diff, in vec4 spec, in sampler2D t1);

void main()
{
	vec3 normal = normalize(F_normal);
	vec4 diffuse = texture2D(LOS_diffuse_texture_0, F_texcoord);
	vec4 diff = LOS_scene_ambient;
	vec4 spec = vec4(0.0);
	for(int i = 0 ; i < LIGHTS ; i++)
	{
		vec3 l = los_blinn_phong(F_lightv[i], F_eyev, LOS_light_direction[i],
			LOS_light_equation[i], normal, LOS_material_shininess);
		diff += l.z * l.x * LOS_light_diffuse[i];
		spec += l.z * l.y * LOS_light_specular[i];
	}
	vec3 color = los_cel_shading(LOS_material_diffuse * diffuse, diff, spec, LOS_cel_texture);
#ifdef ENABLE_MRT
	gl_FragData[0] = vec4(color, diffuse.a);
	gl_FragData[1] = vec4(gl_FragCoord.z);
#else
	gl_FragColor = vec4(color, diffuse.a);
#endif
}
