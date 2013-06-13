#version 120

varying vec2 F_texcoord;

void main()
{
	gl_FragColor = vec4(gl_FragCoord.z);
}
