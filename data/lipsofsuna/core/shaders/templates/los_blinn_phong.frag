vec3 los_blinn_phong(in vec3 lv, in vec3 ev, in vec3 ld, in vec4 eq, in vec3 normal, in float shininess)
{
	float dist = length(lv);
	float attinv = dot(eq.yzw, vec3(1.0, dist, dist * dist));
	if(dist < 0.01 || attinv < 0.01)
		return vec3(0.0);
	vec3 lvn = normalize(lv);
	float diff = dot(normal, lvn);
	if(diff <= 0.0)
		return vec3(0, 0.0, 1.0 / attinv);
	vec3 hv = normalize(lv) + normalize(ev);
	float ndh = dot(normal, normalize(hv));
	float spec = pow(max(0.0, ndh), shininess);
	return vec3(diff, spec, 1.0 / attinv);
}
