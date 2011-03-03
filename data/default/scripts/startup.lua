Startup = Class()

--- Creates the startup screen.
-- @param clss Startup class.
-- @return Startup.
Startup.new = function(clss)
	self = Class.new(clss)
	self.text = Widgets.Label()
	self.button_play = Widgets.Label{font = "mainmenu", text = "Play", pressed = function() self:play() end}
	self.button_quit = Widgets.Label{font = "mainmenu", text = "Quit", pressed = function() Program:shutdown() end}
	-- Load launchers.
	self.db = Database{name = "client.sqlite"}
	self.db:query("CREATE TABLE IF NOT EXISTS launchers (id INTEGER PRIMARY KEY,mode TEXT,mod TEXT,server TEXT,port INTEGER,file INTEGER,account TEXT);")
	self.launchers = {}
	for k,v in ipairs(self.db:query("SELECT * FROM launchers;")) do
		self.launchers[v[1]] = Launcher{mode = v[2], mod = v[3], server = v[4], port = tonumber(v[5]), file = tonumber(v[6]), account = v[7]}
	end
	if #self.launchers == 0 then
		self.launchers = {Launcher{mode = "host"}, Launcher{mode = "join"},
			Launcher{mode = "join", server = "h.lipsofsuna.org"}}
	end
	-- Launcher editor.
	self.combo_mode = Widgets.ComboBox({
		{"Host", function() self:set_mode("host") end},
		{"Join", function() self:set_mode("join") end}})
	self.combo_file = Widgets.ComboBox({
		{"Save #1", function() self:set_slot(1) end},
		{"Save #2", function() self:set_slot(2) end},
		{"Save #3", function() self:set_slot(3) end},
		{"Save #4", function() self:set_slot(4) end},
		{"Save #5", function() self:set_slot(5) end}})
	self.label_mode = Widgets.Label{text = "Mode:"}
	self.label_mode:set_request{width = 40}
	self.label_server = Widgets.Label{text = "Server:"}
	self.label_file = Widgets.Label{text = "File:"}
	self.entry_server = Widgets.Entry()
	self.entry_game = Widgets.Entry()
	self.entry_account = Widgets.Entry()
	self.title_editor = Widgets.Frame{style = "title", text = "Edit"}
	self.button_save = Widgets.Button{text = "Save", pressed = function() self:save() end}
	self.group_editor1 = Widget{cols = 2, rows = 4}
	self.group_editor1:set_expand{col = 2}
	self.group_editor1:set_child{col = 1, row = 1, widget = Widgets.Label{text = "Mod:"}}
	self.group_editor1:set_child{col = 2, row = 1, widget = self.entry_game}
	self.group_editor1:set_child{col = 1, row = 2, widget = self.label_mode}
	self.group_editor1:set_child{col = 2, row = 2, widget = self.combo_mode}
	self.group_editor1:set_child{col = 1, row = 3, widget = self.label_file}
	self.group_editor1:set_child{col = 2, row = 3, widget = self.combo_file}
	self.group_editor1:set_child{col = 1, row = 4, widget = self.label_server}
	self.group_editor1:set_child{col = 2, row = 4, widget = self.entry_server}
	self.frame_editor = Widgets.Frame{cols = 1, rows = 3}
	self.frame_editor:set_expand{col = 1, row = 2}
	self.frame_editor:set_child{col = 1, row = 1, widget = self.group_editor1}
	self.frame_editor:set_child{col = 1, row = 3, widget = self.button_save}
	self.group_editor = Widget{cols = 3, rows = 4, spacings = {0,0}, visible = false}
	self.group_editor:set_expand{row = 1, col = 1}
	self.group_editor:set_expand{row = 4, col = 3}
	self.group_editor:set_child{col = 2, row = 2, widget = self.title_editor}
	self.group_editor:set_child{col = 2, row = 3, widget = self.frame_editor}
	-- Menu labels.
	self.group_buttons = Widget{cols = 4, rows = 1, margins = {bottom = 30}, spacings = {horz = 40}}
	self.group_buttons:set_expand{col = 1}
	self.group_buttons:set_expand{col = 4}
	self.group_buttons:set_child{col = 2, row = 1, widget = self.button_play}
	self.group_buttons:set_child{col = 3, row = 1, widget = self.button_quit}
	-- Launcher selector.
	self.combo = Widgets.ComboBox()
	self.combo:set_request{width = 250}
	self:update_launchers()
	self.combo:activate{index = 1}
	self.group_selector = Widget{cols = 2, rows = 1, spacings = {0,0}}
	self.group_selector:set_expand{col = 1}
	self.group_selector:set_child{col = 1, row = 1, widget = self.combo}
	self.group_selector:set_child{col = 2, row = 1, widget = Widgets.Button{text = " E",
		pressed = function() self.group_editor.visible = not self.group_editor.visible end}}
	-- Account settings.
	self.entry_password = Widgets.Entry{password = true}
	self.group_account = Widget{cols = 2, rows = 3}
	self.group_account:set_expand{col = 2}
	self.group_account:set_child{col = 1, row = 1, widget = Widgets.Label{text = "Game:"}}
	self.group_account:set_child{col = 2, row = 1, widget = self.group_selector}
	self.group_account:set_child{col = 1, row = 2, widget = Widgets.Label{text = "Account:"}}
	self.group_account:set_child{col = 2, row = 2, widget = self.entry_account}
	self.group_account:set_child{col = 1, row = 3, widget = Widgets.Label{text = "Password:"}}
	self.group_account:set_child{col = 2, row = 3, widget = self.entry_password}
	-- Bottom row.
	self.group3 = Widget{cols = 3, rows = 3}
	self.group3:set_expand{col = 1, row = 1}
	self.group3:set_expand{col = 3}
	self.group3:set_child{col = 2, row = 1, widget = self.text}
	self.group3:set_child{col = 2, row = 2, widget = self.group_account}
	self.group3:set_child{col = 2, row = 3, widget = self.group_buttons}
	-- Main.
	self.group = Widgets.Background{cols = 1, rows = 2, behind = true, fullscreen = true, image = "mainmenu1"}
	self.group:set_expand{col = 1, row = 1}
	self.group:set_child{col = 1, row = 1, widget = self.group_editor}
	self.group:set_child{col = 1, row = 2, widget = self.group3}
	self.group.floating = true
	Sound.music_fading = 5.0
	Sound.music_volume = 0.2
	Sound.music = "fairytale8"
