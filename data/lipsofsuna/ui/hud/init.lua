if not Settings then return end
if Settings.server then return end

require(Mod.path .. "compass")
require(Mod.path .. "fps")
require(Mod.path .. "log")
require(Mod.path .. "modifiers")
require(Mod.path .. "notification")
require(Mod.path .. "target")

Ui:add_hud{
	id = "health",
	active = function() return Ui.root == "play" end,
	init = function() return Widgets.Uistat("health", 2) end}

Ui:add_hud{
	id = "willpower",
	active = function() return Ui.root == "play" end,
	init = function() return Widgets.Uistat("willpower", 1) end}

Ui:add_hud{
	id = "chat",
	active = function() return Ui.root == "play" end,
	init = function() return Widgets.Uilog() end}

Ui:add_hud{
	id = "compass",
	active = function() return Ui.root == "play" end,
	init = function() return Widgets.Hudcompass() end}

Ui:add_hud{
	id = "crosshair",
	active = function() return Ui.root == "play" end,
	init = function()
		local self = Widgets.Uiicon(Iconspec:find{name = "crosshair1"})
		self.update = function(self, secs)
			local mode = Program.video_mode
			local padx = mode[1] - self.icon.size[1]
			local pady = mode[2] - self.icon.size[2]
			self.offset = Vector(padx / 2, pady / 2)
		end
		return self
	end}

Ui:add_hud{
	id = "fps",
	active = function() return true end,
	init = function() return Widgets.Hudfps() end}

Ui:add_hud{
	id = "modifier",
	active = function() return Ui.root == "play" end,
	init = function() return Widgets.Hudmodifiers() end}

Ui:add_hud{
	id = "notification",
	active = function() return Ui.root == "play" end,
	init = function() return Widgets.Hudnotification() end}

Ui:add_hud{
	id = "target",
	active = function() return Ui.state == "play" end,
	init = function() return Widgets.Hudtarget() end}
