#version 120

uniform sampler2D SourceTexture;

uniform vec4 HmdWarpParam;
uniform vec4 ViewPortSize;
uniform vec4 ViewPortOffset;

uniform vec2 Prescale;
uniform vec2 Postscale;


/* Scales input coordinates for distortion.*/
vec2 HmdWarp(vec2 theta)
{
	float theta_square = theta.x * theta.x + theta.y * theta.y;
	vec2 warp_vector = theta * (
		HmdWarpParam.x + HmdWarpParam.y * theta_square +
		HmdWarpParam.z * theta_square * theta_square +
		HmdWarpParam.w * theta_square * theta_square * theta_square);
	return warp_vector;
}

void main()
{
	vec2 TexCoord = vec2(
		gl_FragCoord.x * ViewPortSize.z,
		0.5 + (gl_FragCoord.y - ViewPortSize.y * 0.5) * ViewPortSize.z);
	vec2 LensCenter = vec2(floor(TexCoord.x) + 0.5, 0.5);

	vec2 tc = HmdWarp((TexCoord - LensCenter) * Prescale) * Postscale + LensCenter;
	tc.x = tc.x - floor(TexCoord.x);
	tc.y = 1 - tc.y;

	gl_FragColor = texture2D(SourceTexture, tc);
}

