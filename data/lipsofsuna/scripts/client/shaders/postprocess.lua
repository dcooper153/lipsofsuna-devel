Bloom = Class()
Bloom.exposure = 1.5  -- The intensity of the bloom halo.
Bloom.luminance = 1   -- The smallest luminance value that gets bloomed.
Bloom.radius = 20     -- The width of the bloom halo.
Bloom.shape = 4       -- The shape of the intensity curve of the bloom filter.

--- Initializes the bloom filter.
-- @param clss Bloom class.
Bloom.init = function(clss)
	clss:compile_filter_kernel()
	clss.shader_horz = Shader{name = "postprocess-horz-hdr"}
	clss.shader_vert = Shader{name = "postprocess-vert-hdr"}
	clss:compile()
end

--- Recompiles the bloom filters.
-- @param clss Bloom class.
Bloom.compile = function(clss)
	clss:compile_filter_kernel()
	clss.shader_horz:compile{
		pass1_depth_test = false,
		pass1_depth_write = false,
		pass1_vertex = clss.code_vertex,
		pass1_fragment = clss:code_fragment(true)}
	clss.shader_vert:compile{
		pass1_depth_test = false,
		pass1_depth_write = false,
		pass1_vertex = clss.code_vertex,
		pass1_fragment = clss:code_fragment(false)}
end

--- Recompiles the kernel of the bloom filters.
-- @param clss Bloom class.
Bloom.compile_filter_kernel = function(clss)
	-- Create the kernel by sampling the normalized gaussian curve.
	local s = clss.shape / clss.radius
	clss.kernel = {}
	for i = -clss.radius,clss.radius do
		local gauss = 1 / math.sqrt(2 * math.pi) * math.exp(-0.5 * math.pow(s * i, 2))
		table.insert(clss.kernel, gauss)
	end
	-- Calculate the sum of the kernel in two dimensions.
	local total = 0.0
	for i = 1,#clss.kernel do
		for j = 1,#clss.kernel do
			total = total + clss.kernel[i] * clss.kernel[j]
		end
	end
	total = math.sqrt(total)
	-- Normalize the kernel.
	for i = 1,#clss.kernel do
		clss.kernel[i] = clss.kernel[i] / total
	end
	-- Build the shader string.
	clss.kernel_string = "float[]("
	for i = 1,#clss.kernel do
		local v = string.format("%.4f", clss.kernel[i])
		clss.kernel_string = clss.kernel_string .. v .. (i ~= #clss.kernel and "," or ")")
	end
end

------------------------------------------------------------------------------

Bloom.code_vertex = [[
out vec2 var_texcoord;
void main()
{
	var_texcoord = LOS_texcoord;
	gl_Position = vec4(LOS_coord, 1.0);
}]]

Bloom.code_fragment = function(clss, horz)
return [[
in vec2 var_texcoord;
const float bloom_exposure = ]] .. clss.exposure .. [[;
const float bloom_luminance = ]] .. clss.luminance .. [[;
vec4 los_postproc_bloom(vec2 dt)
{
	int i;
	const int r = ]] .. ((#clss.kernel - 1) / 2) .. [[;
	const int w = ]] .. #clss.kernel .. [[;
	const float kernel[w] = ]] .. clss.kernel_string .. [[;
	vec3 bloom = vec3(0.0);
	for(i = 0 ; i < w ; i++)
	{
		vec3 sample = texture2D(LOS_diffuse_texture_0, var_texcoord + float(i - r) * dt).rgb;
		float luminance = dot(sample, vec3(0.3, 0.59, 0.11));
		float bloomed = max(0.0, luminance - bloom_luminance);
		float exposure = 1.0 - exp(-bloom_exposure * bloomed);
		bloom += sample * (kernel[i] * exposure);
	}
	return vec4(bloom, 1.0);
}
void main()
{
	vec4 bloom = los_postproc_bloom(LOS_material_param_0.]] .. (horz and "xw" or "wy") .. [[);
	vec4 color = texture(LOS_diffuse_texture_0, var_texcoord);
	gl_FragColor = color + bloom;
}]]
end

------------------------------------------------------------------------------

Bloom:init()
