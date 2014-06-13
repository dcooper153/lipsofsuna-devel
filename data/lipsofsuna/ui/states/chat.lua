local UiEntry = require("ui/widgets/entry")

Ui:add_state{
	state = "chat",
	label = "Chat"}

Ui:add_widget{
	state = "chat",
	widget = function()
		local widget = UiEntry("Chat")
		widget.input_mode = true
		widget.hint = "[RETURN]: Send\n[ESCAPE]: Cancel"
		widget.accepted = function(w)
			-- Perform the command.
			local cmd,match = ChatCommand:find(w.value, "client")
			if cmd then cmd.func(Client.player_object, {match}) end
			-- Close the chat unless the command did already.
			if Ui:get_state() == "chat" then Ui:pop_state() end
			Main.effect_manager:play_global("uitransition1")
		end
		widget.canceled = function(w)
			Ui:pop_state()
			Main.effect_manager:play_global("uitransition1")
		end
		return widget
	end}
