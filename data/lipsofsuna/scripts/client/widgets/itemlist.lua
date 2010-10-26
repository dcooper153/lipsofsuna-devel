Widgets.ItemList = Class(Widget)

Widgets.ItemList.getter = function(self, key)
	if key == "size" then
		return self.size
	else
		return Widget.getter(self, key)
	end
end

Widgets.ItemList.setter = function(self, key, value)
	if key == "size" then
		self.rows = value
		self.buttons = {}
		for i = 1,value do
			self.buttons[i] = Widgets.ItemButton{pressed = function() self:pressed(i) end}
			self:set_child{col = 1, row = i, widget = self.buttons[i]}
		end
	else
		Widget.setter(self, key, value)
	end
end

Widgets.ItemList.new = function(clss, args)
	local self = Widget.new(clss)
	self.cols = 1
	self.rows = 0
	self.spacings = {0, 0}
	for k,v in pairs(args) do self[k] = v end
	return self
end

Widgets.ItemList.set_item = function(self, args)
	local widget = self.buttons[args.slot]
	if not widget then return end
	widget.text = args.name
	widget.count = 0 -- FIXME
	widget.icon = "item-icon-none" -- FIXME
end

Widgets.ItemList.pressed = function(self, row)
end
