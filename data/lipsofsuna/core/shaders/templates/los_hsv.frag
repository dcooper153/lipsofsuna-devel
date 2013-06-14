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
