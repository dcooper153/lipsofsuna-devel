Widgets.MenuItem = Class(Group)

Widgets.MenuItem.getter = function(self, key)
	if key == "arrow" then
		return self.child_arrow.text == ">"
	elseif key == "icon" then
		return self.child_image.style
	elseif key == "text" then
		return self.child_text.text
	else
		return Group.getter(self, key)
	end
end

Widgets.MenuItem.setter = function(self, key, value)
	if key == "arrow" then
		self.child_arrow.text = value and ">" or "   "
	elseif key == "icon" then
		self.child_image.style = value
	elseif key == "text" then
		self.child_text.text = value
	else
		Group.setter(self, key, value)
	end
end

Widgets.MenuItem.new = function(clss, args)
	local self = Group.new(clss)
	self.style = "menuitem"
	self.rows = 1
	self.child_image = Image{style = "menuitem-icon", pressed = function() self:pressed() end}
	self.child_text = Button{style = "menuitem-label", pressed = function() self:pressed() end}
	self.child_arrow = Button{style = "menuitem-arrow", pressed = function() self:pressed() end}
	self.child_arrow.text = "   "
	self:append_col(self.child_image)
	self:append_col(self.child_text)
	self:append_col(self.child_arrow)
	self:set_expand{col = 2}
	for k,v in pairs(args) do self[k] = v end
	return self
end

Widgets.MenuItem.pressed = function(self, args)
end
