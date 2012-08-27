--- TODO:doc
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.server.modifier
-- @alias Modifier

local Class = require("system/class")

--- TODO:doc
-- @type Modifier
Modifier = Class("Modifier")

--- Creates a new modifier.
-- @param clss Modifier class.
-- @param args Arguments.<ul>
--   <li>object: Target object.</li>
--   <li>spec: Feat effect spec.</li>
--   <li>strength: Strength value.</li></ul>
-- @return Modifier.
Modifier.new = function(clss, args)
	local self = Class.new(clss)
	for k,v in pairs(args) do self[k] = v end
	self.timer = 0
	return self
end

--- Updates the modifiers of the object.
-- @param clss Modifier class.
-- @param object Object whose modifiers to update.
-- @param secs Seconds since the last update.
Modifier.update = function(clss, object, secs)
	local num = 0
	-- Update each modifier.
	for k,v in pairs(object.modifiers) do
		-- Update the modifier.
		local remove
		local effect = Feateffectspec:find{name = k}
		if effect and effect.modifier then
			remove = not effect:modifier(v, secs)
		else
			remove = true
		end
		-- Handle removal.
		if remove then
			object.modifiers[k] = nil
			object:removed_modifier(k)
		else
			num = num + 1
		end
	end
	-- Remove unused modifier lists.
	if object.dead then
		for k,v in pairs(object.modifiers) do
			object.modifiers[k] = nil
			object:removed_modifier(k)
		end
		object.modifiers = nil
	elseif num == 0 then
		object.modifiers = nil
	end
end


