Widgets.Uilog = Class(Widget)
Widgets.Uilog.class_name = "Widgets.Uilog"

Widgets.Uilog.new = function(clss, args)
	local self = Widget.new(clss, args)
	self.lines = {}
	return self
end

Widgets.Uilog.append = function(self, text)
	table.insert(self.lines, 1, {text, 0, 1})
	self.need_relayout = true
end

Widgets.Uilog.reshaped = function(self)
	self:canvas_clear()
	local w = self.width
	local h = self.height
	local y = 200
	for k,v in ipairs(self.lines) do
		local tw,th = Program:measure_text(Theme.text_font_1, v[1], w)
		y = y - th
		Theme:draw_scene_text(self, v[1], 0, y, w, th, v[3], {0.5,0})
	end
end

Widgets.Uilog.update = function(self, secs)
	-- Check for a relayout.
	local changed = self.need_relayout
	self.need_relayout = true
	-- Update the screen offset.
	local mode = Program.video_mode
	if self.width ~= mode[1] or self.offset.y ~= mode[2] - 200 then
		self.offset = Vector(5, mode[2] - Theme.text_height_1 * 3 - 200)
		self:set_request{width = mode[1], height = 200}
		changed = true
	end
	-- Fade out messages.
	local i = 1
	while self.lines[i] do
		local line = self.lines[i]
		line[2] = line[2] + secs * math.exp((i-1)*0.3)
		if line[2] < 6 then
			if line[2] > 5 then
				line[3] = 6 - line[2]
				changed = true
			end
			i = i + 1
		else
			table.remove(self.lines, i)
			changed = true
		end
	end
	-- Update the display.
	if changed then
		self:reshaped()
	end
end
