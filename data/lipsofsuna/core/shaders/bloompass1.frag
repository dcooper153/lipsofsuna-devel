//-------------------------------
//BrightBloom_ps20.glsl
// High-pass filter for obtaining lumminance
// We use an aproximation formula that is pretty fast:
//   f(x) = ( -3 * ( x - 1 )^2 + 1 ) * 2
//   Color += Grayscale( f(Color) ) + 0.6
//
// Special thanks to ATI for their great HLSL2GLSL utility
//     http://sourceforge.net/projects/hlsl2glsl
//-------------------------------

uniform sampler2D RT;

in vec2 F_uv;

void main()
{
	vec4 tex = texture(RT, F_uv) - vec4(1.0);
	vec4 bright4 = -6.0 * tex * tex + vec4(2.0);
	float bright = dot(bright4, vec4(0.333333, 0.333333, 0.333333, 0.000000));
	gl_FragColor = tex + vec4(bright + 0.6);
}
