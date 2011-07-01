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

--- Calculates skeletal animation for the vertex shader inputs.
Shader.los_animation_co = function()
	return [[int anim_i;
	int anim_bone[8] = int[](
		int(LOS_bones1.x), int(LOS_bones1.y), int(LOS_bones1.z), int(LOS_bones1.w),
		int(LOS_bones2.x), int(LOS_bones2.y), int(LOS_bones2.z), int(LOS_bones2.w));
	float anim_weight[8] = float[](
		LOS_weights1.x, LOS_weights1.y, LOS_weights1.z, LOS_weights1.w,
		LOS_weights2.x, LOS_weights2.y, LOS_weights2.z, LOS_weights2.w);
	vec3 anim_coord = vec3(0.0);
	vec3 anim_normal = vec3(0.0);
	for (anim_i = 0 ; anim_i < 8 ; anim_i++)
	{
		int offset = 3 * anim_bone[anim_i];
		vec3 restpos = texelFetch(LOS_buffer_texture, offset).xyz;
		vec4 posepos = texelFetch(LOS_buffer_texture, offset + 1);
		vec4 poserot = texelFetch(LOS_buffer_texture, offset + 2);
		vec3 v1 = (LOS_coord - restpos) * posepos.w;
		vec4 a1 = vec4(
			 (poserot.w * v1.x) + (poserot.y * v1.z) - (poserot.z * v1.y),
			 (poserot.w * v1.y) - (poserot.x * v1.z) + (poserot.z * v1.x),
			 (poserot.w * v1.z) + (poserot.x * v1.y) - (poserot.y * v1.x),
			-(poserot.x * v1.x) - (poserot.y * v1.y) - (poserot.z * v1.z));
		anim_coord += anim_weight[anim_i] * (posepos.xyz + vec3(
			(a1.w * -poserot.x) + (a1.x *  poserot.w) + (a1.y * -poserot.z) - (a1.z * -poserot.y),
			(a1.w * -poserot.y) - (a1.x * -poserot.z) + (a1.y *  poserot.w) + (a1.z * -poserot.x),
			(a1.w * -poserot.z) + (a1.x * -poserot.y) - (a1.y * -poserot.x) + (a1.z *  poserot.w)));
	}]]
end

--- Calculates skeletal animation for the vertex shader inputs.
Shader.los_animation_cono = function()
	return [[int anim_i;
	int anim_bone[8] = int[](
		int(LOS_bones1.x), int(LOS_bones1.y), int(LOS_bones1.z), int(LOS_bones1.w),
		int(LOS_bones2.x), int(LOS_bones2.y), int(LOS_bones2.z), int(LOS_bones2.w));
	float anim_weight[8] = float[](
		LOS_weights1.x, LOS_weights1.y, LOS_weights1.z, LOS_weights1.w,
		LOS_weights2.x, LOS_weights2.y, LOS_weights2.z, LOS_weights2.w);
	vec3 anim_coord = vec3(0.0);
	vec3 anim_normal = vec3(0.0);
	for (anim_i = 0 ; anim_i < 8 ; anim_i++)
	{
		int offset = 3 * anim_bone[anim_i];
		vec3 restpos = texelFetch(LOS_buffer_texture, offset).xyz;
		vec4 posepos = texelFetch(LOS_buffer_texture, offset + 1);
		vec4 poserot = texelFetch(LOS_buffer_texture, offset + 2);
		vec3 v1 = (LOS_coord - restpos) * posepos.w;
		vec4 a1 = vec4(
			 (poserot.w * v1.x) + (poserot.y * v1.z) - (poserot.z * v1.y),
			 (poserot.w * v1.y) - (poserot.x * v1.z) + (poserot.z * v1.x),
			 (poserot.w * v1.z) + (poserot.x * v1.y) - (poserot.y * v1.x),
			-(poserot.x * v1.x) - (poserot.y * v1.y) - (poserot.z * v1.z));
		anim_coord += anim_weight[anim_i] * (posepos.xyz + vec3(
			(a1.w * -poserot.x) + (a1.x *  poserot.w) + (a1.y * -poserot.z) - (a1.z * -poserot.y),
			(a1.w * -poserot.y) - (a1.x * -poserot.z) + (a1.y *  poserot.w) + (a1.z * -poserot.x),
			(a1.w * -poserot.z) + (a1.x * -poserot.y) - (a1.y * -poserot.x) + (a1.z *  poserot.w)));
		vec3 v2 = LOS_normal;
		vec4 a2 = vec4(
			 (poserot.w * v2.x) + (poserot.y * v2.z) - (poserot.z * v2.y),
			 (poserot.w * v2.y) - (poserot.x * v2.z) + (poserot.z * v2.x),
			 (poserot.w * v2.z) + (poserot.x * v2.y) - (poserot.y * v2.x),
			-(poserot.x * v2.x) - (poserot.y * v2.y) - (poserot.z * v2.z));
		anim_normal += anim_weight[anim_i] * vec3(
			(a2.w * -poserot.x) + (a2.x *  poserot.w) + (a2.y * -poserot.z) - (a2.z * -poserot.y),
			(a2.w * -poserot.y) - (a2.x * -poserot.z) + (a2.y *  poserot.w) + (a2.z * -poserot.x),
			(a2.w * -poserot.z) + (a2.x * -poserot.y) - (a2.y * -poserot.x) + (a2.z *  poserot.w));
	}
	anim_normal = normalize(anim_normal) * length(LOS_normal);]]
