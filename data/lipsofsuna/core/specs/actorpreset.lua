--- TODO:doc
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.specs.actorpreset
-- @alias Actorpresetspec

local Class = require("system/class")
local ChargenSliderSpec = require("core/specs/chargen-slider")
local Spec = require("core/specs/spec")

--- TODO:doc
-- @type Actorpresetspec
Actorpresetspec = Spec:register("Actorpresetspec", "actor preset", {
	{name = "name", type = "string", description = "Name of the spec."},
	{name = "categories", type = "dict", dict = {type = "boolean"}, default = {}, description = "Dictionary of categories."},
	{name = "animation_profile", type = "string", description = "Default animation profile."},
	{name = "body", type = "list", list = {type = "number"}, description = "Body morph table."},
	{name = "eye_color", type = "color", description = "Eye color."},
	{name = "eye_style", type = "string", description = "Eye style name."},
	{name = "face", type = "list", list = {type = "number"}, description = "Face morph table."},
	{name = "hair_color", type = "color", description = "Hair color."},
	{name = "hair_style", type = "string", description = "Hair style name."},
	{name = "head_style", type = "string", description = "Head style name."},
	{name = "height", type = "number", description = "Height multiplier."},
	{name = "playable", type = "boolean", description = "True to show the preset in the character creation screen."},
	{name = "skin_color", type = "color", description = "Skin color."},
	{name = "skin_style", type = "string", description = "Skin style name."}
})

--- Registers an actor preset.
-- @param clss Actorpresetspec class.
-- @param args Arguments.
-- @return New actor preset spec.
Actorpresetspec.new = function(clss, args)
	local self = Spec.new(clss, args)
	self.introspect:read_table(self, args)
	return self
end

--- Gets the spec data in the form suitable for character creation.
-- @param self Actorpresetspec.
-- @return Table.
Actorpresetspec.get_chargen = function(self)
	return {
		animation_profile = self.animation_profile,
		body = self:get_chargen_body(),
		eye_color = self.eye_color,
		eye_style = self.eye_style,
		face = self:get_chargen_face(),
		hair_color = self.hair_color,
		hair_style = self.hair_style,
		head_style = self.head_style,
		height = self.height,
		skin_color = self.skin_color,
		skin_style = self.skin_style}
end

--- Gets the body in the list form suitable for character creation.
-- @param self Actorpresetspec.
-- @return List of numbers.
Actorpresetspec.get_chargen_body = function(self)
	local list = {}
	local count = 0
	for k,v in ipairs(ChargenSliderSpec:find_by_category("body")) do
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
-- @param self Actorpresetspec.
-- @return List of numbers.
Actorpresetspec.get_chargen_face = function(self)
	local list = {}
	local count = 0
	for k,v in ipairs(ChargenSliderSpec:find_by_category("face")) do
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

return Actorpresetspec
