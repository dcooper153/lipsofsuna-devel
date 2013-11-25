#version 120

uniform sampler2D LOS_diffuse_texture_0;
uniform vec3 LOS_camera_position;
uniform vec4 LOS_viewport_size;

void main()
{
	float scale = 50.0 / length(LOS_camera_position);
	vec2 pix = scale * floor(gl_FragCoord.xy / scale);
	vec2 uv1 = pix.xy * LOS_viewport_size.zw;
	vec2 uv2 = vec2(uv1.x, 1.0 - uv1.y);
	vec4 diffuse = texture2D(LOS_diffuse_texture_0, uv2);
	gl_FragColor = diffuse;
}
