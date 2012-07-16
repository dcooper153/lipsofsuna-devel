Message{
	name = "start character creation",
	server_to_client_encode = function(self)
		return {}
	end,
	server_to_client_decode = function(self, packet)
		return {}
	end,
	server_to_client_handle = function(self)
		Client.player_object = nil
		Operators.chargen:init()
		Ui.state = "chargen"
	end}
