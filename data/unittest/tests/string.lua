Unittest:add(1, "string", function()
	require "system/string"
	-- Wide character conversion.
	local pos = 1
	local str = "STOP、ペロペロ"
	local wstr = String.utf8_to_wchar(str)
	assert(#wstr == 9)
	local str1 = String.wchar_to_utf8(wstr)
	assert(str == str1)
end)
