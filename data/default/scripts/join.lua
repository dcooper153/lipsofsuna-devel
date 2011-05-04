require "serverinfo"

Join = Class(Widget)

Join.new = function(clss)
	local self = Widget.new(clss, {cols = 1, rows = 3})
	self.text = Widgets.Label()
	-- Server list.
	self.list_servers = Widgets.List{page_size = 7}
	self.list_servers:set_request{height = 255}
	self.button_servers = Widgets.Button{text = "Refresh", pressed = function() self:refresh() end}
	self.title_servers = Widgets.Frame{style = "title", text = "Server list"}
	self.frame_servers = Widgets.Frame{cols = 1, rows = 3}
	self.frame_servers:set_expand{col = 1, row = 1}
	self.frame_servers:set_child(1, 1, self.list_servers)
	self.frame_servers:set_child(1, 2, self.button_servers)
	self.group_servers = Widget{cols = 1, rows = 4, spacings = {0,0}}
	self.group_servers:set_expand{row = 1, col = 1}
	self.group_servers:set_expand{row = 4, col = 3}
	self.group_servers:set_child(1, 2, self.title_servers)
	self.group_servers:set_child(1, 3, self.frame_servers)
	-- Address settings.
	self.entry_address = Widgets.Entry{text = Config.inst.join_address}
	self.entry_port = Widgets.Entry{text = Config.inst.join_port}
	self.entry_port:set_request{width = 50}
	self.group_address = Widget{cols = 2, rows = 1}
	self.group_address:set_expand{col = 1}
	self.group_address:set_child(1, 1, self.entry_address)
	self.group_address:set_child(2, 1, self.entry_port)
	-- Account settings.
	self.entry_account = Widgets.Entry{text = Config.inst.join_account}
	self.entry_password = Widgets.Entry{password = true}
	self.group_account = Widgets.Frame{style = "default", cols = 2, rows = 3}
	self.group_account:set_expand{col = 2}
	self.group_account:set_child{col = 1, row = 1, widget = Widgets.Label{text = "Address:"}}
	self.group_account:set_child{col = 2, row = 1, widget = self.group_address}
	self.group_account:set_child{col = 1, row = 2, widget = Widgets.Label{text = "Account:"}}
	self.group_account:set_child{col = 2, row = 2, widget = self.entry_account}
	self.group_account:set_child{col = 1, row = 3, widget = Widgets.Label{text = "Password:"}}
	self.group_account:set_child{col = 2, row = 3, widget = self.entry_password}
	-- Menu labels.
	self.button_play = Widgets.Label{font = "mainmenu", text = "Join!", pressed = function() self:play() end}
	self.button_back = Widgets.Label{font = "mainmenu", text = "Back", pressed = function() self:back() end}
	self.group_buttons = Widget{cols = 4, rows = 1, margins = {bottom = 30}, spacings = {horz = 40}}
	self.group_buttons:set_expand{col = 1}
	self.group_buttons:set_expand{col = 4}
	self.group_buttons:set_child(2, 1, self.button_play)
	self.group_buttons:set_child(3, 1, self.button_back)
	-- Packing.
	self.group = Widget{cols = 3, rows = 2}
	self.group:set_expand{col = 1, row = 1}
	self.group:set_expand{col = 3}
	self.group:set_child{col = 2, row = 1, widget = self.group_servers}
	self.group:set_child{col = 2, row = 2, widget = self.group_account}
	self:set_expand{col = 1, row = 1}
	self:set_child{col = 1, row = 1, widget = self.text}
	self:set_child{col = 1, row = 2, widget = self.group}
	self:set_child{col = 1, row = 3, widget = self.group_buttons}
	return self
end

--- Returns back to the main menu.
-- @param self Join.
Join.back = function(self)
	Startup.inst:set_mode("main")
end

--- Launches the selected game.
-- @param self Join.
Join.play = function(self)
	local address = self.entry_address.text
	local port = self.entry_port.text
	local account = self.entry_account.text
	local password = self.entry_password.text
	args = string.format("--join %s %d", address, port)
	if account and #account > 0 then
		args = string.format("%s --account %s", args, account)
	end
	if password and #password > 0 then
		args = string.format("%s --password %s", args, password)
	end
	Program:launch_mod{name = "lipsofsuna", args = args}
	Program.quit = true
	Config.inst.join_account = account
	Config.inst.join_address = address
	Config.inst.join_port = port
	Config.inst:save()
end

--- Downloads the server list from the master server.
-- @param self Join.
Join.refresh = function(self)
	-- Download servers.
	Lobby.master = "http://lipsofsuna.org"
	local servers = Lobby:download_server_list()
	-- Rebuild the list.
	self.list_servers:clear()
	if servers then
		table.insert(servers, 1,  {ip = "localhost", port = 10101, name = "Localhost",
			desc = "Connect to a locally hosted server"})
		for k,v in pairs(servers) do
			local w = Widgets.Serverinfo(v)
			w.pressed = function(s)
				self.entry_address.text = s.ip
				self.entry_port.text = s.port
			end
			self.list_servers:append{widget = w}
		end
	end
end
