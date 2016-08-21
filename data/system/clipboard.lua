--- Module for accessing the clipboard.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module system.clipboard
-- @alias Clipboard

local Class = require("system/class")

if not Los.program_load_extension("clipboard") then
	error("loading extension `clipboard' failed")
end

Los.clipboard_init()

------------------------------------------------------------------------------

--- Module for accessing the clipboard.
--
--
-- @type Clipboard
local Clipboard = Class("Clipboard")

--- Get a text string from the clipboard.
-- @return The current text in the clipboard.
Clipboard.get = function()
	return Los.clipboard_get()
end

--- Set a text string a clipboard.
-- @param text The text to set into the clipboard.
Clipboard.set = function(text)
	Los.clipboard_set(text)
end


return Clipboard
