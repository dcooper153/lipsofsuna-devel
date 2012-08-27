--- TODO:doc
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module system.string
-- @alias String

local Class = require("system/class")

if not Los.program_load_extension("string") then
	error("loading extension `string' failed")
end

------------------------------------------------------------------------------

--- TODO:doc
-- @type String
local String = Class("String")

String.split = function(self, sep)
	local sep,fields = sep or " ", {}
	local pattern = string.format("([^%s]+)", sep)
	self:gsub(pattern, function(c) fields[#fields+1] = c end)
	return fields
end

--- Converts a UTF-8 string to an array of wide characters.
-- @param str String.
-- @return Array of wide characters.
String.utf8_to_wchar = function(str)
	return Los.string_utf8_to_wchar(str)
end

--- Converts an array of wide characters to a UTF-8 string.
-- @param wstr Array of wide characters.
-- @return String.
String.wchar_to_utf8 = function(wstr)
	return Los.string_wchar_to_utf8(wstr)
end

return String


