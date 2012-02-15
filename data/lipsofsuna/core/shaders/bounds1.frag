#version 120

uniform vec4 LOS_material_diffuse;

void main()
{
	gl_FragColor = LOS_material_diffuse;
}
