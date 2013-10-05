local UiLabel = require("ui/widgets/label")
local UiHelpLink = require("help/ui-help-link")
local UiHelpText = require("help/ui-help-text")

Ui:add_state{
	state = "help",
	label = "Help",
	init = function()
		-- Find the help text.
		local help = Helpspec:find_by_name(Client.data.help.page)
		if not help then return end
		-- Create the widgets.
		local widgets = {UiLabel(help.title)}
		for k,v in ipairs(help.text) do
			if v[1] == "link" then
				table.insert(widgets, UiHelpLink(v[2], v[3]))
			elseif v[1] == "paragraph" then
				table.insert(widgets, UiHelpText(v[2]))
			end
		end
		return widgets
	end}
