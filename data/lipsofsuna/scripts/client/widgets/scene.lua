Widgets.Scene = Class(Widget)

Widgets.Scene.setter = function(self, key, value)
	if key == "action" then
		if self.action ~= value then
			Widget.setter(self, key, value)
			self:reshaped()
		end
	elseif key == "compass" then
		if self.compass ~= value then
			Widget.setter(self, key, value)
			self:reshaped()
		end
	elseif key == "compass_quest" then
		if self.compass_quest ~= value then
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
	if not Drag:clicked_scene() then
		Target:select_scene()
	end
end

Widgets.Scene.render = function(self)
	Player.camera.viewport = {self.x, self.y, self.width, self.height}
	self.scene:draw_begin{
		hdr = Views.Options.inst.bloom_enabled,
		modelview = self.camera.modelview,
		multisamples = Views.Options.inst.multisamples,
		projection = self.camera.projection,
		viewport = self.camera.viewport}
	self.scene:draw_pass{pass = 1} -- Depth
	self.scene:draw_pass{pass = 3} -- Forward ambient
	self.scene:draw_pass{pass = 4, lighting = true} -- Forward lighting
	self.scene:draw_pass{pass = 5, sorting = true} -- Transparent ambient
	self.scene:draw_pass{pass = 6, lighting = true, sorting = true} -- Transparent lighting
	if Views.Options.inst.bloom_enabled then
		self.scene:draw_post_process{shader = "postprocess-vert-hdr"}
		self.scene:draw_post_process{shader = "postprocess-horz-hdr"}
	end
	self.scene:draw_end()
	Speech:draw{
		modelview = self.camera.modelview,
		projection = self.camera.projection,
		viewport = self.camera.viewport}
end

Widgets.Scene.reshaped = function(self)
	local w = self.width
	local h = self.height
	self:canvas_clear()
	if self.compass then
		local x = (self.compass + 1) % 1
		local w1 = math.floor(512*(1-x))
		local w2 = math.floor(512*x)
		self:canvas_image{
			dest_position = {(w-512)/2,h-100},
			dest_size = {w1,50},
			source_image = "compass1",
			source_position = {w2,0},
			source_tiling = {0,512,0,0,50,0}}
		self:canvas_image{
			dest_position = {(w-512)/2+w1,h-100},
			dest_size = {w2,50},
			source_image = "compass1",
			source_position = {0,0},
			source_tiling = {0,512,0,0,50,0}}
		if self.compass_quest then
			x = (self.compass_quest - x + 2) % 1
			self:canvas_image{
				dest_position = {(w-512)/2+512*x,h-100},
				dest_size = {11,50},
				source_image = "compass1",
				source_position = {1,50},
				source_tiling = {0,11,0,0,50,0}}
		end
	end
	if self.action then
		local o = 20
		self:canvas_text{
			dest_position = {2,o+2},
			dest_size = {w,o},
			text = self.action,
			text_alignment = {0.5,0},
			text_color = {0,0,0,1},
			text_font = "medium"}
		self:canvas_text{
			dest_position = {0,o},
			dest_size = {w,o},
			text = self.action,
			text_alignment = {0.5,0},
			text_color = {1,1,1,1},
			text_font = "medium"}
	end
	self:canvas_compile()
end
