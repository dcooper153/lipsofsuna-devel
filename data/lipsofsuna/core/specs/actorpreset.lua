--- TODO:doc
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.specs.actorpreset
-- @alias ActorPresetSpec

local Class = require("system/class")
local ChargenSliderSpec = require("core/specs/chargen-slider")
local Spec = require("core/specs/spec")

--- TODO:doc
-- @type ActorPresetSpec
local ActorPresetSpec = Spec:register("ActorPresetSpec", "actor preset", {
	{name = "name", type = "string", description = "Name of the spec."},
	{name = "categories", type = "dict", dict = {type = "boolean"}, default = {}, description = "Dictionary of categories."},
	{name = "animation_profile", type = "string", description = "Default animation profile."},
	{name = "body", type = "list", list = {type = "number"}, description = "Body morph table."},
	{name = "brow_style", type = "string", description = "Brow style name."},
	{name = "eye_color", type = "color", description = "Eye color."},
	{name = "eye_style", type = "string", description = "Eye style name."},
	{name = "face", type = "list", list = {type = "number"}, description = "Face morph table."},
	{name = "face_style", type = "string", description = "Face style name."},
	{name = "hair_color", type = "color", description = "Hair color."},
	{name = "hair_style", type = "string", description = "Hair style name."},
	{name = "head_style", type = "string", description = "Head style name."},
	{name = "height", type = "number", description = "Height multiplier."},
	{name = "mouth_style", type = "string", description = "Mouth style name."},
	{name = "playable", type = "boolean", description = "True to show the preset in the character creation screen."},
	{name = "skin_color", type = "color", description = "Skin color."},
	{name = "skin_style", type = "string", description = "Skin style name."}
})

--- Registers an actor preset.
-- @param clss ActorPresetSpec class.
-- @param args Arguments.
-- @return New actor preset spec.
ActorPresetSpec.new = function(clss, args)
	local self = Spec.new(clss, args)
	self.introspect:read_table(self, args)
	return self
end

--- Gets the spec data in the form suitable for actor creation.
-- @param self ActorPresetSpec.
-- @return Table.
ActorPresetSpec.get_actor = function(self)
	local copy = function(t)
		local res = {}
		for k,v in pairs(t) do res[k] = v end
		return res
	end
	local res = {
		animation_profile = self.animation_profile,
		body_sliders = self:get_chargen_body(),
		brow_style = self.brow_style,
		eye_color = copy(self.eye_color),
		eye_style = self.eye_style,
		face_sliders = self:get_chargen_face(),
		face_style = self.face_style,
		hair_color = copy(self.hair_color),
		hair_style = self.hair_style,
		head_style = self.head_style,
		height = self.height,
		skin_color = copy(self.skin_color),
		skin_style = self.skin_style,
		mouth_style = self.mouth_style}
	for k,v in pairs(res.body_sliders) do
		res.body_sliders[k] = 255 * v
	end
	for k,v in pairs(res.eye_color) do
		res.eye_color[k] = 255 * v
	end
	for k,v in pairs(res.face_sliders) do
		res.face_sliders[k] = 255 * v
	end
	for k,v in pairs(res.hair_color) do
		res.hair_color[k] = 255 * v
	end
	for k,v in pairs(res.skin_color) do
		res.skin_color[k] = 255 * v
	end
	return res
end

--- Gets the spec data in the form suitable for character creation.
-- @param self ActorPresetSpec.
-- @return Table.
ActorPresetSpec.get_chargen = function(self)
	return {
		animation_profile = self.animation_profile,
		body = self:get_chargen_body(),
		brow_style = self.brow_style,
		eye_color = self.eye_color,
		eye_style = self.eye_style,
		face = self:get_chargen_face(),
		face_style = self.face_style,
		hair_color = self.hair_color,
		hair_style = self.hair_style,
		head_style = self.head_style,
		height = self.height,
		skin_color = self.skin_color,
		skin_style = self.skin_style,
		mouth_style = self.mouth_style}
end

--- Gets the body in the list form suitable for character creation.
-- @param self ActorPresetSpec.
-- @return List of numbers.
ActorPresetSpec.get_chargen_body = function(self)
	local list = {}
	local count = 0
	for k,v in ipairs(Main.specs:find_by_category("ChargenSliderSpec", "body")) do
		count = math.max(count, v.field_index)
		list[v.field_index] = self.body[v.name] or v.default
	end
	for i = 1,count do
		if not list[i] then
			list[i] = 0
		end
	end
	return list
end

--- Gets the face in the list form suitable for character creation.
-- @param self ActorPresetSpec.
-- @return List of numbers.
ActorPresetSpec.get_chargen_face = function(self)
	local list = {}
	local count = 0
	for k,v in ipairs(Main.specs:find_by_category("ChargenSliderSpec", "face")) do
		count = math.max(count, v.field_index)
		list[v.field_index] = self.face[v.name] or v.default
	end
	for i = 1,count do
		if not list[i] then
			list[i] = 0
		end
	end
	return list
end

return ActorPresetSpec
