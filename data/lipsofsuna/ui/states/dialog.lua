Ui:add_state{
	state = "dialog",
	label = "Dialog",
	update = function()
		local object = Client.active_dialog_object
		if not object or not object:get_visible() then
			Ui:pop_state()
		end
	end,
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
			table.insert(widgets, Widgets.Uidialogtext(o.id, 1, dlg.character, dlg.message))
		end
		return widgets
	end}
