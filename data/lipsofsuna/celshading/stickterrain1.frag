#version 120

uniform sampler2D LOS_diffuse_texture_1;
uniform sampler1D LOS_diffuse_texture_2;
uniform sampler1D LOS_diffuse_texture_3;
uniform vec4 LOS_scene_ambient;
uniform vec4 LOS_material_diffuse;
uniform vec4 LOS_material_celshading;
uniform float LOS_material_shininess;
uniform vec3 LOS_light_direction[LIGHTS];
uniform vec4 LOS_light_diffuse[LIGHTS];
uniform vec4 LOS_light_specular[LIGHTS];
uniform vec4 LOS_light_equation[LIGHTS];
uniform vec4 LOS_light_spotparams[LIGHTS];

varying vec3 F_coord;
varying vec3 F_normal;
varying vec2 F_texcoord;
varying vec3 F_eyev;
varying vec3 F_lightv[LIGHTS];

vec3 los_blinn_phong(in vec3 lv, in vec3 ev, in vec3 ld, in vec4 eq, in vec3 normal, in float shininess);
vec3 los_cel_shading(in vec4 material, in vec4 diff, in vec4 spec, in vec4 p, in sampler1D t1, in sampler1D t2);

void main()
{
	/* Texcoord generation. */
	vec2 uv_world;
	if(F_texcoord.y < 0.25)
		uv_world = F_coord.yz;
	else if(F_texcoord.y < 0.65)
		uv_world = F_coord.xy;
	else
		uv_world = F_coord.xz;
	int material = int(F_texcoord.x * 255.0 + 0.5);
	vec2 uv_orig = vec2(mod(material, 4), float(material / 4)) * 0.25;
	vec2 uv = uv_orig + mod(uv_world * 0.2, 1.0) * 0.248 + vec2(0.001);
	/* Lighting. */
	vec3 normal = normalize(F_normal);
	vec4 diffuse = texture2D(LOS_diffuse_texture_1, uv);
	vec4 diff = LOS_scene_ambient;
	vec4 spec = vec4(0.0);
	for(int i = 0 ; i < LIGHTS ; i++)
	{
		vec3 l = los_blinn_phong(F_lightv[i], F_eyev, LOS_light_direction[i],
			LOS_light_equation[i], normal, LOS_material_shininess);
		diff += l.z * l.x * LOS_light_diffuse[i];
		spec += l.z * l.y * LOS_light_specular[i];
	}
	vec3 color = los_cel_shading(LOS_material_diffuse * diffuse, diff, spec,
		LOS_material_celshading, LOS_diffuse_texture_2, LOS_diffuse_texture_3);
	gl_FragColor = vec4(color, diffuse.a);
}
