local UiLabel = require("ui/widgets/label")
local UiInvCraft = require("ui/widgets/invcraft")

Ui:add_state{
	state = "crafting",
	label = "Crafting",
	init = function()
		-- Get the player object.
		local object = Client.player_object
		if not object then return end
		-- Create the crafting widgets.
		local recipes = Operators.crafting:get_craftable()
		local widgets = {}
		for k,v in ipairs(recipes) do
			table.insert(widgets, UiInvCraft(object:get_id(), v))
		end
		-- Create the fallback widget if nothing to craft.
		if #widgets == 0 then
			table.insert(widgets, UiLabel("You don't have enough materials in your inventory to craft anything."))
		end
		return widgets
	end}
