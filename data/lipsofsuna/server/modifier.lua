Modifier = Class()

--- Creates a new modifier.
-- @param clss Modifier class.
-- @param args Arguments.<ul>
--   <li>object: Target object.</li>
--   <li>spec: Feat effect spec.</li>
--   <li>strength: Strength value.</li></ul>
-- @return Modifier.
Modifier.new = function(clss, args)
	local self = Class.new(clss, args)
	self.timer = 0
	return self
end

--- Updates the modifiers of the object.
-- @param clss Modifier class.
-- @param object Object whose modifiers to update.
-- @param secs Seconds since the last update.
-- @param args Arguments sent to the modifier
Modifier.update = function(clss, object, secs)
	local num = 0
	local keep = {}
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
			object:removed_modifier(k)
		else
			keep[k] = v
			num = num + 1
		end
	end
	-- Update the modifier list.
	if object.dead then
		for k,v in pairs(keep) do
			object:remove_modifier(k)
		end
		object.modifiers = nil
	else
		object.modifiers = num > 0 and keep or nil
	end
end
