Shader{
name = "luminous",
pass3_depth_func = "lequal",
pass3_fragment = [[
void main()
{
	gl_FragColor = vec4(1.3, 1.3, 1.3, 1.0);
}]]}

