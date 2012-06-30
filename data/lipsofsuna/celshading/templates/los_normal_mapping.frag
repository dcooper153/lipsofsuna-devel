vec3 los_normal_mapping(in vec3 normal, in vec3 tangent, in vec4 sample)
{
	vec3 nml1 = normalize(normal);
	if(length(tangent) < 0.01) return nml1;
	vec3 tan1 = normalize(tangent);
	if(abs(dot(nml1, tan1)) > 0.9) return nml1;
	mat3 tangentspace = mat3(tan1, cross(tan1, nml1), nml1);
	vec3 n = tangentspace * (sample.xyz * 2.0 - vec3(1.0));
	if(length(n) < 0.01) return nml1;
	return normalize(n);
}
