Widgets.SkillControl = Class(Group)

Widgets.SkillControl.getter = function(self, key)
	if key == "cap" then
		return self.child_scroll.reference
	elseif key == "icon" then
		return self.child_image.style
	elseif key == "text" then
		return self.child_text.text
	elseif key == "value" then
		return self.child_scroll.value
	else
		return Group.getter(self, key)
	end
end

Widgets.SkillControl.setter = function(self, key, value)
	if key == "cap" then
		self.child_scroll.reference = value
	elseif key == "icon" then
		self.child_image.style = value
	elseif key == "max" then
		Group.setter(self, key, value)
		self.child_scroll:set_range{min = 0, max = self.max}
		self.child_scroll:set_request{width = self.max}
	elseif key == "text" then
		self.child_text.text = value
	elseif key == "value" then
		self.child_scroll.value = value
	else
		Group.setter(self, key, value)
	end
end

Widgets.SkillControl.new = function(clss, args)
	local self = Group.new(clss, {rows = 1})
	self.child_image = Image{style = "image", pressed = function() self:pressed() end}
	self.child_image:set_request{width = 32}
	self.child_text = Button{style = "label", pressed = function() self:pressed() end}
	self.child_text:set_request{width = 80}
	self.child_scroll = Scroll{reference = 100, pressed = function() self:pressed() end}
	self.child_scroll:set_range{min = 0, max = 100}
	self:append_col(self.child_image)
	self:append_col(self.child_text)
	self:append_col(self.child_scroll)
	self.spacings = {5, 5}
	self.max = 100
	for k,v in pairs(args) do self[k] = v end
	return self
end

Widgets.SkillControl.pressed = function(self)
end
