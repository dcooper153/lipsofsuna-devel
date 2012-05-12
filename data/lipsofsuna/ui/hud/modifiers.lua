Widgets.Hudmodifiers = Class(Widget)
Widgets.Hudmodifiers.class_name = "Widgets.Hudmodifiers"

--- Creates a new modifiers widget.
-- @param clss Modifiers class.
-- @return Modifiers widget.
Widgets.Hudmodifiers.new = function(clss)
	local self = Widget.new(clss)
	self.dict_id = {}
	self.dict_name = {}
	clss.inst = self
	return self
end

-- Adds a new modifier.
-- @param self Modifiers widget.
-- @param name Modifier name.
-- @param time Modifier duration.
Widgets.Hudmodifiers.add = function(self, name, time)
	-- Special handling for light and plague.
	if name == "light" then Lighting:set_light_spell(true) end
	if name == "black haze" and not self.dict_name[name] then Effect:play("disease1") end
	-- Add the icon.
	local icon = self.dict_name[name]
	if icon then
		icon.time = time
	else
		local id = #self.dict_id + 1
		local effect = Feateffectspec:find{name = name}
		local icon_name = effect and effect.icon or "missing1"
		local icon_table = {id = id, icon = icon_name, name = name, time = time}
		self.dict_id[id] = icon_table
		self.dict_name[name] = icon_table
	end
end

--- Removes a modifier.
-- @param self Modifiers widget.
-- @param name Modifier name.
Widgets.Hudmodifiers.remove = function(self, name)
	local icon = self.dict_name[name]
	if not icon then return end
	-- Remove from dictionaries.
	self.dict_name[icon.name] = nil
	table.remove(self.dict_id, icon.id)
	-- Update indices of modifiers after the removed index.
	for id = icon.id,#self.dict_id do
		self.dict_id[id].id = id
	end
	-- Special handling for light.
	if name == "light" then Lighting:set_light_spell(false) end
end

Widgets.Hudmodifiers.reshaped = function(self)
	local x = 0
	local w = Theme.width_icon_1
	local h = Theme.width_icon_1 + Theme.text_height_1
	self:canvas_clear()
	for k,v in ipairs(self.dict_id) do
		Theme:draw_icon_scaled(self, v.icon, x, 0, Theme.width_icon_1, Theme.width_icon_1)
		if v.time_coarse then
			self:canvas_text{
				dest_position = {x+2,2},
				dest_size = {w,h},
				text = tostring(v.time_coarse),
				text_alignment = {0.5,1},
				text_color = Theme.text_color_1,
				text_font = Theme.text_font_1}
			self:canvas_text{
				dest_position = {x+0.5,0},
				dest_size = {w,h},
				text = tostring(v.time_coarse),
				text_alignment = {0.5,1},
				text_color = Theme.text_color_3,
				text_font = Theme.text_font_1}
		end
		x = x + w
	end
end

--- Updates the modifiers.
-- @param self Modifiers widget.
-- @param secs Seconds since the last update.
Widgets.Hudmodifiers.update = function(self, secs)
	local len = #self.dict_id
	if len == 0 then
		self.visible = false
	else
		local mode = Program.video_mode
		local width = mode[1] - len * Theme.width_icon_1
		self.offset = Vector(width / 2, 5)
		self.visible = true
		for k,v in pairs(self.dict_id) do
			if v.time < 10000 then
				v.time = math.max(0, v.time - secs)
				v.time_coarse = math.ceil((v.time + 0.1) / 5) * 5
			else
				v.time_coarse = nil
			end
		end
		self:reshaped()
	end
end
