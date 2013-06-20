--- TODO:doc
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module system.file
-- @alias File

local Class = require("system/class")

if not Los.program_load_extension("file") then
	error("loading extension `file' failed")
end

------------------------------------------------------------------------------

--- TODO:doc
-- @type File
local File = Class("File")

--- Reads the contents of a file.
-- @param self File class.
-- @param name File name relative to the mod root.
-- @return String or nil.
File.read = function(self, name)
	return Los.file_read(name)
end

--- Requires all the Lua files in the given directory.
-- @param self File class.
-- @param name Filename relative to the mod root.
-- @param ignore Filename to ignore, or nil for none.
File.require_directory = function(self, name, ignore)
	local i = ignore and ignore .. ".lua"
	for k,v in pairs(File:scan_directory(name)) do
		if not i or v ~= i then
			if string.match(v, "[.]lua$") then
				require(name .. "/" .. string.gsub(v, "(.*)[.]lua$", "%1"))
			end
		end
	end
end

--- Returns the contents of a directory.
-- @param self File class.
-- @param dir Directory name relative to the mod root.
-- @return Table of file names or nil.
File.scan_directory = function(self, dir)
	return Los.file_scan_directory(dir)
end

--- Returns the contents of a save directory.
-- @param self File class.
-- @param dir Directory name relative to the mod save directory root.
-- @return Table of file names or nil.
File.scan_save_directory = function(self, dir)
	return Los.file_scan_save_directory(dir)
end

--- Writes the contents of a file.
-- @param self File class.
-- @param name File name relative to the mod root.
-- @param data Data string.
-- @return True on success.
File.write = function(self, name, data)
	return Los.file_write(name, data)
end

return File