end

--- Launches the selected game.
-- @param self Startup.
Startup.play = function(self)
	if self.launcher then
		self.launcher.account = self.entry_account.text
		self:save()
		self.launcher:execute(self.entry_account.text, self.entry_password.text)
	end
end

--- Saves the current state to the database.
-- @param self Startup.
Startup.save = function(self)
	-- Update the edited launcher.
	local launcher
	if self.creating then
		launcher = Launcher()
	else
		launcher = self.launcher
	end
	launcher.mode = (self.combo_mode.text == "Host" and "host" or "join")
	launcher.file = self.combo_file.value
	launcher.mod = self.entry_game.text
	launcher.server = self.entry_server.text
	if creating then
		table.insert(self.launchers, launcher)
		self:update_launchers()
		self.combo:activate{index = #self.launchers}
		self.creating = nil
	else
		local index = self.combo.value
		self:update_launchers()
		self.combo:activate{index = index}
	end
	self.group_editor.visible = false
	-- Save launchers.
	self.db:query("BEGIN TRANSACTION;")
	self.db:query("DELETE FROM launchers;")
	for k,v in ipairs(self.launchers) do
		self.db:query("REPLACE INTO launchers (id,mode,mod,server,port,file,account) VALUES (?,?,?,?,?,?,?);",
			{k, v.mode, v.mod, v.server, v.port, v.file, v.account})
	end
	self.db:query("END TRANSACTION;")
end

--- Sets the active launcher.
-- @param self Startup.
-- @param launcher Launcher.
Startup.set_launcher = function(self, launcher)
	self.launcher = launcher
	if launcher.mode == "host" then
		self.combo_mode:activate{index = 1}
	else
		self.combo_mode:activate{index = 2}
	end
	self:set_mode(launcher.mode)
	self.combo_file:activate{index = launcher.file}
	self.entry_game.text = launcher.mod
	self.entry_server.text = launcher.server
	self.entry_account.text = launcher.account
end

--- Sets the host/join mode of the edited launcher.
-- @param self Startup.
-- @param mode Mode string ("host"/"join").
Startup.set_mode = function(self, mode)
	if mode == "host" then
		self.label_file.visible = true
		self.combo_file.visible = true
		self.label_server.visible = false
		self.entry_server.visible = false
	else
		self.label_file.visible = false
		self.combo_file.visible = false
		self.label_server.visible = true
		self.entry_server.visible = true
	end
end

--- Sets the saveslot of the edited launcher.
-- @param self Startup.
-- @param slot Saveslot number.
Startup.set_slot = function(self, slot)
end

--- Updates the launcher list.
-- @param self Startup.
Startup.update_launchers = function(self)
	local items = {}
	for k,v in ipairs(self.launchers) do
		table.insert(items, {v:get_description(), function() self:set_launcher(v) end})
	end
	self.combo:set_items(items)
end

Startup.inst = Startup()
