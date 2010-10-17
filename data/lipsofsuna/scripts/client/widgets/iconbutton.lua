Widgets.IconButton = Class(Group)

Widgets.IconButton.getter = function(self, key)
	if key == "icon" then
		return self.child_image.style
	elseif key == "text" then
		return self.child_text.text
	else
		return Group.getter(self, key)
	end
end

Widgets.IconButton.setter = function(self, key, value)
	if key == "icon" then
		self.child_image.style = value
	elseif key == "text" then
		self.child_text.text = value
	else
		Group.setter(self, key, value)
	end
end

Widgets.IconButton.new = function(clss, args)
	local self = Group.new(clss)
	self.style = "button"
	self.rows = 1
	self.child_image = Image{style = "image", pressed = function() self:pressed() end}
	self.child_text = Button{style = "label", pressed = function() self:pressed() end}
	self:append_col(self.child_image)
	self:append_col(self.child_text)
	for k,v in pairs(args) do self[k] = v end
	return self
end

Widgets.IconButton.pressed = function(self, args)
end
