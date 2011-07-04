Eventhandler{type = "login", func = function(self, event)
	print("Client login")
	-- Tell the client to authenticate.
	Network:send{client = event.client, packet = Packet(packets.CLIENT_AUTHENTICATE)}
	-- Update lobby.
	Lobby.players = Lobby.players + 1
end}

Eventhandler{type = "logout", func = function(self, event)
	print("Client logout")
	-- Detach the player object.
	local object = Player.clients[event.client]
	if object then
		object:detach()
		Player.clients[event.client] = nil
	end
	-- Update the account.
	local account = Account.dict_client[event.client]
	if account then
		Serialize:save_account(account, object)
		account.client = nil
		Account.dict_client[event.client] = nil
		Account.dict_name[account.login] = nil
	end
	-- Update lobby.
	Lobby.players = Lobby.players - 1
end}

Eventhandler{type = "object-contact", func = function(self, event)
	if event.self.contact_cb then
		event.self:contact_cb(event)
	end
end}

Eventhandler{type = "object-motion", func = function(self, event)
	Vision:event{type = "object-moved", object = event.object}
end}
