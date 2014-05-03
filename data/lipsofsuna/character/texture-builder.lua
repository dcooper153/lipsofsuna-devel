--- Builds character textures.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module character.texture_builder
-- @alias TextureBuilder

local ActorTextureSpec = require("core/specs/actor-texture")
local Class = require("system/class")
local Color = require("system/color")
local HairStyleSpec = require("core/specs/hair-style")
local Image = require("system/image")
local ImageMerger = require("system/image-merger")
local Serialize = require("system/serialize")

local yield = coroutine.yield

--- Builds character textures.
-- @type TextureBuilder
local TextureBuilder = Class("TextureBuilder")

--- Creates a new texture builder.
-- @param clss TextureBuilder class.
-- @param object Object whose texture to build.
-- @return TextureBuilder.
TextureBuilder.new = function(clss, object)
	local self = Class.new(clss)
	local spec = object:get_spec()
	if spec and spec.models then
		self.object = object
		self.merger = ImageMerger()
		self.coroutine = coroutine.create(self.__main)
		local ret,err = coroutine.resume(self.coroutine, self, spec)
		if not ret then print(debug.traceback(self.coroutine, "ERROR: " .. err)) end
		if coroutine.status(self.coroutine) == "dead" then self.coroutine = nil end
	end
	return self
end

--- Updates the texture builder.
-- @param self TextureBuilder.
-- @param secs Seconds since the last update.
-- @return True if done. False otherwise.
TextureBuilder.update = function(self, secs)
	-- Update the image loader.
	if self.coroutine then
		local ret,err = coroutine.resume(self.coroutine, secs)
		if not ret then print(debug.traceback(self.coroutine, "ERROR: " .. err)) end
		if coroutine.status(self.coroutine) ~= "dead" then return end
		self.coroutine = nil
	end
	-- Update the image merger.
	if self.merger then
		local image = self.merger:pop_image()
		if not image then return end
		self.merger = nil
		self.result = image
	end
	return true
end

--- Pops the finished image.
-- @param self TextureBuilder.
-- @return Image on success. Nil otherwise.
TextureBuilder.pop_image = function(self)
	local r = self.result
	self.result = nil
	return r
end

--- The main function of the image loader coroutine.
-- @param self TextureBuilder.
-- @param spec ActorSpec.
TextureBuilder.__main = function(self, spec)
	local object = self.object
	local hash = object.texture_build_hash
	-- Get the appearance variables.
	local beheaded = object:get_beheaded()
	local body_scale = object.body_scale
	local body_sliders = object.body_sliders
	local brow_style = object.brow_style
	local eye_color = Color:ubyte_to_float(object.eye_color)
	local eye_style = object.eye_style
	local face_style = object.face_style
	local hair_color = Color:ubyte_to_float(object.hair_color)
	local hair_style = object.hair_style
	local head_style = object.head_style
	local mouth_style = object.mouth_style
	local nudity = Client.options.nudity_enabled
	local skin_color = Color:ubyte_to_float(object.skin_color)
	local skin_style = object.skin_style
	-- Find the equipment specs.
	local specs = {}
	for slot,index in pairs(object.inventory.equipped) do
		local item = object.inventory:get_object_by_index(index)
		if item then
			local spec1 = item:get_spec()
			if spec1 then
				table.insert(specs, spec1)
			end
		end
	end
	yield()
	-- Find the haircut spec.
	if hair_style and hair_style ~= "" then
		local spec1 = HairStyleSpec:find_by_name(hair_style)
		if spec1 then
			table.insert(specs, spec1)
		end
	end
	yield()
	-- Sort the specs by priority.
	table.sort(specs, function(a,b) return a.equipment_priority < b.equipment_priority end)
	yield()
	-- Find the equipment textures.
	local textures = {}
	for i,spec1 in ipairs(specs) do
		local tex = spec1:get_equipment_textures(spec.equipment_class or spec.name, lod)
		if tex then
			for k,v in pairs(tex) do
				if textures[k] then
					table.insert(textures[k], v)
				else
					textures[k] = {v}
				end
			end
		end
		yield()
	end
	-- Build and compare the hash.
	local hash1 = Serialize:write{brow_style, skin_style, skin_color, face_style, eye_style, eye_color, mouth_style, textures}
	if hash1 == hash then return end
	yield()
	-- Set the base texture.
	local basename = spec:get_base_texture()
	if not basename then return end
	local skinspec = skin_style and Actorskinspec:find_by_name(skin_style)
	local skinname = skinspec and skinspec.textures[1] or basename
	local base = Main.images:find_by_name_async(skinname, yield)
	if not base then return end
	local merger = self.merger
	merger:replace(base)
	yield()
	if skin_color then
		merger:add_hsv_weightv(skin_color[1], -1 + 2 * skin_color[2], -1 + 2 * skin_color[3])
		yield()
	end
	-- Blit the face textures.
	local colors = {eye_color = eye_color, hair_color = hair_color, skin_color = skin_color}
	self:__merge_colored(face_style, colors)
	self:__merge_colored(mouth_style, colors)
	self:__merge_colored(brow_style, colors)
	self:__merge_colored(eye_style, colors)
	-- Blit the additional textures.
	local blits = textures[basename]
	if blits then
		for k,blitname in ipairs(blits) do
			local blit = Main.images:find_by_name_async(blitname, yield)
			if blit then
				merger:blit(blit)
			end
			yield()
		end
	end
	merger:finish()
	object.texture_build_hash = hash1
	yield()
end

--- Adds an actor texture to the texture merger.
-- @param self TextureBuilder.
-- @param name ActorTextureSpec name.
-- @param colors Color dictionary.
TextureBuilder.__merge_colored = function(self, name, colors)
	if not name then return end
	local spec = ActorTextureSpec:find_by_name(name)
	if not spec then return end
	if not spec.blit_texture then return end
	local blit = Main.images:find_by_name_async(spec.blit_texture, yield)
	if not blit then return end
	local dst = spec.blit_dst
	local src = dst and spec.blit_src or {0, 0, 10000, 10000}
	local color = nil
	if spec.color == "eye_color" or spec.color == "skin_color" then
		color = colors[spec.color]
	end
	local merger = self.merger
	yield()
	if color then
		if spec.blit_mode == "hsv_add_weightv" then
			if dst then
				merger:blit_rect_hsv_add_weightv(blit, dst[1], dst[2], dst[3], dst[4], src[1], src[2], color[1], -1 + 2 * color[2], -1 + 2 * color[3])
			else
				merger:blit_hsv_add_weightv(blit, color[1], -1 + 2 * color[2], -1 + 2 * color[3])
			end
		else
			if dst then
				merger:blit_rect_hsv_add(blit, dst[1], dst[2], dst[3], dst[4], src[1], src[2], color[1], -1 + 2 * color[2], -1 + 2 * color[3])
			else
				merger:blit_hsv_add(blit, color[1], -1 + 2 * color[2], -1 + 2 * color[3])
			end
		end
	else
		if dst then
			merger:blit_rect(blit, dst[1], dst[2], dst[3], dst[4], src[1], src[2])
		else
			merger:blit(blit)
		end
	end
	yield()
end

return TextureBuilder
