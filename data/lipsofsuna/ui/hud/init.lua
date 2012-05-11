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
		local self = Widgets.Label()
		self.update = function(self, secs)
			local mode = Program.video_mode
			local text = "FPS: " .. tostring(math.floor(Program.fps + 0.5))
			self.offset = Vector(mode[1] - Theme.text_height_1 * 4.5, mode[2] - Theme.text_height_1 - 5)
			self.font = Theme.text_font_1
			self.text = text
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

Ui:add_hud{
	id = "target",
	active = function() return Ui.state == "play" end,
	init = function()
		local self = Widgets.Label{font = "medium"}
		self.update_timer = 0
		self.update = function(self, secs)
			self.update_timer = self.update_timer + secs
			if self.update_timer < 0.1 then return end
			self.update_timer = 0
			local obj = Operators.world:get_target_object()
			if obj and Operators.world:get_target_usages() then
				-- Format the text.
				local key = Binding:get_control_name("menu apply") or "[---]"
				if obj.count and obj.count > 1 then
					self.text = string.format("%s %s (%d)", key, obj.spec.name, obj.count)
				else
					self.text = string.format("%s %s", key, obj.spec.name)
				end
				-- Update the position.
				local mode = Program.video_mode
				local padx = mode[1] - self.width
				self.offset = Vector(padx / 2, Theme.width_icon_1 + Theme.text_height_1 * 2)
				-- Show the widget.
				self.visible = true
			else
				-- Hide the widget.
				self.visible = false
			end
		end
		return self
	end}
