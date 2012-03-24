Ui:add_state{
	state = "help",
	label = "Help",
	init = function()
		-- Find the help text.
		local help = Helpspec:find{name = Client.data.help.page}
		if not help then return end
		-- Create the widgets.
		local widgets = {Widgets.Uilabel(help.title)}
		for k,v in ipairs(help.text) do
			if v[1] == "link" then
				table.insert(widgets, Widgets.Uihelplink(v[2], v[3]))
			elseif v[1] == "paragraph" then
				table.insert(widgets, Widgets.Uihelptext(v[2]))
			end
		end
		return widgets
	end}
