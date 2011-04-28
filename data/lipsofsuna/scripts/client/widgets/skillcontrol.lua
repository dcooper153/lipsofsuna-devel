Widgets.SkillControl = Class(Widget)

Widgets.SkillControl.new = function(clss, args)
	local self = Widget.new(clss, args)
	self.cap = self.cap or 0
	self.max = self.max or 100
	self.text = self.text or ""
	self.value = self.value or 0
	self.init = true
	if args.name then
		self.tooltip = Widgets.Skilltooltip{desc = args.desc, name = args.name}
	end
	return self
end

Widgets.SkillControl.get_value_at = function(self, point)
	local p = point - Vector(self.x, self.y)
	if not self.compact then
		p = p - Vector(100, 0)
		p.x = p.x / 2
	end
	if p.x >= 0 and p.x <= self.max then
		return math.ceil(p.x)
	end
end

Widgets.SkillControl.pressed = function(self)
end

Widgets.SkillControl.reshaped = function(self)
	if not self.init then return end
	local xlbl = 2
	local xbar = self.compact and 0 or 100
	self:set_request{
		font = "tiny",
		internal = true,
		width = xbar + (self.compact and 100 or 200),
		height = 16}
	local w = self.compact and self.max or 2 * self.max
	local h = self.height
	local c = self.cap / self.max
	local v = self.value / self.max
	self:canvas_clear()
	-- TODO: Icon
	self:canvas_image{
		dest_position = {xbar,0},
		dest_size = {w,h},
		source_image = "widgets1",
		source_position = {450,375},
		source_tiling = {34,120,34,6,15,7}}
	if v > 0 then
		self:canvas_image{
			dest_clip = {xbar,0,v*w,h},
			dest_position = {xbar,0},
			dest_size = {w,h},
			source_image = "widgets1",
			source_position = {450,405},
			source_tiling = {34,120,34,6,15,7}}
	end
	if c > v then
		self:canvas_image{
			dest_clip = {xbar+v*w,0,c*w-v*w,h},
			dest_position = {xbar,0},
			dest_size = {w,h},
			source_image = "widgets1",
			source_position = {450,465},
			source_tiling = {34,120,34,6,15,7}}
	end
	if not self.compact and self.text ~= "" then
		self:canvas_text{
			dest_position = {xlbl,0},
			dest_size = {xbar,h},
			text = self.text .. ":",
			text_alignment = {0,0.5},
			text_color = {1,1,1,1},
			text_font = "default"}
	end
	self:canvas_text{
		dest_position = {xbar,0},
		dest_size = {w,h},
		text = tostring(self.value),
		text_alignment = {0.5,0.5},
		text_color = {1,1,1,1},
		text_font = "tiny"}
	self:canvas_compile()
end

Widgets.SkillControl:add_getters{
	cap = function(s) return rawget(s, "__cap") end,
	compact = function(s) return rawget(s, "__compact") end,
	icon = function(s) return rawget(s, "__icon") end,
	max = function(s) return rawget(s, "__max") end,
	text = function(s) return rawget(s, "__text") end,
	value = function(s) return rawget(s, "__value") end}

Widgets.SkillControl:add_setters{
	cap = function(s, v)
		if s.cap == v then return end
		rawset(s, "__cap", v)
		s:reshaped()
	end,
	compact = function(s, v)
		if s.compact == v then return end
		rawset(s, "__compact", v)
		s:reshaped()
	end,
	icon = function(s, v)
		if s.icon == v then return end
		rawset(s, "__icon", v)
		s:reshaped()
	end,
	max = function(s, v)
		if s.max == v then return end
		rawset(s, "__max", v)
		s:reshaped()
	end,
	text = function(s, v)
		if s.text == v then return end
		rawset(s, "__text", v)
		s:reshaped()
	end,
	value = function(s, v)
		if s.value == v then return end
		rawset(s, "__value", v)
		s:reshaped()
	end}
