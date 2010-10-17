Widgets.ComboBox = Class(Button)

Widgets.ComboBox.new = function(clss, args)
	local self = Button.new(clss)
	self.value = 1
	self.menu = Group{cols = 1, style = "combobox-menu", temporary = true}
	self.menu:set_expand{col = 1}
	-- Copy arguments.
	for k,v in pairs(args or {}) do
		if type(k) ~= "number" then
			self[k] = v
		end
	end
	-- Create menu items.
	for k,v in ipairs(args or {}) do
		if type(v) == "table" then
			self:append{text = v[1], pressed = v[2]}
		elseif type(v) == "string" then
			self:append{text = v}
		end
	end
	return self
end

Widgets.ComboBox.append = function(self, args)
	local row = self.menu.rows + 1
	local widget = Button{text = args.text, style = "combobox-menuitem", pressed = function()
		self.text = args.text
		self.value = row
		self.menu.visible = false
		if args.pressed then args.pressed(self, row) end
	end}
	self.menu:append_row(widget)
end

Widgets.ComboBox.activate = function(self, args)
	local widget = self.menu:get_child{col = 1, row = args.index}
	if not widget then return end
	widget:pressed()
end

Widgets.ComboBox.pressed = function(self)
	local w = self
	self.menu:set_request{width = w.width - 10}
	self.menu:popup{x = w.x, y = w.y, width = w.width, height = w.height, dir = "down"}
end
