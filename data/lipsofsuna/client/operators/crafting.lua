Operators.crafting = Class()
Operators.crafting.data = {}

--- Resets the crafting operator.
--
-- Context: Any.
--
-- @param self Operator.
Operators.crafting.reset = function(self)
	self.data = {}
end

--- Updates the craftability status of items.
--
-- Context: Must have logged into a game.
--
-- @param self Operator.
Operators.crafting.get_craftable = function(self)
	local object = Client.player_object
	local recipes = Crafting:get_craftable(object)
	table.sort(recipes)
	return recipes
end

--- Updates the craftability status of items.
--
-- Context: Must have logged into a game.
--
-- @param self Operator.
Operators.crafting.update_craftability = function(self)
	local object = Client.player_object
	for k,v in pairs(Ui.widgets) do
		if v.class_name == "Widgets.Uiinvcraft" then
			local spec = Itemspec:find{name = v.name}
			v:set_enabled(Crafting:can_craft(spec, object))
		end
	end
end
