#version 120
#extension GL_ARB_draw_buffers : enable

uniform sampler2D LOS_diffuse_texture_0;
uniform sampler2D LOS_shadow_texture_0;
uniform vec4 LOS_scene_ambient;
uniform vec4 LOS_material_diffuse;
uniform float LOS_material_shininess;
uniform vec3 LOS_light_direction[LIGHTS];
uniform vec4 LOS_light_diffuse[LIGHTS];
uniform vec4 LOS_light_specular[LIGHTS];
uniform vec4 LOS_light_equation[LIGHTS];
uniform vec4 LOS_light_spotparams[LIGHTS];

varying vec3 F_coord;
varying vec3 F_normal;
varying vec2 F_texatlas;
varying vec2 F_texcoord;
varying vec3 F_splatting;
varying vec3 F_eyev;
varying vec4 F_shadow;
varying vec3 F_lightv[LIGHTS];

vec3 los_blinn_phong(in vec3 lv, in vec3 ev, in vec3 ld, in vec4 eq, in vec3 normal, in float shininess);

void main()
{
	/* Texture atlas fitting. */
	vec2 uv_atlas = F_texatlas;
	vec2 uv = fract(F_texcoord) * 0.125;

	/* Texture splatting.
	 *
	 * F_splatting contains the splatting multiplier of the fragment
	 * in X and the coordinate in the quad in YZ.
	 *
	 * m +           #   The influence of splatting increases as the
	 * a |         ###   distance of the fragment from the center of
	 * g |        ####   the quad increases.
	 * n +-----+######
	 *  d i s t a n c e
	 */
	float dist = length(F_splatting.yz - vec2(0.5));
	float splat = F_splatting.x * smoothstep(0.0, 0.5, dist);
	vec4 diffuse0 = texture2D(LOS_diffuse_texture_0, uv_atlas + uv);
	vec4 diffuse1 = texture2D(LOS_diffuse_texture_0, uv);
	vec4 diffuse = mix(diffuse0, diffuse1, splat);

	/* Lighting. */
	vec3 normal = normalize(F_normal);
	vec4 diff = LOS_scene_ambient;
	for(int i = 0 ; i < LIGHTS ; i++)
	{
		vec3 l = los_blinn_phong(F_lightv[i], F_eyev, LOS_light_direction[i],
			LOS_light_equation[i], normal, LOS_material_shininess);
		if (i == 0)
			l *= texture2DProj(LOS_shadow_texture_0, F_shadow).x;
		diff += l.z * l.x * LOS_light_diffuse[i];
	}
	vec3 color = (LOS_material_diffuse * diffuse).rgb * diff.rgb;
#ifdef ENABLE_MRT
	gl_FragData[0] = vec4(color, diffuse.a);
	gl_FragData[1] = vec4(gl_FragCoord.z);
#else
	gl_FragColor = vec4(color, diffuse.a);
#endif
}
