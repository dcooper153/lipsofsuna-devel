#version 120

uniform sampler2D LOS_diffuse_texture_0;
uniform sampler2D LOS_diffuse_texture_1;
uniform vec4 LOS_pixel_size;
uniform float LOS_near_distance;
uniform float LOS_far_distance;

varying vec2 F_uv;

void main()
{
	vec2 p = LOS_pixel_size.xy;
	float d00 = texture2D(LOS_diffuse_texture_1, F_uv + vec2(-1.0, -1.0) * p).x;
	float d10 = texture2D(LOS_diffuse_texture_1, F_uv + vec2(0.0, -1.0) * p).x;
	float d20 = texture2D(LOS_diffuse_texture_1, F_uv + vec2(1.0, -1.0) * p).x;
	float d01 = texture2D(LOS_diffuse_texture_1, F_uv + vec2(-1.0, 0.0) * p).x;
	float d11 = texture2D(LOS_diffuse_texture_1, F_uv).x;
	float d21 = texture2D(LOS_diffuse_texture_1, F_uv + vec2(1.0, 0.0) * p).x;
	float d02 = texture2D(LOS_diffuse_texture_1, F_uv + vec2(-1.0, 1.0) * p).x;
	float d12 = texture2D(LOS_diffuse_texture_1, F_uv + vec2(0.0, 1.0) * p).x;
	float d22 = texture2D(LOS_diffuse_texture_1, F_uv + vec2(1.0, 1.0) * p).x;
	float pass = min(1.0, d11 + 0.15 * (d00 + d10 + d20 + d01 + d21 + d02 + d12 + d22));
	vec4 color = texture2D(LOS_diffuse_texture_0, F_uv);
	gl_FragColor = color * pass;
}
