Ui:add_state{
	state = "controls",
	label = "Controls",
	grab = function()
		return Client.player_object ~= nil
	end,
	background = function()
		if Client.player_object then return end
		return Widgets.Background{fullscreen = true, image = "mainmenu1"}
	end,
	init = function()
		-- Get the control actions.
		local actions = {}
		for k,v in pairs(Action.dict_name) do
			table.insert(actions, v)
		end
		table.sort(actions, function(a,b) return a.name < b.name end)
		-- Create the binding widgets.
		local widgets = {}
		for k,v in pairs(actions) do
			table.insert(widgets, Widgets.Uibinding(v))
		end
		return widgets
	end}

Ui:add_widget{
	state = "controls",
	widget = function()
		return Widgets.Uibutton("Save bindings", function()
			Client:save_controls()
		end)
	end}
