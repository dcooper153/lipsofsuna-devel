vec3 los_cel_shading_skin(in vec4 material, in vec4 diff, in vec4 spec, in sampler2D t1)
{
	float diff_v = dot(diff.rgb, vec3(0.3, 0.59, 0.11));
	float spec_v = dot(spec.rgb, vec3(0.3, 0.59, 0.11));
	float cel_v = 2.0 * texture2D(t1, vec2(diff_v, spec_v)).r;
	vec3 lit = (diff + spec).rgb;
	float lit_v = max(0.00001, dot(lit, vec3(0.3, 0.59, 0.11)));
	vec3 mat = material.rgb * lit * (cel_v / lit_v);
	float mat_v = max(max(mat.r, mat.g), mat.b);
	float mat_m = min(min(mat.r, mat.g), mat.b);
	float mat_s = 1.0 - mat_m / mat_v;
	float mult = min(1.0 / mat_s, mix(2.0, 1.0, cel_v));
	return max(vec3(0.0), vec3(mat_v) + (mat - vec3(mat_v)) * mult);
}
