#version 120

uniform sampler2D LOS_diffuse_texture_0;
uniform sampler2D LOS_diffuse_texture_1;
uniform vec4 LOS_pixel_size;
uniform float LOS_near_distance;
uniform float LOS_far_distance;

varying vec2 F_uv;

float mapz(vec4 tex)
{
	return tex.z;
}

void main()
{
	/* Sobel edge detection
	 *
	 *     |-A  0  A|      |-A -B -A|
	 * x = |-B  0  B|, y = | 0  0  0|
	 *     |-A  0  A|      | A  B  A|
	 *
	 * e = sqrt(x^2 + y^2)
	 */
	vec2 p = LOS_pixel_size.xy;
	float d00 = mapz(texture2D(LOS_diffuse_texture_1, F_uv + vec2(-1.0, -1.0) * p));
	float d10 = mapz(texture2D(LOS_diffuse_texture_1, F_uv + vec2(0.0, -1.0) * p));
	float d20 = mapz(texture2D(LOS_diffuse_texture_1, F_uv + vec2(1.0, -1.0) * p));
	float d01 = mapz(texture2D(LOS_diffuse_texture_1, F_uv + vec2(-1.0, 0.0) * p));
	float d11 = mapz(texture2D(LOS_diffuse_texture_1, F_uv));
	float d21 = mapz(texture2D(LOS_diffuse_texture_1, F_uv + vec2(1.0, 0.0) * p));
	float d02 = mapz(texture2D(LOS_diffuse_texture_1, F_uv + vec2(-1.0, 1.0) * p));
	float d12 = mapz(texture2D(LOS_diffuse_texture_1, F_uv + vec2(0.0, 1.0) * p));
	float d22 = mapz(texture2D(LOS_diffuse_texture_1, F_uv + vec2(1.0, 1.0) * p));
	float a = 3.0;
	float b = 10.0;
	float x = a*(-d00 -d02 +d20 + d22) + b*(-d01 +d21);
	float y = a*(-d00 -d20 +d02 + d22) + b*(-d10 +d12);
	float e = sqrt(x * x + y * y);

	/* Edge ramp function.
	 *
	 * Linear ramp is used close to the near plane.
	 * Derivative-based ramp is used further away.
	 */
	float fn = LOS_far_distance / LOS_near_distance;
	float linear = 2.0 / ((1.0 + fn) + (1.0 - fn) * d11);
	float deriv = 2.0 / (1.0 - fn) * log(linear);
	float ramp = max(0.00002 / deriv, 0.0004 / linear);
	vec4 color = texture2D(LOS_diffuse_texture_0, F_uv);
	gl_FragColor = mix(color, vec4(0.0), step(ramp, e));
}
