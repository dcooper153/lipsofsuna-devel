local instfunc = Shader.new
Shader.dict_name = {}

--- Creates a new shader and enables it.<br/>
-- The shader isn't compiled at all when it's created. It's done later in the
-- options screen when the used shader quality is known. Until then we only
-- store the settings for the different quality levels.
-- @param clss Shader class.
-- @param args Arguments.<ul>
--   <li>high: High quality shader.</li>
--   <li>low: Low quality shader.</li>
--   <li>medium: Medium quality shader.</li>
--   <li>name: Shader name.</li></ul>
-- @return New shader.
Shader.new = function(clss, args)
	local self = instfunc(clss, args)
	if self then
		for k,v in pairs(args) do self[k] = v end
		self:enable()
		return self
	end
end

--- Disables the shader.<br/>
-- The shader will be subject to normal garbage collection when disabled.
-- @param self Shader.
Shader.disable = function(self)
	Shader.dict_name[self.name] = nil
end

--- Enables the shader.<br/>
-- The shader will not be subject to garbage collection when enabled.
-- @param self Shader.
Shader.enable = function(self)
	Shader.dict_name[self.name] = self
end

--- Sets the quality level of the shader.<br/>
-- Older graphics cards can't deal with certain effects, such as geometry
-- amplification in the geometry shader, so multiple shader sets are used.
-- This function allows switching the shader quality be recompiling the
-- shader with different settings.
-- \param self Shader.
-- \param value Integer in the range of [1,3].
Shader.set_quality = function(self, value)
	local qualities = {self.low, self.medium, self.high}
	if self.quality ~= value then
		self.quality = value
		-- Try the requested level.
		local q = qualities[value]
		if q then return self:compile(q) end
		-- Fallback to lower setting.
		for i = value,1,-1 do
			local q = qualities[i]
			if q then return self:compile(q) end
		end
		-- Fallback to higher setting.
		for i = value,3 do
			local q = qualities[i]
			if q then return self:compile(q) end
		end
	end
end

Shader.los_light_attenuation = [[
float los_light_attenuation(in vec3 coord, in vec3 equation)
{
	float d = length(coord);
	return 1.0 / (equation.x + equation.y * d + equation.z * d * d);
}]]

Shader.los_light_combine = [[
vec4 los_light_combine(in float fattn, in float fdiff, in float fspec, in vec4 cambi, in vec4 cdiff, in vec4 cspec)
{
	return vec4(fattn * (cambi.rgb + fdiff * cdiff.rgb + fspec * cspec.rgb), 1.0);
}]]

Shader.los_light_diffuse = [[
float los_light_diffuse(in vec3 coord, in vec3 normal)
{
	vec3 dir = normalize(coord);
	float coeff = dot(normal, dir);
	return max(0.0, coeff);
}]]

Shader.los_lighting_default = function(co, no, lv)
	return [[int lighting_index;
	vec4 lighting = vec4(0.0, 0.0, 0.0, 1.0);
	for(lighting_index = 0 ; lighting_index < LOS_LIGHT_MAX ; lighting_index++)
	{
		vec3 lv = ]] .. lv .. [[[lighting_index];
		float fattn = 1.0 / dot(LOS_light[lighting_index].equation, vec3(1.0, length(lv), dot(lv, lv)));
		float fdiff = max(0.0, dot(]] .. no .. [[, normalize(lv)));
		float fspec = pow(max(0.0, dot(]] .. no .. [[, reflect(-normalize(]] .. co .. [[), ]] .. no .. [[))), LOS_material_shininess);
		lighting.rgb += fattn * (LOS_light[lighting_index].ambient.rgb +
			fdiff * LOS_light[lighting_index].diffuse.rgb +
			fspec * LOS_light[lighting_index].specular.rgb * LOS_material_specular.rgb);
	}]]
end

