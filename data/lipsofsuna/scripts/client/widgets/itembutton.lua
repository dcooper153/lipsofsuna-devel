Widgets.ItemButton = Class(Group)

Widgets.ItemButton.getter = function(self, key)
	if key == "count" then
		return self.count
	elseif key == "icon" then
		return self.child_image.style
	elseif key == "text" then
		return self.child_text.text
	else
		return Group.getter(self, key)
	end
end

Widgets.ItemButton.setter = function(self, key, value)
	if key == "count" then
		Group.setter(self, "count", value)
		if value > 0 then
			self.child_count.text = tostring(value)
		else
			self.child_count.text = ""
		end
	elseif key == "icon" then
		self.child_image.style = value
	elseif key == "text" then
		self.child_text.text = value or ""
	else
		Group.setter(self, key, value)
	end
end

Widgets.ItemButton.new = function(clss, args)
	local self = Group.new(clss)
	self.style = "button"
	self.rows = 1
	self.child_image = Image{style = "image", pressed = function() self:pressed() end}
	self.child_text = Button{style = "label", pressed = function() self:pressed() end}
	self.child_count = Button{style = "label", pressed = function() self:pressed() end}
	self.child_count:set_request{width = 30}
	self:append_col(self.child_image)
	self:append_col(self.child_text)
	self:append_col(self.child_count)
	self:set_expand{col = 2}
	for k,v in pairs(args or {}) do self[k] = v end
	return self
end

Widgets.ItemButton.pressed = function(self, args)
end
