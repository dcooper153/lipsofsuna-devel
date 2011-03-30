Widgets.ItemList = Class(Widget)

Widgets.ItemList.setter = function(self, key, value)
	if key == "size" then
		Widget.setter(self, key, value)
		local rows = math.ceil(value / 5)
		self.cols = 5
		self.rows = rows
		self.buttons = {}
		for i = 1,value do
			self.buttons[i] = Widgets.ItemButton{pressed = function(w, a) self:activated(i, a) end}
			self:set_child{
				col = 1 + math.floor((i - 1) / rows),
				row = 1 + (i - 1) % rows,
				widget = self.buttons[i]}
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

Widgets.ItemList.activated = function(self, row, args)
end

Widgets.ItemList.get_item = function(self, args)
	local widget = self.buttons[args.slot]
	if not widget then return end
	if not widget.text then return end
	return widget
end

Widgets.ItemList.set_item = function(self, args)
	local widget = self.buttons[args.slot]
	if not widget then return end
	widget.text = args.name
	widget.count = args.count
	widget.icon = args.icon
end
