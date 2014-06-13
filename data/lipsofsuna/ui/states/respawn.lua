local UiButton = require("ui/widgets/button")

Ui:add_state{
	state = "respawn",
	label = "Respawn",
	update = function()
		if not Client.player_object.dead then
			Ui:set_state("play")
		end
	end}

Ui:add_widget{
	state = "respawn",
	widget = function()
		return UiButton("Respawn", function()
			Main.messaging:client_event("respawn")
		end)
	end}
