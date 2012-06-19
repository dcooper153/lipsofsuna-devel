Unittest:add(2, "widget", function()
	require "system/graphics"
	require "system/widget"
	require "system/widgets"
	-- Initialization.
	local w = Widget{cols = 1, rows = 1}
	assert(w.cols == 1)
	assert(w.rows == 1)
	-- Getters.
	assert(type(w.height) == "number")
	assert(type(w.width) == "number")
	-- Child packing.
	local c = Widget()
	w:set_child(1, 1, c)
	collectgarbage()
	assert(w:get_child(1, 1) == c)
	assert(Los.widget_get_child(w.handle, 1, 1) == c.handle)
	local c1 = Widget()
	local c2 = Widget()
	local c3 = Widget()
	w:append_col(c1)
	w:append_row(c2, c3)
	collectgarbage()
	assert(w:get_child(2, 1) == c1)
	assert(w:get_child(1, 2) == c2)
	assert(w:get_child(2, 2) == c3)
	-- Row and column removal.
	w:remove{col = 1}
	assert(w.cols == 1)
	assert(w:get_child(1, 1) == c1)
	assert(w:get_child(1, 2) == c3)
	w:remove{col = 1}
	assert(w.cols == 0)
	w:remove{row = 1}
	assert(w.rows == 1)
	w:remove{row = 1}
	assert(w.rows == 0)
	-- Shrinking.
	do
		local w1 = Widget{cols = 2, rows = 2}
		local weak = setmetatable({}, {__mode = "kv"})
		do
			weak[1] = Widget()
			weak[2] = Widget()
			weak[3] = Widget()
			weak[4] = Widget()
			w1:set_child(1, 1, weak[1])
			w1:set_child(2, 1, weak[2])
			w1:set_child(1, 2, weak[3])
			w1:set_child(2, 2, weak[4])
			collectgarbage()
			assert(weak[1])
		end
		w1.rows = 1
		assert(w1:get_child(1, 1))
		assert(w1:get_child(2, 1))
		assert(w1:get_child(1, 2) == nil)
		assert(w1:get_child(2, 2) == nil)
		collectgarbage()
		assert(weak[1])
		assert(weak[2])
		assert(weak[3] == nil)
		assert(weak[4] == nil)
		w1.cols = 1
		assert(w1:get_child(1, 1))
		assert(w1:get_child(2, 2) == nil)
		collectgarbage()
		assert(weak[1])
		assert(weak[2] == nil)
		w1:remove{col = 1}
		assert(w1.cols == 0)
		assert(w1:get_child(1, 1) == nil)
		collectgarbage()
		assert(weak[1] == nil)
	end
	-- Visibility.
	assert(w.visible)
	assert(not w.floating)
	w.floating = true
	assert(w.floating)
	assert(w.visible)
	w.visible = false
	assert(not w.floating)
	assert(not w.visible)
	-- Inheritance of new classes.
	local Label = Class(Widget)
	Label.class_name = "Label"
	Label.new = function(clss, args)
		local self = Widget.new(clss, args)
		self.font = self.font or "default"
		self.text = self.text or ""
		self.halign = self.halign or 0
		self.valign = self.valign or 0.5
		return self
	end
	Label:add_getters{text = function(s) return rawget(s, "__text") end}
	Label:add_setters{text = function(s, v) rawset(s, "__text", v) end}
	-- Instantiating inherited classes.
	local lab = Label{text = "test"}
	assert(lab.text == "test")
	assert(rawget(lab, "__text"))
	assert(not rawget(lab, "text"))
	assert(lab.visible)
	lab.visible = false
	assert(not lab.visible)
	-- Inheritance of accessors.
	local Button = Class(Label)
	Button:add_getters{text = function(s) return Label.getters.text(s) .. "Y" end}
	Button:add_setters{text = function(s, v) return Label.setters.text(s, v .. "X") end}
	local but = Button{text = "test"}
	assert(but.text == "testXYXY") -- Called twice in initializers.
	assert(rawget(but, "__text"))
	assert(not rawget(but, "text"))
end)
