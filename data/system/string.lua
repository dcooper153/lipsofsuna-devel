require "system/bitwise"

String = Class()
String.class_name = "String"

--- Checks if the byte at the index of the string is the first byte of a UTF-8 character.
-- @param str String.
-- @param index Index to the string.
-- @return True if starts a character, false if not.
String.utf8_is_char_start = function(str, index)
	local b = string.byte(str, index)
	return Bitwise:band(b, 0xC0) <= 0x80
end

--- Removes the last UTF-8 character of the string.
-- @param str String.
-- @return Copied and modified string.
String.utf8_erase_last_char = function(str)
	local l = #str - 1
	if l < 1 then return "" end
	while l > 0 and not String.utf8_is_char_start(str, l) do
		l = l - 1
	end
	return string.sub(str, 1, l)
end
