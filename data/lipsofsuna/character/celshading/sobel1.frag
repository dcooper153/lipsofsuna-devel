#version 120

uniform sampler2D LOS_diffuse_texture_0;
uniform sampler2D LOS_diffuse_texture_1;
uniform vec4 LOS_pixel_size;
uniform float LOS_near_distance;
uniform float LOS_far_distance;

varying vec2 F_uv;

float mapz(vec4 tex)
{
	float fn = LOS_far_distance / LOS_near_distance;
	return 2.0 / ((1.0 + fn) + (1.0 - fn) * tex.x);
}

void main()
{
	/* Sobel edge detection
	 *
	 *     |-A  0  A|      |-A -B -A|
	 * x = |-B  0  B|, y = | 0  0  0|, where A=3 and B=10
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
	float x = 3.0*(-d00 -d02 +d20 + d22) + 10.0*(-d01 +d21);
	float y = 3.0*(-d00 -d20 +d02 + d22) + 10.0*(-d10 +d12);
	float e = sqrt(x * x + y * y);

	/* Edge ramp function.
	 *
	 * The depth discontinuity required by the outline increases linearly
	 * as the function of depth. This alone seems to give decent results in
	 * landscape scenes if the depth multiplier is calibrated well.
	 * 
	 * The value subtracted from the refrence distance is used for giving
	 * very close objects more self-outlines. This makes characters look
	 * better while not causing lots of artifacts if the value is just
	 * large enough but still very small.
	 */
	float ramp = 0.005 + 0.7 * max(0.0, d11 - 0.01);
	gl_FragColor = vec4(step(e, ramp));
}
