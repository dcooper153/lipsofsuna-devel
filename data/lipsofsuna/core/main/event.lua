local Eventhandler = require("system/eventhandler")
local Lobby = require("system/lobby")
local Network = require("system/network")
local Timer = require("system/timer")

local handle_input = function(self, args)
	Client.input:event(args)
	if Ui:handle_event(args) then
		Client.bindings:event(args)
	end
end

Eventhandler{type = "keypress", func = handle_input}

Eventhandler{type = "keyrelease", func = handle_input}

Eventhandler{type = "mousepress", func = handle_input}

Eventhandler{type = "mouserelease", func = handle_input}

Eventhandler{type = "mousescroll", func = handle_input}

Eventhandler{type = "mousemotion", func = handle_input}

Eventhandler{type = "music-ended", func = function(self, args)
	Client.effects:cycle_music_track()
end}

Eventhandler{type = "quit", func = function(self, args)
	Program:set_quit(true)
end}

Eventhandler{type = "login", func = function(self, event)
	Server.log:format("Client connect from %q.", Network:get_client_address(event.client) or "???")
	-- Tell the client to authenticate.
	Game.messaging:server_event("login", event.client)
	-- Update lobby.
	Lobby:set_players(Lobby:get_players() + 1)
end}

Eventhandler{type = "logout", func = function(self, event)
	Server.log:text("Client disconnect.")
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
	Lobby:set_players(Lobby:get_players() - 1)
end}

Eventhandler{type = "object-contact", func = function(self, event)
	if event.self then
		event.self = Game.objects:find_by_id(event.self)
		if not event.self then return end
	end
	if event.object then
		event.object = Game.objects:find_by_id(event.object)
		if not event.object then return end
	end
	if event.self.contact_cb then
		event.self:contact_cb(event)
	end
end}

Eventhandler{type = "packet", func = function(self, args)
	Game.messaging:handle_packet(args.client, args.packet)
end}

Eventhandler{type = "sector-load", func = function(self, args)
	if Game.initialized then
		Game.sectors:load_sector(args.sector)
	end
end}

Timer{delay = 3000, func = function()
	if Server.initialized then
		Lobby:upload_server_info()
	end
end}
