Widgets.Hudmodifiers = Class(Widget)
Widgets.Hudmodifiers.class_name = "Widgets.Hudmodifiers"

--- Creates a new modifiers widget.
-- @param clss Modifiers class.
-- @return Modifiers widget.
Widgets.Hudmodifiers.new = function(clss)
	local self = Widget.new(clss, {rows = 1})
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
		local id = self.cols + 1
		local effect = Feateffectspec:find{name = name}
		local icon_name = effect and effect.icon or "missing1"
		icon = Widgets.Modifier{id = id, icon = Iconspec:find{name = icon_name}, name = name, time = time}
		self.dict_id[id] = icon
		self.dict_name[name] = icon
		self:append_col(icon)
	end
end

--- Removes a modifier.
-- @param self Modifiers widget.
-- @param name Modifier name.
Widgets.Hudmodifiers.remove = function(self, name)
	local icon = self.dict_name[name]
	if not icon then return end
	-- Remove from dictionaries.
	Widget.remove(self, {col = icon.id})
	self.dict_name[icon.name] = nil
	table.remove(self.dict_id, icon.id)
	-- Update indices of modifiers after the removed index.
	for id = icon.id,#self.dict_id do
		self.dict_id[id].id = id
	end
	-- Special handling for light.
	if name == "light" then Lighting:set_light_spell(false) end
end

--- Updates the modifiers.
-- @param self Modifiers widget.
-- @param secs Seconds since the last update.
Widgets.Hudmodifiers.update = function(self, secs)
	local mode = Program.video_mode
	self.offset = Vector((mode[1] - self.width) / 2, 5)
	for k,v in pairs(self.dict_id) do
		if v.time < 10000 then
			v.time = math.max(0, v.time - secs)
		end
	end
end
