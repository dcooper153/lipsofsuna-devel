--- Help text widget.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module help.ui_help_text
-- @alias UiHelpText

local Class = require("system/class")
local UiLabel = require("ui/widgets/label")

--- Help text widget.
-- @type UiHelpText
local UiHelpText = Class("UiHelpText", UiLabel)

--- Creates a new help text widget.
-- @param clss UiHelpText class.
-- @param text Context of the widget.
-- @return UiHelpText.
UiHelpText.new = function(clss, text)
	local self = UiLabel.new(clss, text)
	self.hint = "$$B\n$$U\n$$D"
	return self
end

return UiHelpText
