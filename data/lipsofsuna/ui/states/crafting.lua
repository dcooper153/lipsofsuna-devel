
Ui:add_state{
	state = "crafting",
	init = function()
		-- Get the player object.
		local object = Client.player_object
		if not object then return end
		-- Get the list of crafting recipes.
		local recipes = Crafting:get_craftable{
			get_item = function(name)
				return object.inventory:count_objects_by_name(name)
			end,
			get_skill = function(name)
				return 100 -- FIXME
			end}
		table.sort(recipes)
		-- Create the crafting widgets.
		local widgets = {}
		for k,v in ipairs(recipes) do
			table.insert(widgets, Widgets.Uiinvcraft(object.id, v))
		end
		-- Create the fallback widget if nothing to craft.
		if #widgets == 0 then
			table.insert(widgets, Widgets.Uilabel("You don't have enough materials in your inventory to craft anything."))
		end
		return widgets
	end}
