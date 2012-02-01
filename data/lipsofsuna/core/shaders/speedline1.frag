#version 120

uniform vec4 LOS_material_diffuse;

varying float F_alpha;

void main()
{
	gl_FragColor = vec4(LOS_material_diffuse.rgb, F_alpha);
}
