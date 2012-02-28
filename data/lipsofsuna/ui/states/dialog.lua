
Ui:add_state{
	state = "dialog",
	init = function()
		local o = Client.active_dialog_object
		if not o then return end
		local dlg = o.dialog
		if not dlg then return end
		local widgets = {}
		if dlg.type == "choice" then
			-- Show dialog choices.
			for k,v in ipairs(dlg.choices) do
				table.insert(widgets, Widgets.Uidialogchoice(o.id, k, v))
			end
		else
			-- Show a dialog line.
			if dlg.character ~= "" then
				table.insert(widgets, Widgets.Uidialogtext(o.id, 1, dlg.character))
			end
			table.insert(widgets, Widgets.Uidialogtext(o.id, 1, dlg.message))
		end
		return widgets
	end}
