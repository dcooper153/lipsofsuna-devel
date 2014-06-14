local Class = require("system/class")

Operators.crafting = Class("CraftingOperator")
Operators.crafting.data = {}

--- Resets the crafting operator.
--
-- Context: Any.
--
-- @param self Operator.
Operators.crafting.reset = function(self)
	self.data = {}
end

--- Gets the list of craftable items.
--
-- Context: Must have logged into a game.
--
-- @param self Operator.
Operators.crafting.get_craftable = function(self)
	-- TODO: Take the crafting mode into account.
	local player = Client.player_object
	local recipes = Main.crafting_utils:get_craftable(player, self.data.mode)
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
	for k,v in Ui.widgets:get_children() do
		if v.class_name == "UiInvCraft" then
			local spec = Main.specs:find_by_name("CraftingRecipeSpec", v.name)
			v:set_enabled(Main.crafting_utils:can_craft(spec, object, self.data.mode))
		end
	end
end

--- Sets the current crafting mode.
--
-- Context: Must have logged into a game.
--
-- @param self Operator.
-- @param value Crafting mode string.
Operators.crafting.set_mode = function(self, value)
	self.data.mode = value
end