end

--- Calculates skeletal animation for the vertex shader inputs.
Shader.los_animation_conota = function()
	return [[int anim_i;
	int anim_bone[8] = int[](
		int(LOS_bones1.x), int(LOS_bones1.y), int(LOS_bones1.z), int(LOS_bones1.w),
		int(LOS_bones2.x), int(LOS_bones2.y), int(LOS_bones2.z), int(LOS_bones2.w));
	float anim_weight[8] = float[](
		LOS_weights1.x, LOS_weights1.y, LOS_weights1.z, LOS_weights1.w,
		LOS_weights2.x, LOS_weights2.y, LOS_weights2.z, LOS_weights2.w);
	vec3 anim_coord = vec3(0.0);
	vec3 anim_normal = vec3(0.0);
	vec3 anim_tangent = vec3(0.0);
	for (anim_i = 0 ; anim_i < 8 ; anim_i++)
	{
		int offset = 3 * anim_bone[anim_i];
		vec3 restpos = texelFetch(LOS_buffer_texture, offset).xyz;
		vec4 posepos = texelFetch(LOS_buffer_texture, offset + 1);
		vec4 poserot = texelFetch(LOS_buffer_texture, offset + 2);
		vec3 v1 = (LOS_coord - restpos) * posepos.w;
		vec4 a1 = vec4(
			 (poserot.w * v1.x) + (poserot.y * v1.z) - (poserot.z * v1.y),
			 (poserot.w * v1.y) - (poserot.x * v1.z) + (poserot.z * v1.x),
			 (poserot.w * v1.z) + (poserot.x * v1.y) - (poserot.y * v1.x),
			-(poserot.x * v1.x) - (poserot.y * v1.y) - (poserot.z * v1.z));
		anim_coord += anim_weight[anim_i] * (posepos.xyz + vec3(
			(a1.w * -poserot.x) + (a1.x *  poserot.w) + (a1.y * -poserot.z) - (a1.z * -poserot.y),
			(a1.w * -poserot.y) - (a1.x * -poserot.z) + (a1.y *  poserot.w) + (a1.z * -poserot.x),
			(a1.w * -poserot.z) + (a1.x * -poserot.y) - (a1.y * -poserot.x) + (a1.z *  poserot.w)));
		vec3 v2 = LOS_normal;
		vec4 a2 = vec4(
			 (poserot.w * v2.x) + (poserot.y * v2.z) - (poserot.z * v2.y),
			 (poserot.w * v2.y) - (poserot.x * v2.z) + (poserot.z * v2.x),
			 (poserot.w * v2.z) + (poserot.x * v2.y) - (poserot.y * v2.x),
			-(poserot.x * v2.x) - (poserot.y * v2.y) - (poserot.z * v2.z));
		anim_normal += anim_weight[anim_i] * vec3(
			(a2.w * -poserot.x) + (a2.x *  poserot.w) + (a2.y * -poserot.z) - (a2.z * -poserot.y),
			(a2.w * -poserot.y) - (a2.x * -poserot.z) + (a2.y *  poserot.w) + (a2.z * -poserot.x),
			(a2.w * -poserot.z) + (a2.x * -poserot.y) - (a2.y * -poserot.x) + (a2.z *  poserot.w));
		vec3 v3 = LOS_tangent;
		vec4 a3 = vec4(
			 (poserot.w * v3.x) + (poserot.y * v3.z) - (poserot.z * v3.y),
			 (poserot.w * v3.y) - (poserot.x * v3.z) + (poserot.z * v3.x),
			 (poserot.w * v3.z) + (poserot.x * v3.y) - (poserot.y * v3.x),
			-(poserot.x * v3.x) - (poserot.y * v3.y) - (poserot.z * v3.z));
		anim_tangent += anim_weight[anim_i] * vec3(
			(a3.w * -poserot.x) + (a3.x *  poserot.w) + (a3.y * -poserot.z) - (a3.z * -poserot.y),
			(a3.w * -poserot.y) - (a3.x * -poserot.z) + (a3.y *  poserot.w) + (a3.z * -poserot.x),
			(a3.w * -poserot.z) + (a3.x * -poserot.y) - (a3.y * -poserot.x) + (a3.z *  poserot.w));
	}
	anim_normal = normalize(anim_normal) * length(LOS_normal);
	anim_tangent = normalize(anim_tangent);]]
