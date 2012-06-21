#version 120

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

varying vec3 F_normal;
varying vec2 F_texcoord;
varying vec3 F_eyev;
varying vec3 F_lightv[LIGHTS];

vec3 los_blinn_phong(in vec3 lv, in vec3 ev, in vec3 ld, in vec4 eq, in vec3 normal, in float shininess)
{
	float dist = length(lv);
	float attinv = dot(eq.yzw, vec3(1.0, dist, dist * dist));
	if(dist < 0.01 || attinv < 0.01)
		return vec3(0.0);
	vec3 lvn = normalize(lv);
	float diff = dot(normal, lvn);
	if(diff <= 0.0)
		return vec3(0.0);
	vec3 hv = normalize(lv) + normalize(ev);
	float ndh = dot(normal, normalize(hv));
	float spec = pow(max(0.0, ndh), shininess);
	return vec3(diff, spec, 1.0 / attinv);
}
vec3 los_hsv_to_rgb(in vec3 hsv)
{
	float c = hsv.b * hsv.g;
	float l = hsv.b - c;
	float hh = hsv.r * 6.0;
	float x = c * (1.0 - abs(mod(hh, 2.0) - 1.0));
	if(0.0 <= hh && hh < 1.0) return vec3(c + l, x + l, l);
	if(1.0 <= hh && hh < 2.0) return vec3(x + l, c + l, l);
	if(2.0 <= hh && hh < 3.0) return vec3(l, c + l, x + l);
	if(3.0 <= hh && hh < 4.0) return vec3(l, x + l, c + l);
	if(4.0 <= hh && hh < 5.0) return vec3(x + l, l, c + l);
	return vec3(c + l, l, x + l);
}
vec3 los_rgb_to_hsv(in vec3 rgb)
{
	float v = max(max(rgb.r, rgb.g), rgb.b);
	float m = min(min(rgb.r, rgb.g), rgb.b);
	float c = v - m;
	float h;
	if(c < 0.00001) h = 0.0;
	else if(v == rgb.r) h = (mod((rgb.g - rgb.b) / c, 6.0)) / 6.0;
	else if(v == rgb.g) h = ((rgb.b - rgb.r) / c + 2.0) / 6.0;
	else if(v == rgb.b) h = ((rgb.r - rgb.g) / c + 4.0) / 6.0;
	if(c < 0.00001)
		return vec3(h, 0.0, v);
	else
		return vec3(h, c / v, v);
}
vec3 los_cel_shading(in vec4 material, in vec4 diff, in vec4 spec, in vec4 p, in sampler1D t1, in sampler1D t2)
{
	vec3 diff_hsv = los_rgb_to_hsv(diff.rgb);
	vec3 spec_hsv = los_rgb_to_hsv(spec.rgb);
	float diff_f = texture1D(t1, p.x * diff_hsv.b).x;
	float spec_f = texture1D(t2, p.y * spec_hsv.b).x;
	vec3 hsv = vec3(diff_hsv.rg + spec_hsv.rg, diff_f + spec_f);
	return material.rgb * los_hsv_to_rgb(hsv);
}

void main()
{
	vec3 normal = normalize(F_normal);
	vec4 diffuse = vec4(1.0);
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
	gl_FragColor = vec4(color, 0.3 + 0.1 * spec.a);
}
