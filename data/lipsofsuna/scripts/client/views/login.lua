Views.Login = Class(Widgets.Background)

--- Creates a login view.
-- @param clss Login view class.
-- @return Login view.
Views.Login.new = function(clss)
	local self = Widgets.Background.new(clss, {cols = 1, rows = 2, behind = true, fullscreen = true, image = "mainmenu1"})
	self.label_hint = Widgets.Label{halign = 0.01, valign = 0.01, font = "medium"}
	self.text = Widgets.Label()
	-- Menu labels.
	self.button_host = Widgets.Label{font = "mainmenu", text = "Host", pressed = function() Client:set_mode("host") end}
	self.button_join = Widgets.Label{font = "mainmenu", text = "Join", pressed = function() Client:set_mode("join") end}
	self.button_quit = Widgets.Label{font = "mainmenu", text = "Quit", pressed = function() Program:shutdown() end}
	self.group_buttons = Widget{cols = 5, rows = 1, margins = {bottom = 30}, spacings = {horz = 40}}
	self.group_buttons:set_expand{col = 1}
	self.group_buttons:set_expand{col = 5}
	self.group_buttons:set_child{col = 2, row = 1, widget = self.button_host}
	self.group_buttons:set_child{col = 3, row = 1, widget = self.button_join}
	self.group_buttons:set_child{col = 4, row = 1, widget = self.button_quit}
	-- Main.
	self:set_expand{col = 1, row = 1}
	self:set_child(1, 2, self.group_buttons)
	self:set_child(1, 1, self.label_hint)
	return self
end

--- Exits the game.
-- @param self Login view.
Views.Login.back = function(self)
	Program.quit = true
end

--- Closes the login view.
-- @param self Startup view.
Views.Login.close = function(self)
	self.floating = false
end

--- Initializes and shows the login view.
-- @param self Login view.
-- @param from Name of the previous mode.
Views.Login.enter = function(self, from)
	-- Cleanup if disconnected.
	if from then
		for k,v in pairs(Object.objects) do v:detach() end
		Player.object = nil
		Gui.main.floating = false
	end
	if from ~= "join" and from ~= "host" then
		Sound:switch_music_track("menu")
	end
	-- Show the UI.
	self.floating = true
	-- Show controls.
	self.label_hint.text = [[Default Key map:
e = interact
space = jump
c = climb
y = swap between first and third person
tab = swap quickbar mode
esc = game menu
You can reconfigure controls in the game menu
]]
end
