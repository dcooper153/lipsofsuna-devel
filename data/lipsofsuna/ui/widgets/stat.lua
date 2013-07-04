local Class = require("system/class")
local UiWidget = require("ui/widgets/widget")

local UiStat = Class("UiStat", UiWidget)

UiStat.new = function(clss, skill, index)
	local self = UiWidget.new(clss)
	self.skill = skill
	self.index = index
	self.cap = 1
	self.value = 1
	self.timer = 0
	return self
end

UiStat.rebuild_size = function(self)
	return Vector(300, Theme.text_height_1)
end

UiStat.rebuild_canvas = function(self)
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

UiStat.update = function(self, secs)
	-- Call the base class update.
	UiWidget.update(self, secs)
	-- Update the stat values.
	local object = Client.player_object
	if not object then return end
	local stats = object.stats
	if not stats then return end
	local stat = stats:get_skill(self.skill)
	if stat then
		if self.cap ~= stat.maximum or self.value ~= stat.value then
			self.cap = stat.maximum
			self.value = stat.value
			self.timer = 2
			self.need_repaint = true
		end
	end
	-- Keep regenerating stats visible.
	if stat and self.value < stat.maximum then
		self.timer = 2
	end
	self.timer = self.timer - secs
	-- Update the offset and visibility.
	if stat and self.timer > 0 then
		local mode = Program:get_video_mode()
		local pad = mode[1] - self.size.x
		self:set_offset(Vector(pad / 2, mode[2] - self.index * Theme.text_height_1 * 1.2))
		self:set_visible(true)
	else
		self.timer = 0
		self:set_visible(false)
	end
end

return UiStat
