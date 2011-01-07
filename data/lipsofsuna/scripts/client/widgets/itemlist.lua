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
		if value <= 10 then
			-- Small inventories are single column.
			self.cols = 1
			self.rows = value
			self.buttons = {}
			for i = 1,value do
				self.buttons[i] = Widgets.ItemButton{pressed = function() self:pressed(i) end}
				self:set_child{col = 1, row = i, widget = self.buttons[i]}
			end
		else
			-- Large inventories are dual column.
			local rows = math.ceil(value / 2)
			self.cols = 2
			self.rows = rows
			self.buttons = {}
			for i = 1,value do
				self.buttons[i] = Widgets.ItemButton{pressed = function() self:pressed(i) end}
				self:set_child{
					col = 1 + math.floor((i - 1) / rows),
					row = 1 + (i - 1) % rows,
					widget = self.buttons[i]}
			end
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
	widget.count = args.count
	widget.icon = args.icon
end

Widgets.ItemList.pressed = function(self, row)
end
