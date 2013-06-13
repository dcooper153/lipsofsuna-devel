#version 120

uniform sampler2D LOS_diffuse_texture_0;

varying vec2 F_texcoord;

void main()
{
	vec4 diffuse = texture2D(LOS_diffuse_texture_0, F_texcoord);
#ifdef ENABLE_MRT
	gl_FragData[0] = diffuse;
	gl_FragData[1] = gl_FragCoord;
#else
	gl_FragColor = diffuse;
#endif
}
