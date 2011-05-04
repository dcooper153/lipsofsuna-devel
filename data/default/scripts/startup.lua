require "join"
require "host"

Startup = Class()

--- Creates the startup screen.
-- @param clss Startup class.
-- @return Startup.
Startup.new = function(clss)
	self = Class.new(clss)
	self.text = Widgets.Label()
	self.group_host = Host()
	self.group_join = Join()
	-- Menu labels.
	self.button_host = Widgets.Label{font = "mainmenu", text = "Host", pressed = function() self:set_mode("host") end}
	self.button_join = Widgets.Label{font = "mainmenu", text = "Join", pressed = function() self:set_mode("join") end}
	self.button_quit = Widgets.Label{font = "mainmenu", text = "Quit", pressed = function() Program:shutdown() end}
	self.group_buttons = Widget{cols = 5, rows = 1, margins = {bottom = 30}, spacings = {horz = 40}}
	self.group_buttons:set_expand{col = 1}
	self.group_buttons:set_expand{col = 5}
	self.group_buttons:set_child{col = 2, row = 1, widget = self.button_host}
	self.group_buttons:set_child{col = 3, row = 1, widget = self.button_join}
	self.group_buttons:set_child{col = 4, row = 1, widget = self.button_quit}
	-- Main.
	self.group = Widgets.Background{cols = 1, rows = 2, behind = true, fullscreen = true, image = "mainmenu1"}
	self.group:set_expand{col = 1, row = 1}
	self.group:set_child(1, 2, self.group_buttons)
	self.group.floating = true
	Sound.music_fading = 5.0
	Sound.music_volume = 0.2
	Sound.music = "fairytale8"
	return self
end

--- Sets the mina/host/join mode of the edited launcher.
-- @param self Startup.
-- @param mode Mode string ("main"/"host"/"join").
Startup.set_mode = function(self, mode)
	if mode == "main" then
		self.group:set_child(1, 2, self.group_buttons)
	elseif mode == "host" then
		self.group:set_child(1, 2, self.group_host)
	else
		self.group:set_child(1, 2, self.group_join)
		self.group_join:refresh()
	end
end

Startup.inst = Startup()
