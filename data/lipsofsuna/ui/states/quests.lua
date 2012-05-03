Ui:add_state{
	state = "quests",
	label = "Quests",
	init = function()
		local widgets = {}
		local active = Operators.quests:get_active_quest()
		for k,v in ipairs(Operators.quests:get_quests()) do
			local widget = Widgets.Uiquest(v[2])
			if v[2] == active then
				widget.value = true
			end
			table.insert(widgets, widget)
		end
		return widgets
	end}
