--- Integer value selector.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module ui.widgets.scroll_integer
-- @alias UiScrollInteger

local Class = require("system/class")
local UiScrollFloat = require("ui/widgets/scrollfloat")

--- Integer value selector.
-- @alias UiScrollInteger
local UiScrollInteger = Class("UiScrollInteger", UiScrollFloat)

UiScrollInteger.new = function(clss, label, min, max, value, changed)
	local self = UiScrollFloat.new(clss, label, min, max, value, changed)
	self.text = tostring(value)
	self.step = 1
	return self
end

UiScrollInteger.get_value_at = function(self, x)
	return math.floor(UiScrollFloat.get_value_at(self, x) + 0.5)
end

UiScrollInteger.update_text = function(self)
	self.text = tostring(self.value)
end

return UiScrollInteger
