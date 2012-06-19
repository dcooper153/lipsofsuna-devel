require "system/bitwise"

if not Los.program_load_extension("string") then
	error("loading extension `string' failed")
end

------------------------------------------------------------------------------

String = Class()
String.class_name = "String"

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
