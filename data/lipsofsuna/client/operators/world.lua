Operators.world = Class()

--- Gets the currently targeted object.</br>
--
-- Context: Any.
--
-- @param self Operator.
-- @return Object, or nil.
Operators.world.get_target_object = function(self)
	local object = Target.target_object
	if not object then return end
	if not object.realized then return end
	return object
end

--- Gets the usage actions of the currently targeted object.</br>
--
-- Context: Any.
--
-- @param self Operator.
-- @return List of actions, or nil.
Operators.world.get_target_usages = function(self)
	-- Get the targeted object.
	local object = self:get_target_object()
	if not object then return end
	-- Get the object spec.
	local spec = object.spec
	if not spec then return end
	local actions = {}
	-- Add the spec actions.
	if spec.get_use_actions then
		for k,v in pairs(spec:get_use_actions()) do
			table.insert(actions, {k, v.label or v.name, v})
		end
	end
	-- Add the hard-coded actions.
	if spec.type == "actor" then
		if spec.dialog then
			if object.dead then
				table.insert(actions, {"dialog", "Evaluate", nil})
			else
				table.insert(actions, {"dialog", "Chat", nil})
			end
		end
		if object.dead then
			table.insert(actions, {"loot", "Loot", nil})
		else
			table.insert(actions, {"pickpocket", "Pickpocket", nil})
		end
	elseif spec.type == "item" then
		table.insert(actions, {"pick up", "Pick up", nil})
	end
	-- Return the list if it's not empty.
	if not actions[1] then return end
	return actions
end