--- TODO:doc
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module system.widgets
-- @alias Widgets

local Class = require("system/class")

if not Los.program_load_extension("widgets") then
	error("loading extension `widgets' failed")
end

------------------------------------------------------------------------------

--- TODO:doc
-- @type Widgets
local Widgets = Class("Widgets")

--- Adds a font style.
-- @param clss Widgets class.
-- @param name Font name.
-- @param file Font file.
-- @param size Font size.
Widgets.add_font_style = function(clss, name, file, size)
	Los.widgets_add_font_style(name, file, size)
end

return Widgets
