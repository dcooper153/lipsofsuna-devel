vec3 los_cel_shading(in vec4 material, in vec4 diff, in vec4 spec, in sampler2D t1)
{
	float diff_v = dot(diff.rgb, vec3(0.3, 0.59, 0.11));
	float spec_v = dot(spec.rgb, vec3(0.3, 0.59, 0.11));
	float cel_v = 2.0 * texture2D(t1, vec2(diff_v, spec_v)).r;
	vec3 lit = max(vec3(0.001), (diff + spec).rgb);
	float lit_v = max(0.00001, dot(lit, vec3(0.3, 0.59, 0.11)));
	return material.rgb * lit * (cel_v / lit_v);
}
