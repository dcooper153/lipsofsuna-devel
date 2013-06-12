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

--- Builds character textures.
-- @type TextureBuilder
local TextureBuilder = Class("TextureBuilder")

--- Builds the textures for the given actor.
-- @param clss TextureBuilder class.
-- @param object Object whose model to build.
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
	-- Build the character textures.
	-- The result is handled in the tick handler in event.lua.
	return clss:build{
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
		spec = object:get_spec()}
end

--- Builds the mesh for the given object.
-- @param clss TextureBuilder class.
-- @param args Model building arguments.
-- @return Texture override dictionary.
TextureBuilder.build = function(clss, args)
	-- Add equipment textures.
	-- TODO: Texture priorities for equipment.
	local textures = {}
	if args.equipment then
		for slot,name in pairs(args.equipment) do
			local spec = Itemspec:find{name = name}
			if spec then
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
		end
	end
	-- Merge the textures.
	local overrides = {}
	for basename,blits in pairs(textures) do
		local base = Image(basename .. ".png")
		if base then
			for k,blitname in ipairs(blits) do
				local blit = Image(blitname .. ".png")
				if blit then
					base:blit(blit)
				end
			end
			overrides[basename] = base
		end
	end
	return overrides
end

return TextureBuilder
