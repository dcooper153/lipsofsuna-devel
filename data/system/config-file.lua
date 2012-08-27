--- TODO:doc
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module system.config_file
-- @alias ConfigFile

local Class = require("system/class")

if not Los.program_load_extension("config-file") then
	error("loading extension `config-file' failed")
end

------------------------------------------------------------------------------

--- TODO:doc
-- @type ConfigFile
local ConfigFile = Class("ConfigFile")

--- Opens a configuration file.
-- @param clss ConfigFile class.
-- @param args Arguments.<ul>
--   <li>1,name: Unique configuration file name.</li></ul>
-- @return New tile.
ConfigFile.new = function(clss, args)
	local self = Class.new(clss)
	self.handle = Los.config_file_new(args)
	assert(self.handle, string.format("creating config file %q failed", args[1] or args.name))
	return self
end

--- Gets a configuration value.
-- @param self Configuration file.
-- @param k Key string.
-- @return Value string or nil.
ConfigFile.get = function(self, k)
	return Los.config_file_get(self.handle, k)
end

--- Saves the configuration file.
-- @param self Configuration file.
ConfigFile.save = function(self)
	return Los.config_file_save(self.handle)
end

--- Sets a configuration value.
-- @param self Configuration file.
-- @param k Key string.
-- @param v Value string.
ConfigFile.set = function(self, k, v)
	Los.config_file_set(self.handle, k, v)
end

return ConfigFile


