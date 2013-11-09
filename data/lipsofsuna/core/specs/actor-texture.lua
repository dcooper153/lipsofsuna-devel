--- Specifies an eye, face or other actor texture.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.specs.actor_texture
-- @alias Actorskinspec

local Class = require("system/class")
local Spec = require("core/specs/spec")

--- Specifies an eye, face or other actor texture.
-- @type Actorskinspec
local ActorTextureSpec = Spec:register("ActorTextureSpec", "actor skin", {
	{name = "name", type = "string", description = "Name of the spec."},
	{name = "categories", type = "dict", dict = {type = "boolean"}, default = {}, description = "Dictionary of categories."},
	{name = "actors", type = "dict", dict = {type = "boolean"}, default = {}, description = "Dictionary of actor spec names.", details = {keys = {spec = "Actorspec"}}},
	{name = "blit_dst", type = "list", list = {type = "number"}, description = "Blit destination point in pixels"},
	{name = "blit_mode", type = "string", description = "Blit mode name."},
	{name = "blit_texture", type = "string", description = "Texture name."},
	{name = "color", type = "string", description = "Color name."},
	{name = "label", type = "string", description = "Label to display in the charactor creator."},
	{name = "usages", type = "dict", dict = {type = "boolean"}, default = {}, description = "Dictionary of usage names."}
})

--- Registers an actor skin.
-- @param clss ActorTextureSpec class.
-- @param args Arguments.
-- @return ActorTextureSpec.
ActorTextureSpec.new = function(clss, args)
	local self = Spec.new(clss, args)
	self.introspect:read_table(self, args)
	return self
end

--- Gets textures by actor type.
-- @param self ActorTextureSpec class.
-- @param name Actor variant name.
-- @param usage Usage name.
-- @return List of skins.
ActorTextureSpec.find_by_actor_and_usage = function(self, name, usage)
	local res = {}
	for k,v in pairs(self.dict_name) do
		if v.actors[name] and v.usages[usage] then table.insert(res, v) end
	end
	table.sort(res, function(a,b) return a.name < b.name end)
	return res
end

return ActorTextureSpec
