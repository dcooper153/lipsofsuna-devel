require(Mod.path .. "widget")

Widgets.Uistat = Class(Widgets.Uiwidget)
Widgets.Uistat.class_name = "Widgets.Uistat"

Widgets.Uistat.new = function(clss, skill, index)
	local self = Widgets.Uiwidget.new(clss)
	self.skill = skill
	self.index = index
	self.cap = 1
	self.value = 1
	self.timer = 0
	return self
end

Widgets.Uistat.rebuild_size = function(self)
	return Vector(300, Theme.text_height_1)
end

Widgets.Uistat.rebuild_canvas = function(self)
	local w = self.size.x
	local h = self.size.y
	-- Add the background.
	self:canvas_image{
		dest_position = {0,0},
		dest_size = {w,h},
		source_image = "widgets2",
		source_position = {800,0},
		source_tiling = {7,86,7,7,86,7}}
	-- Make sure that initialized.
	if self.value > 0 and self.cap > 0 then
		-- Add the bar.
		local v = self.value / self.cap
		self:canvas_image{
			dest_clip = {0,0,v*w,h},
			dest_position = {0,0},
			dest_size = {w,h},
			source_image = "widgets2",
			source_position = (self.skill == "health") and {800,100} or {800,200},
			source_tiling = {7,86,7,7,86,7}}
		-- Add the label.
		self:canvas_text{
			dest_position = {0,Theme.text_pad_1},
			dest_size = {w,h},
			text = string.format("%d / %d", self.value, self.cap),
			text_alignment = {0.5,0.5},
			text_color = Theme.text_color_1,
			text_font = Theme.text_font_1}
	end
end

Widgets.Uistat.update = function(self, secs)
	-- Call the base class update.
	Widgets.Uiwidget.update(self, secs)
	-- Update the skill values.
	local object = Client.player_object
	if not object then return end
	local skill = object.stats[self.skill]
	if skill then
		if self.cap ~= skill.cap or self.value ~= skill.value then
			self.cap = skill.cap
			self.value = skill.value
			self.timer = 2
			self.need_repaint = true
		end
	end
	-- Keep regenerating stats visible.
	if skill and self.value < self.cap then
		self.timer = 2
	end
	self.timer = self.timer - secs
	-- Update the offset and visibility.
	if skill and self.timer > 0 then
		local mode = Program.video_mode
		local pad = mode[1] - self.size.x
		self.offset = Vector(pad / 2, mode[2] - self.index * Theme.text_height_1 * 1.2)
		self.visible = true
	else
		self.timer = 0
		self.visible = false
	end
end
