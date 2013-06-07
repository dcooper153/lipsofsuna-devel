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
	if(c < 0.00001)
		return vec3(0.0, 0.0, v);
	float h = 0.0;
	if(v == rgb.r) h = (mod((rgb.g - rgb.b) / c, 6.0)) / 6.0;
	else if(v == rgb.g) h = ((rgb.b - rgb.r) / c + 2.0) / 6.0;
	else if(v == rgb.b) h = ((rgb.r - rgb.g) / c + 4.0) / 6.0;
	return vec3(h, c / v, v);
}
vec3 los_cel_shading_skin(in vec4 material, in vec4 diff, in vec4 spec, in vec4 p, in sampler1D t1, in sampler1D t2)
{
	vec3 diff_hsv = los_rgb_to_hsv(diff.rgb);
	vec3 spec_hsv = los_rgb_to_hsv(spec.rgb);
	float diff_f = texture1D(t1, p.x * diff_hsv.b).x;
	float spec_f = texture1D(t2, p.y * spec_hsv.b).x;
	vec3 hsv = vec3(diff_hsv.rg + spec_hsv.rg, diff_f + spec_f);
	hsv.g = mix(1.0, hsv.g, diff_f); /* Adds more saturation for shadows. */
	return material.rgb * los_hsv_to_rgb(hsv);
}
