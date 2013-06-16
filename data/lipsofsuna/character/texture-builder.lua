--- Builds character textures.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module character.texture_builder
-- @alias TextureBuilder

local Class = require("system/class")
local Color = require("system/color")
local Image = require("system/image")
local ImageMerger = require("system/image-merger")
local Serialize = require("system/serialize")

--- Builds character textures.
-- @type TextureBuilder
local TextureBuilder = Class("TextureBuilder")

--- Builds the textures for the given actor.
-- @param clss TextureBuilder class.
-- @param object Object whose texture to build.
-- @return Texture override dictionary.
TextureBuilder.build_for_actor = function(clss, object)
	if not object.spec then return end
	if not object.spec.models then return end
	-- Create the equipment list.
	local equipment = {}
	for k,v in pairs(object.inventory.equipped) do
		local item = object.inventory:get_object_by_index(v)
		equipment[k] = item.spec.name
	end
	-- Create or reuse the image merger.
	local merger = object.image_merger
	if not merger then
		merger = ImageMerger()
		object.image_merger = merger
	end
	-- Build the character textures.
	-- The result is handled in the update handler of Chargen.
	object.texture_build_hash = clss:build_with_merger(merger, {
		beheaded = object:get_beheaded(),
		body_scale = object.body_scale,
		body_style = object.body_style,
		equipment = equipment,
		eye_color = Color:ubyte_to_float(object.eye_color),
		eye_style = object.eye_style,
		face_style = object.face_style,
		hair_color = Color:ubyte_to_float(object.hair_color),
		hair_style = object.hair_style,
		head_style = object.head_style,
		nudity = Client.options.nudity_enabled,
		skin_color = Color:ubyte_to_float(object.skin_color),
		skin_style = object.skin_style,
		spec = object:get_spec()}, object.texture_build_hash)
end

--- Builds the texture for the given object.
--
-- If the hash argument is given, the texture building is skipped if the new
-- texture has the given has.
--
-- @param clss TextureBuilder class.
-- @param merger Image merger to use.
-- @param args Image building arguments.
-- @param hash Hash of the old texture, or nil.
-- @return hash Hash of the new texture.
TextureBuilder.build_with_merger = function(clss, merger, args, hash)
	-- Sort equipment by priority.
	local equipment = {}
	if args.equipment then
		for slot,name in pairs(args.equipment) do
			local spec = Itemspec:find{name = name}
			if spec then
				table.insert(equipment, spec)
			end
		end
		table.sort(equipment, function(a,b) return a.equipment_priority < b.equipment_priority end)
	end
	-- Add equipment textures.
	local textures = {}
	for i,spec in ipairs(equipment) do
		local tex = spec:get_equipment_textures(args.spec.equipment_class or args.spec.name, lod)
		if tex then
			for k,v in pairs(tex) do
				if textures[k] then
					table.insert(textures[k], v)
				else
					textures[k] = {v}
				end
			end
		end
	end
	-- Build and compare the hash.
	local hash1 = Serialize:write{
		args.skin_style,
		args.skin_color,
		args.face_texture,
		args.eye_style,
		args.eye_color,
		textures}
	if hash1 == hash then
		return hash
	end
	-- Set the base texture.
	local basename = "aer1" -- FIXME
	local skinspec = args.skin_style and Actorskinspec:find_by_name(args.skin_style)
	local skinname = skinspec and skinspec.textures[1] or basename
	local base = Main.images:copy_by_name(skinname)
	if not base then return end
	merger:replace(base)
	if args.skin_color then
		merger:add_hsv(args.skin_color[1], -1 + 2 * args.skin_color[2], -1 + 2 * args.skin_color[3])
	end
	-- Blit the face texture.
	local face = args.face_texture or "aerface1" --FIXME
	if face then
		local blit = Main.images:copy_by_name(face)
		if blit then
			merger:blit(blit)
		end
	end
	-- Blit the eye texture.
	local eye = args.eye_style
	if eye then
		local blit = Main.images:copy_by_name(eye)
		if blit then
			if args.eye_color then
				merger:blit_hsv_add(blit, args.eye_color[1], -1 + 2 * args.eye_color[2], -1 + 2 * args.eye_color[3])
			else
				merger:blit(blit)
			end
		end
	end
	-- Blit the additional textures.
	local blits = textures[basename]
	if blits then
		for k,blitname in ipairs(blits) do
			local blit = Main.images:copy_by_name(blitname)
			if blit then
				merger:blit(blit)
			end
		end
	end
	merger:finish()
	return hash1
end

return TextureBuilder
