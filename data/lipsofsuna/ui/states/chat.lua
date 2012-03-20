Ui:add_state{
	state = "chat",
	label = "Chat"}

Ui:add_widget{
	state = "chat",
	widget = function()
		local widget = Widgets.Uientry("Chat")
		widget.input_mode = true
		widget.hint = "[RETURN]: Send\n[ESCAPE]: Cancel"
		widget.accepted = function(w)
			-- Perform the command.
			local cmd,match = ChatCommand:find(w.value)
			if cmd then cmd.func{match} end
			-- Close the chat unless the command did already.
			if Ui.state == "chat" then Ui:pop_state() end
		end
		widget.canceled = function(w)
			Ui:pop_state()
		end
		return widget
	end}
