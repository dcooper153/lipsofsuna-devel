--- TODO:doc
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.specs.personality
-- @alias Personalityspec

local Class = require("system/class")
local Spec = require("core/specs/spec")

--- TODO:doc
-- @type Personalityspec
Personalityspec = Spec:register("Personalityspec", "personality", {
	{name = "name", type = "string", description = "Name of the spec."},
	{name = "categories", type = "dict", dict = {type = "boolean"}, default = {}, description = "Dictionary of categories."},
	{name = "phrases_angered", type = "list", list = {type = "string"}, description = "List of angered phrases."},
	{name = "phrases_combat", type = "list", list = {type = "string"}, description = "List of combat phrases."},
	{name = "phrases_death", type = "list", list = {type = "string"}, description = "List of death phrases."}
})

--- Registers an actor personality.
-- @param clss Personalityspec class.
-- @param args Arguments.
-- @return New personality spec.
Personalityspec.new = function(clss, args)
	local self = Spec.new(clss, args)
	self.introspect:read_table(self, args)
	return self
end

--- Gets a random phrase for the given situation.
-- @param self Personality spec.
-- @param type Phrase type: "angered"/"combat"/"death".
Personalityspec.get_phrase = function(self, type)
	local rnd = function(tbl)
		if not tbl then return end
		local len = #tbl
		if len == 0 then return end
		return tbl[math.random(1, len)]
	end
	return rnd(self["phrases_" .. type])
end


