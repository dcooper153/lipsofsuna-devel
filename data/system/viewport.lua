--- A front end for adding and removing viewports.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module system.viewport
-- @alias Viewport

local Class = require("system/class")

if not Los.program_load_extension("render") then
	error("loading extension `render' failed")
end

------------------------------------------------------------------------------

--- A front end for adding and removing viewports.
--
-- This modules is for adding and removing named view ports.
--
-- @type Viewport
local Viewport = Class("Viewport")

--- Adds a new named viewport.
-- @param clss Viewport class.
-- @param name The name of the view port.
-- @param l The left position of the view port (0.0 to 1.0).
-- @param t The top position of the view port (0.0 to 1.0).
-- @param w The width of the view port (0.0 to 1.0).
-- @param h The height of the view port (0.0 to 1.0).
-- @param x The X position of the camera associated with this view port.
-- @param y The Y position of the camera associated with this view port.
-- @param z The Z position of the camera associated with this view port.
Viewport.add = function(clss, name, l, t, w, h, x, y, z)
	Los.viewport_add(name, l, t, w, h, x, y, z)
end

--- Removes a named viewport.
-- @param clss Viewport class.
-- @param name The name of the viewport.
Viewport.remove = function(clss, name)
	Los.viewport_remove(name)
end

--- Removes all viewports.
-- @param clss Viewport class.
Viewport.remove_all = function(clss)
	Los.viewport_remove_all()
end




return Viewport
