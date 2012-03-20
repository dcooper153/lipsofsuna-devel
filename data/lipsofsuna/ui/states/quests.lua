Ui:add_state{
	state = "quests",
	label = "Quests",
	init = function()
		-- Sort the quests by name.
		local quests = {}
		for id,quest in pairs(Client.data.quests.quests) do
			local spec = Quest:find{id = id}
			table.insert(quests, {spec, quest})
		end
		table.sort(quests, function(a, b) return a[1].name < b[1].name end)
		-- Create a widget for each quest.
		local widgets = {}
		for k,v in ipairs(quests) do
			local text = string.format("Name: %s\nStatus: %s\nDescription: %s\n",
				v[1].name, v[2].status, v[2].text)
			table.insert(widgets, Widgets.Uilabel(text))
		end
		return widgets
	end}
