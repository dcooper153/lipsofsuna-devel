uniform sampler2D LOS_diffuse_texture_0;

varying vec2 F_texcoord;
varying vec2 F_depth;

void main()
{
	vec4 diffuse = texture2D(LOS_diffuse_texture_0, F_texcoord);
	/*float depth = F_depth.x / F_depth.y;
	gl_FragColor = vec4(depth, depth, depth, 1.0);*/
	gl_FragColor = vec4(0.0, 0.0, 0.0, diffuse.a);
}