end

--- Calculates lighting and stores it to a variable named lighting.
-- @param co Variable name that contains the fragment coordinate.
-- @param no Variable name that contains the fragment normal.
-- @param lv Array name that contains the light vectors.
-- @param hv Array name that contains the light half vectors.
-- @param sp Variable name that contains additional specular color.
-- @param sh Variable name that contains shininess override value.
Shader.los_lighting_default = function(co, no, lv, hv, sp, sh)
	return string.format([[int lighting_index;
	vec4 lighting = vec4(0.0, 0.0, 0.0, 1.0);
	for(lighting_index = 0 ; lighting_index < LOS_LIGHT_MAX ; lighting_index++)
	{
		vec3 lv = %s[lighting_index];
		float fattn = 1.0 / dot(LOS_light[lighting_index].equation, vec3(1.0, length(lv), dot(lv, lv)));
		float fdiff = max(0.0, dot(%s, normalize(lv)));
		float coeff = max(0.0, dot(%s, normalize(%s[lighting_index])));
		float fspec = pow(max(0.0, coeff), %s);
		lighting.rgb += fattn * (LOS_light[lighting_index].ambient.rgb +
			fdiff * LOS_light[lighting_index].diffuse.rgb +
			fdiff * fspec * LOS_light[lighting_index].specular.rgb * %s.rgb);
	}]], lv, no, no, hv, sh or "LOS_material_shininess", sp or "LOS_material_specular")
end

