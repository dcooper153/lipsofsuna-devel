Widgets.Scene = Class(Widget)

Widgets.Scene.setter = function(self, key, value)
	if key == "action" then
		if self.action ~= value then
			Widget.setter(self, key, value)
			self:reshaped()
		end
	else
		Widget.setter(self, key, value)
	end
end

Widgets.Scene.new = function(clss, args)
	local self = Widget.new(clss, args)
	self.scene = Scene()
	return self
end

Widgets.Scene.pick = function(self)
	return Target:pick_scene()
end

Widgets.Scene.pressed = function(self)
	Target:select_scene()
end

Widgets.Scene.render = function(self)
	Player.camera.viewport = {self.x, self.y, self.width, self.height}
	self.scene:draw_begin{
		modelview = self.camera.modelview,
		projection = self.camera.projection,
		viewport = self.camera.viewport}
	self.scene:draw_deferred_begin()
	self.scene:draw_deferred_opaque()
	self.scene:draw_deferred_end()
	self.scene:draw_forward_transparent()
	self.scene:draw_post_process{shader = "postprocess-vert-hdr"}
	self.scene:draw_post_process{shader = "postprocess-horz-hdr"}
	self.scene:draw_end()
	Speech:draw{
		modelview = self.camera.modelview,
		projection = self.camera.projection,
		viewport = self.camera.viewport}
end

Widgets.Scene.reshaped = function(self)
	local w = self.width
	local h = self.height
	local o = 100
	self:canvas_clear()
	if self.action then
		self:canvas_text{
			dest_position = {0,h-o},
			dest_size = {w,o},
			text = self.action,
			text_alignment = {0.5,0},
			text_color = {1,1,1,1},
			text_font = "medium"}
	end
	self:canvas_compile()
end
