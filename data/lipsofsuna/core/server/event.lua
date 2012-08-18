Eventhandler{type = "login", func = function(self, event)
	Log:format("Client connect from %q.", Network:get_client_address(event.client) or "???")
	-- Tell the client to authenticate.
	Game.messaging:server_event("login", event.client)
	-- Update lobby.
	Lobby.players = Lobby.players + 1
end}

Eventhandler{type = "logout", func = function(self, event)
	Log:text("Client disconnect.")
	-- Detach the player object.
	local object = Server.players_by_client[event.client]
	if object then
		Server.object_database:save_object(object)
		object:detach()
		Server.players_by_client[event.client] = nil
	end
	-- Update the account.
	local account = Server.accounts_by_client[event.client]
	if account then
		if not object then
			Server.account_database:save_account(account)
		end
		account.client = nil
		Server.accounts_by_client[event.client] = nil
		Server.accounts_by_name[account.login] = nil
	end
	-- Update lobby.
	Lobby.players = Lobby.players - 1
end}

Eventhandler{type = "object-contact", func = function(self, event)
	if event.self.contact_cb then
		event.self:contact_cb(event)
	end
end}

Eventhandler{type = "sector-load", func = function(self, args)
	if Game.initialized then
		Game.sectors:load_sector(args.sector)
	end
end}

Eventhandler{type = "tick", func = function(self, event)
	Server:update(event.secs)
end}
