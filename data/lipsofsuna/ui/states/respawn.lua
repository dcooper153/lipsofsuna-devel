Ui:add_state{
	state = "respawn",
	label = "Respawn",
	update = function()
		if not Client.player_object.dead then
			Ui.state = "play"
		end
	end}

Ui:add_widget{
	state = "respawn",
	widget = function()
		return Widgets.Uibutton("Respawn", function()
			Game.messaging:client_event("respawn")
		end)
	end}