--- Calculates lighting and stores it to a variable named lighting.
-- @param co Variable name that contains the fragment coordinate.
-- @param no Variable name that contains the fragment normal.
-- @param lv Array name that contains the light vectors.
-- @param hv Array name that contains the light half vectors.
-- @param sp Variable name that contains additional specular color.
-- @param sh Variable name that contains shininess override value.
Shader.los_lighting_skin = function(co, no, lv, hv, sp, sh)
	return string.format([[int lighting_index;
	vec4 lighting = vec4(0.0, 0.0, 0.0, 1.0);
	for(lighting_index = 0 ; lighting_index < LOS_LIGHT_MAX ; lighting_index++)
	{
		vec3 lv = %s[lighting_index];
		vec3 no = %s;
		vec3 co = normalize(%s);
		vec3 hv = %s[lighting_index];
		vec3 HV = normalize(hv);
		float sh = %s/128.0;
		float ndoth = dot(no, HV);
		float ndotl = dot(no, normalize(lv));
		/* Kelemen/Szirmay-Kalos specular. */
		float m = 0.5;
		float fexp = pow(1.0 - dot(co, HV), 5.0);
		float fres = fexp + 0.028 * (1.0 - fexp);
		vec4 beck = pow(2.0 * texture(LOS_diffuse_texture_3, vec2(ndoth, m)), vec4(10.0));
		vec4 spec = clamp(beck * fres / dot(hv, hv), vec4(0.0), vec4(1.0));
		float fspec = sh * ndotl * dot(spec, vec4(1.0, 0.625, 0.075, 0.005));
		/* Combine. */
		float fattn = 1.0 / dot(LOS_light[lighting_index].equation, vec3(1.0, length(lv), dot(lv, lv)));
		float fdiff = max(0.0, dot(no, normalize(lv)));
		lighting.rgb += fattn * (LOS_light[lighting_index].ambient.rgb +
			fdiff * LOS_light[lighting_index].diffuse.rgb +
			fspec * LOS_light[lighting_index].specular.rgb);
	}]], lv, no, co, hv, sh or "LOS_material_shininess")
end

--- Calculates lighting and stores it to a variable named lighting.
-- @param co Variable name that contains the fragment coordinate.
-- @param no Variable name that contains the fragment normal.
-- @param ta Variable name that contains the fragment tangent.
-- @param lv Array name that contains the light vectors.
-- @param hv Array name that contains the light half vectors.
Shader.los_lighting_hair = function(co, no, ta, lv, hv)
	return string.format([[int lighting_index;
	vec4 lighting = vec4(0.0, 0.0, 0.0, 1.0);
	for(lighting_index = 0 ; lighting_index < LOS_LIGHT_MAX ; lighting_index++)
	{
		vec3 lv = %s[lighting_index];
		float fattn = 1.0 / dot(LOS_light[lighting_index].equation, vec3(1.0, length(lv), dot(lv, lv)));
		float fdiff = max(0.0, 0.25 + 0.75 * dot(%s, normalize(lv)));
		float coeff1 = max(0.0, dot(%s, normalize(%s[lighting_index])));
		float tanref = max(0.0, dot(%s, reflect(-normalize(%s), %s)));
		float coeff2 = sqrt(1.0 - tanref * tanref);
		float coeff = mix(coeff1, coeff2, 0.6);
		float fspec = pow(coeff, LOS_material_shininess);
		lighting.rgb += fattn * (LOS_light[lighting_index].ambient.rgb +
			fdiff * LOS_light[lighting_index].diffuse.rgb +
			fdiff * fspec * LOS_light[lighting_index].specular.rgb * LOS_material_specular.rgb);
	}]], lv, no, no, hv, ta, co, no)
end

--- Calculates the light vectors and half vectors.
-- @param lv Array name where to return light vectors.
-- @param hv Array name where to return half vectors.
-- @oaran co Variable name that contains the transformed vertex coordinate.
Shader.los_lighting_vectors = function(lv, hv, co)
	return string.format([[int lighting_vindex;
	for(lighting_vindex = 0 ; lighting_vindex < LOS_LIGHT_MAX ; lighting_vindex++)
	{
		vec3 lighting_vector = LOS_light[lighting_vindex].position_premult - %s;
		%s[lighting_vindex] = lighting_vector;
		%s[lighting_vindex] = normalize(lighting_vector - %s);
	}]], co, lv, hv, co)
end

Shader.los_normal_mapping = [[
vec3 los_normal_mapping(in vec3 normal, in vec3 tangent, in vec2 texcoord, in sampler2D sampler)
{
	vec3 tmp = normalize(normal);
	mat3 tangentspace = mat3(tangent, cross(tangent, tmp), tmp);
	vec3 n = normalize(texture(sampler, texcoord).xyz * 2.0 - 1.0);
	return normalize(tangentspace * n);
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
