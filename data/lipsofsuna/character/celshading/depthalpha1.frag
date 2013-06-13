#version 120

uniform sampler2D LOS_diffuse_texture_0;

varying vec2 F_texcoord;

void main()
{
	vec4 diffuse = texture2D(LOS_diffuse_texture_0, F_texcoord);
	if (diffuse.a < 0.5)
		discard;
	gl_FragColor = vec4(gl_FragCoord.z);
}
