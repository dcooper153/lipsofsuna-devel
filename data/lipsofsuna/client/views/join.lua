require "client/widgets/serverinfo"

Views.Join = Class(Widgets.Background)

--- Creates a new join view.
-- @param clss Join class.
-- @return Join view.
Views.Join.new = function(clss)
	local self = Widgets.Background.new(clss, {cols = 1, rows = 2, fullscreen = true, image = "mainmenu1"})
	self.list_servers = Widgets.List{page_size = 7}
	self.list_servers:set_request{height = 255}
	return self
end

--- Returns back to the login screen.
-- @param self Join view.
Views.Join.back = function(self)
end

--- Closes the join view.
-- @param self Join view.
Views.Join.close = function(self)
end

--- Enters the join view.
-- @param self Join view.
Views.Join.enter = function(self)
	self:refresh()
end

--- Launches the selected game.
-- @param self Join view.
Views.Join.play = function(self)
end

--- Downloads the server list from the master server.
-- @param self Join view.
Views.Join.refresh = function(self)
	-- Download servers.
	Lobby.master = Client.config.server_master
	self.servers = Lobby:download_server_list()
	-- Rebuild the list.
	self.list_servers:clear()
	if self.servers then
		table.insert(self.servers, 1,  {ip = "localhost", port = 10101, name = "Localhost",
			desc = "Connect to a locally hosted server"})
		for k,v in pairs(self.servers) do
			local w = Widgets.Serverinfo(v)
			w.pressed = function(s)
				for k,v in pairs(self.servers) do
					v.widget.active = (v.widget == s)
				end
				self.entry_address.text = s.ip
				self.entry_port.text = s.port
			end
			v.widget = w
			self.list_servers:append{widget = w}
		end
	end
end
