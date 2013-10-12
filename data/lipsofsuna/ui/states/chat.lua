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
			local cmd,match = ChatCommand:find(w.value, "client")
			if cmd then cmd.func(Client.player_object, {match}) end
			-- Close the chat unless the command did already.
			if Ui:get_state() == "chat" then Ui:pop_state() end
			Client.effects:play_global("uitransition1")
		end
		widget.canceled = function(w)
			Ui:pop_state()
			Client.effects:play_global("uitransition1")
		end
		return widget
	end}
