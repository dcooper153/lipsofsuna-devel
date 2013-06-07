local Quickslots = require("core/quickslots/quickslots")

Message{
	name = "accept character",
	server_to_client_encode = function(self)
		return {}
	end,
	server_to_client_decode = function(self, packet)
		return {}
	end,
	server_to_client_handle = function(self)
		Ui:set_state("play")
		Operators.play:reset()
		Quickslots:reset()
	end}