Shader.los_lighting_hair = function(co, no, ta, lv)
	return [[int lighting_index;
	vec4 lighting = vec4(0.0, 0.0, 0.0, 1.0);
	for(lighting_index = 0 ; lighting_index < LOS_LIGHT_MAX ; lighting_index++)
	{
		vec3 lv = ]] .. lv .. [[[lighting_index];
		float fattn = 1.0 / dot(LOS_light[lighting_index].equation, vec3(1.0, length(lv), dot(lv, lv)));
		float fdiff = max(0.0, 0.25 + 0.75 * dot(]] .. no .. [[, normalize(lv)));
		float fspec = pow(max(0.0, dot(]] .. no .. [[, reflect(-normalize(]] .. co .. [[), ]] .. no .. [[))), LOS_material_shininess);
		lighting.rgb += fattn * (LOS_light[lighting_index].ambient.rgb +
			fdiff * LOS_light[lighting_index].diffuse.rgb +
			fspec * LOS_light[lighting_index].specular.rgb * LOS_material_specular.rgb);
	}]]
end

Shader.los_lighting_vectors = function(lv, co)
	return [[int lighting_vindex;
	for(lighting_vindex = 0 ; lighting_vindex < LOS_LIGHT_MAX ; lighting_vindex++)
		]] .. lv .. [[[lighting_vindex] = LOS_light[lighting_vindex].position_premult - ]] .. co .. [[;]]
end

Shader.los_light_specular = [[
float los_light_specular(in vec3 coord, in vec3 normal, in float shininess)
{
	vec3 refl = reflect(-normalize(coord), normal);
	float coeff = max(0.0, dot(normal, refl));
	return pow(coeff, shininess);
}]]

Shader.los_light_spot = [[
float los_light_spot(in vec3 coord, in vec3 spotdir, in vec3 spotcfg)
{
	/* spotcfg = (cutoff, coscutoff, exponent) */
	float coeff = clamp(dot(spotdir, -normalize(coord)), -1.0, 1.0);
	float cutoff = step(spotcfg.y, 0.0);
	float blend = step(spotcfg.y, coeff);
	return blend * max(cutoff, pow(coeff, spotcfg.z));
}]]

Shader.los_normal_mapping = [[
vec3 los_normal_mapping(in vec3 normal, in vec3 tangent, in vec2 texcoord, in sampler2D sampler)
{
	vec3 tmp = normalize(normal);
	mat3 tangentspace = mat3(tangent, tmp * tangent, tmp);
	vec3 n = normalize(texture(sampler, texcoord).xyz * 2.0 - 1.0);
	return normalize(tangentspace * n);
}]]

Shader.normalmapping = [[
vec3 normalmapping()
{
	vec3 nor = normalize(var_normal);
	vec3 tan = normalize(var_tangent);
	vec3 bin = nor * tan;
	mat3 tangentspace = mat3(tan, bin, nor);
	vec3 n = normalize(texture2D(uni_texturemap[1], var_texcoord).xyz * 2.0 - 1.0);
	return normalize(tangentspace * n);
}]]

Shader.los_triangle_tangent = [[
vec3 los_triangle_tangent(in vec3 co0, in vec3 co1, in vec3 co2, in vec2 uv0, in vec2 uv1, in vec2 uv2)
{
	vec3 ed0 = co1 - co0;
	vec3 ed1 = co2 - co0;
	return normalize(ed1 * (uv1.y - uv0.y) - ed0 * (uv2.y - uv0.y));
}]]

Shader.los_shadow_mapping = [[
float los_shadow_mapping(in vec4 lightcoord, in sampler2DShadow sampler)
{
	float esm_c = 80.0;
	float esm_d = lightcoord.z;
	float esm_z = lightcoord.w * textureProj(sampler, lightcoord);
	float esm_f = exp(-esm_c * esm_d + esm_c * esm_z);
	return clamp(esm_f, 0.0, 1.0);
}]]

Shader.los_shadow_mapping_pcf = [[
float los_shadow_mapping_pcf(in vec4 lightcoord, in sampler2DShadow sampler)
{
	vec2 pcf = vec2(clamp(0.004 * lightcoord.z, 0.02, 0.2), 0.0);
	float blend = los_shadow_mapping(lightcoord, sampler);
	blend += los_shadow_mapping(lightcoord - pcf.xyyy, sampler);
	blend += los_shadow_mapping(lightcoord + pcf.xyyy, sampler);
	blend += los_shadow_mapping(lightcoord - pcf.yxyy, sampler);
	blend += los_shadow_mapping(lightcoord + pcf.yxyy, sampler);
	blend *= 0.2;
	return blend;
}]]
