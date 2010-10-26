require "client/widgets/button"
require "client/widgets/popup"

Widgets.ComboBox = Class(Widgets.Button)

Widgets.ComboBox.new = function(clss, args)
	local self = Widgets.Button.new(clss)
	self.value = 1
	self.menu = Widgets.Popup{cols = 1, temporary = true}
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
	local widget = Widgets.MenuItem{text = args.text, pressed = function()
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
	self.menu:set_request{width = w.width - 4}
	self.menu:popup{x = w.x, y = w.y, width = w.width, height = w.height, dir = "down"}
end

Widgets.ComboBox.reshaped = function(self)
	self:set_request{
		font = self.font,
		internal = true,
		paddings = {2,2,2,2},
		text = self.text}
	local w = self.width
	local h = self.height
	self:canvas_clear()
	self:canvas_image{
		dest_position = {0,0},
		dest_size = {w,h},
		source_image = "widgets1",
		source_position = self.focused and {64,0} or {0,0},
		source_tiling = {6,52,6,6,52,6}}
	self:canvas_text{
		dest_position = {3,0},
		dest_size = {w,h},
		text = self.text,
		text_alignment = {0,0.5},
		text_color = self.focused and {1,1,0,1} or {1,1,1,1},
		text_font = self.font}
	self:canvas_compile()
end
