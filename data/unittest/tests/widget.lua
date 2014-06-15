Unittest:add(2, "system", "widget", function()
	local Class = require("system/class")
	local Program = require("system/graphics")
	local Widget = require("system/widget")
	-- Initialization.
	local w = Widget()
	assert(w)
	-- Getters.
	assert(type(w:get_height()) == "number")
	assert(type(w:get_width()) == "number")
	-- Child packing.
	local c = Widget()
	w:add_child(c)
	assert(c:get_parent() == w)
	collectgarbage()
	assert(c:get_parent() == w)
	-- Visibility.
	assert(w:get_visible())
	assert(not w:get_floating())
	w:set_floating(true)
	assert(w:get_floating())
	assert(w:get_visible())
	w:set_visible(false)
	assert(not w:get_floating())
	assert(not w:get_visible())
	-- Inheritance of new classes.
	local Label = Class("Label", Widget)
	Label.new = function(clss)
		local self = Widget.new(clss)
		self.text = self.__text or ""
		return self
	end
	Label.get_text = function(self) return self.__text end
	Label.set_text = function(self, v) self.__text = v end
	-- Instantiating inherited classes.
	local lab = Label()
	lab:set_text("test") 
	assert(lab:get_text() == "test")
	assert(lab.__text)
	assert(lab:get_visible())
	lab:set_visible(false)
	assert(not lab:get_visible())
	-- Inheritance of accessors.
	local Button = Class("Button", Label)
	Button.get_text = function(self) return Label.get_text(self) .. "Y" end
	Button.set_text = function(self, v) Label.set_text(self, v .. "X") end
	local but = Button()
	but:set_text("test")
	assert(but:get_text() == "testXY")
	assert(but.__text)
end)
