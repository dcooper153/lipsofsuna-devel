Ui:add_state{
	state = "enchant",
	label = "Enchant",
	init = function()
		local widgets = {}
		-- Get the player object.
		local object = Client.player_object
		if not object then return end
		-- Get the weapon.
		local weapon = object:get_weapon()
		if not weapon then
			table.insert(widgets, Widgets.Uilabel("You must wield the weapon to enchant it."))
			return widgets
		end
		-- Create the widgets.
		if Main.crafting_utils:is_item_enchanted(weapon) then
			table.insert(widgets, Widgets.Uibutton("Disenchant", function()
				Main.messaging:client_event("disenchant")
			end))
		else
			-- TODO: Action selection
			-- TODO: Modifier selection
			table.insert(widgets, Widgets.Uibutton("Enchant", function()
				-- FIXME
				Main.messaging:client_event("enchant", "ranged spell",
				{
					["fire damage"] = 10,
					["poison"] = 10,
					["restore health"] = 5
				})
			end))
		end
		return widgets
	end}
