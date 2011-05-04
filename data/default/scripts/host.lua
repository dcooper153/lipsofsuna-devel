Host = Class(Widget)

Host.new = function(clss)
	local self = Widget.new(clss, {cols = 1, rows = 3})
	self.text = Widgets.Label()
	-- Save slot settings.
	self.combo_file = Widgets.ComboBox({
		{"Save #1", function() self:set_slot(1) end},
		{"Save #2", function() self:set_slot(2) end},
		{"Save #3", function() self:set_slot(3) end},
		{"Save #4", function() self:set_slot(4) end},
		{"Save #5", function() self:set_slot(5) end}})
	self.combo_file:activate{index = Config.inst.host_saveslot}
	-- Account settings.
	self.entry_account = Widgets.Entry{text = Config.inst.host_account}
	self.entry_password = Widgets.Entry{password = true}
	self.group_account = Widgets.Frame{style = "default", cols = 2, rows = 3}
	self.group_account:set_expand{col = 2}
	self.group_account:set_child{col = 1, row = 1, widget = Widgets.Label{text = "Savefile:"}}
	self.group_account:set_child{col = 2, row = 1, widget = self.combo_file}
	self.group_account:set_child{col = 1, row = 2, widget = Widgets.Label{text = "Account:"}}
	self.group_account:set_child{col = 2, row = 2, widget = self.entry_account}
	self.group_account:set_child{col = 1, row = 3, widget = Widgets.Label{text = "Password:"}}
	self.group_account:set_child{col = 2, row = 3, widget = self.entry_password}
	-- Menu labels.
	self.button_play = Widgets.Label{font = "mainmenu", text = "Host!", pressed = function() self:play() end}
	self.button_back = Widgets.Label{font = "mainmenu", text = "Back", pressed = function() self:back() end}
	self.group_buttons = Widget{cols = 4, rows = 1, margins = {bottom = 30}, spacings = {horz = 40}}
	self.group_buttons:set_expand{col = 1}
	self.group_buttons:set_expand{col = 4}
	self.group_buttons:set_child(2, 1, self.button_play)
	self.group_buttons:set_child(3, 1, self.button_back)
	-- Packing.
	self.group = Widget{cols = 3, rows = 1}
	self.group:set_expand{col = 1, row = 1}
	self.group:set_expand{col = 3}
	self.group:set_child{col = 2, row = 1, widget = self.group_account}
	self:set_expand{col = 1, row = 1}
	self:set_child{col = 1, row = 1, widget = self.text}
	self:set_child{col = 1, row = 2, widget = self.group}
	self:set_child{col = 1, row = 3, widget = self.group_buttons}
	return self
end

--- Returns back to the main menu.
-- @param self Join.
Host.back = function(self)
	Startup.inst:set_mode("main")
end

--- Launches the game.
-- @param self Join.
Host.play = function(self)
	local saveslot = self.combo_file.value or 1
	local account = self.entry_account.text
	local password = self.entry_password.text
	args = string.format("--host localhost %d --file %d", 10101, saveslot)
	if account and #account > 0 then
		args = string.format("%s --account %s", args, account)
	end
	if password and #password > 0 then
		args = string.format("%s --password %s", args, password)
	end
	Program:launch_mod{name = "lipsofsuna", args = args}
	Program.quit = true
	Config.inst.host_saveslot = saveslot
	Config.inst.host_account = account
	Config.inst:save()
end

---- Sets the active saveslot.
--- @param self Startup.
--- @param slot Saveslot number.
Host.set_slot = function(self, slot)
end
