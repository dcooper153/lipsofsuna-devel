--- Help link widget.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module help.ui_help_link
-- @alias UiHelpLink

local Class = require("system/class")
local UiHelpText = require("help/ui-help-text")
local UiMenu = require("ui/widgets/menu")

--- Help link widget.
-- @type UiHelpLink
local UiHelpLink = Class("UiHelpLink", UiMenu)

--- Creates a new help link widget.
-- @param clss UiHelpLink class.
-- @param text Label of the widget.
-- @param link Name of the linked help spec.
-- @return UiHelpLink.
UiHelpLink.new = function(clss, text, link)
	local self = UiMenu.new(clss)
	self.label = text
	self.link = link
	self.hint = "$A: Show topic\n$$B\n$$U\n$$D"
	return self
end

--- Handles the apply command.
-- @param self UiMenu.
UiHelpLink.apply = function(self)
	-- Find the help text.
	local help = Main.specs:find_by_name("HelpSpec", self.link)
	if not help then return end
	-- Create the widgets.
	local widgets = {}
	for k,v in ipairs(help.text) do
		if v[1] == "link" then
			table.insert(widgets, UiHelpLink(v[2], v[3]))
		elseif v[1] == "paragraph" then
			table.insert(widgets, UiHelpText(v[2]))
		end
	end
	-- Popup the widgets.
	Main.effect_manager:play_global("uitransition1")
	self:set_menu_widgets(widgets)
	UiMenu.apply(self)
end

--- Rebuilds the canvas.
-- @param self UiRadioMenu.
UiHelpLink.rebuild_canvas = function(self)
	Theme:draw_button(self, self.label,
		0, 0, self.size.x, self.size.y, self.focused, false)
end

return UiHelpLink
