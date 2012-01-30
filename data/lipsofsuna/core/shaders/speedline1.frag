#version 130

uniform vec4 LOS_material_diffuse;

in float F_alpha;

out vec4 colour;

void main()
{
	colour = vec4(LOS_material_diffuse.rgb, F_alpha);
}
