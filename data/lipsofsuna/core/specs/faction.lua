--- Faction specification.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.specs.faction
-- @alias FactionSpec

local Class = require("system/class")
local Spec = require("core/specs/spec")

--- Faction specification.
-- @type FactionSpec
local FactionSpec = Spec:register("FactionSpec", "faction", {
	{name = "name", type = "string", description = "Name of the spec."},
	{name = "categories", type = "dict", dict = {type = "boolean"}, default = {}, description = "Dictionary of categories."},
	{name = "enemies", type = "dict", dict = {type = "boolean"}, default = {}, description = "List of enemy faction names.", details = {keys = {spec = "FactionSpec"}}}
})

--- Creates a new faction.
-- @param clss FactionSpec class.
-- @param args Arguments.
-- @return FactionSpec.
FactionSpec.new = function(clss, args)
	local self = Spec.new(clss, args)
	self.introspect:read_table(self, args)
	return self
end

--- Adds an enemy faction.
-- @param self FactionSpec.
-- @param args Arguments.<ul>
--   <li>name: Faction name.</li></ul>
FactionSpec.add_enemy = function(self, args)
	self.enemies[args.name] = true
end

return FactionSpec
