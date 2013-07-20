#version 120
#extension GL_ARB_draw_buffers : enable

uniform sampler2D LOS_diffuse_texture_0;

varying vec3 F_coord;
varying vec2 F_texatlas;
varying vec2 F_texcoord;

void main()
{
	/* Texture atlas fitting. */
	vec2 uv_atlas = F_texatlas;
	vec2 uv = mod(F_texcoord, 1.0) * 0.234;

	vec4 diffuse = texture2D(LOS_diffuse_texture_0, uv_atlas + uv);
	if (diffuse.a < 0.5)
		discard;
	gl_FragColor = vec4(gl_FragCoord.z);
}
