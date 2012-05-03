if not Settings then return end
if Settings.server then return end

require(Mod.path .. "compass")
require(Mod.path .. "log")
require(Mod.path .. "modifiers")

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
		local self = Widgets.Icon{icon = Iconspec:find{name = "crosshair1"}}
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
	init = function()
		local self = Widgets.Label{valign = 1, request = Vector(60,20)}
		self.update = function(self, secs)
			local mode = Program.video_mode
			self.offset = Vector(mode[1] - 60, mode[2] - 20)
			self.text = "FPS: " .. tostring(math.floor(Program.fps + 0.5))
		end
		return self
	end}

Ui:add_hud{
	id = "modifier",
	active = function() return Ui.root == "play" end,
	init = function()
		return Widgets.Hudmodifiers()
	end}

Ui:add_hud{
	id = "notification",
	active = function() return Ui.root == "play" end,
	init = function()
		return Widgets.Notification()
	end}
