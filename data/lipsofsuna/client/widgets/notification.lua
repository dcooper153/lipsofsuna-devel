Widgets.Notification = Class(Widget)
Widgets.Notification.class_name = "Widgets.Notification"

Widgets.Notification.new = function(clss, args)
	local self = Widget.new(clss, args)
	return self
end

Widgets.Notification.reshaped = function(self)
	local w = Theme.text_height_1*10
	self:set_request{
		font = Theme.text_font_1,
		internal = true,
		text = self.text,
		paddings = {7,7,7,7},
		width = w-14}
	self:canvas_clear()
	self:canvas_image{
		dest_position = {0,0},
		dest_size = {w,self.height},
		source_image = "widgets2",
		source_position = {300,0},
		source_tiling = {7,86,7,7,86,7}}
	self:canvas_text{
		dest_position = {7,7},
		dest_size = {w-14,self.height-10},
		text = self.text,
		text_alignment = {0.5,0.5},
		text_color = Theme.text_color_1,
		text_font = Theme.text_font_1}
end

Widgets.Notification.update = function(self, secs)
	if not self.visible then return end
	if self.text and self.timer < 5 then
		-- Show.
		local mode = Program.video_mode
		local off = 1
		if self.timer < 0.5 then off = 2 * self.timer end
		if self.timer > 4.5 then off = 2 * (5 - self.timer) end
		self.offset = Vector(mode[1], mode[2]) - Vector(self.width, self.height * off)
		self.timer = self.timer + secs
	else
		-- Hide.
		self.timer = nil
		self.visible = false
	end
end

Widgets.Notification:add_getters{
	text = function(self) return rawget(self, "__text") end}

Widgets.Notification:add_setters{
	text = function(self, v)
		if self.text == v then return end
		rawset(self, "__text", v)
		self.timer = 0
		self:reshaped()
		self.visible = true
	end}
