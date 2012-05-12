Widgets.Hudfps = Class(Widget)
Widgets.Hudfps.class_name = "Widgets.Hudfps"

Widgets.Hudfps.new = function(clss)
	local self = Widget.new(clss)
	return self
end

Widgets.Hudfps.reshaped = function(self)
	local w = self.width
	local h = self.height
	self:canvas_clear()
	Theme:draw_scene_text(self, self.text, 0, 0, w, h, 1, {0, 0.5})
end

Widgets.Hudfps.update = function(self, secs)
	local mode = Program.video_mode
	local text = "FPS: " .. tostring(math.floor(Program.fps + 0.5))
	self.offset = Vector(mode[1] - Theme.text_height_1 * 4.5, mode[2] - Theme.text_height_1 - 5)
	self.font = Theme.text_font_1
	self.text = text
	self:reshaped()
end
