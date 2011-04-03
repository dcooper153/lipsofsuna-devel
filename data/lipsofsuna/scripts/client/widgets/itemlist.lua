Widgets.ItemList = Class(Widget)

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

Widgets.ItemList:add_getters{
	size = function(s) return rawget(s, "__size") end}

Widgets.ItemList:add_setters{
	size = function(s, v)
		rawset(s, "__size", v)
		local rows = math.ceil(v / 5)
		s.cols = 5
		s.rows = rows
		s.buttons = {}
		for i = 1,v do
			s.buttons[i] = Widgets.ItemButton{pressed = function(w, a) s:activated(i, a) end}
			s:set_child(
				1 + math.floor((i - 1) / rows),
				1 + (i - 1) % rows,
				s.buttons[i])
		end
	end}
