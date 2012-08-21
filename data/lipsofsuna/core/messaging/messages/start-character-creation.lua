Message{
	name = "start character creation",
	server_to_client_encode = function(self)
		return {}
	end,
	server_to_client_decode = function(self, packet)
		return {}
	end,
	server_to_client_handle = function(self)
		Client:set_player_object(nil)
		Operators.chargen:init()
		Ui:set_state("chargen")
	end}
