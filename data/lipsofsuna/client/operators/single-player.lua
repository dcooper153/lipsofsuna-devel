Operators.single_player = Class()
Operators.single_player.data = {}

--- Checks if the given save name exists.
--
-- Context: Any.
--
-- @param self Operator.
-- @param name String.
-- @return True if exists, false if not.
Operators.single_player.does_save_name_exist = function(self, name)
	local found = false
	local file = "save" .. self:encode_save_filename(name) .. ".sqlite"
	for k,v in ipairs(File:scan_save_directory("")) do
		if v == file then return true end
	end
end

--- Generates a unique save file name.
--
-- Context: Any.
--
-- @param self Operator.
-- @return String.
Operators.single_player.generate_new_save_name = function(self)
	local name
	for i=1,1000 do
		name = string.format("Game %d", i)
		if not self:does_save_name_exist(name) then break end
	end
	self:set_save_name(name)
	return name
end

--- Decodes as a save file name.</br>
--
-- Context: Any.
--
-- @param self Operator.
-- @param value String.
-- @return String.
Operators.single_player.decode_save_filename = function(self, value)
	return string.gsub(value, "_(%x%x)", function(s)
		return string.char(tonumber(s, 16)) end)
end

--- Encodes as a save file name.</br>
--
-- Context: Any.
--
-- @param self Operator.
-- @param value String.
-- @return String.
Operators.single_player.encode_save_filename = function(self, value)
	return string.gsub(value, "[^a-z0-9]", function(s)
		return string.format("_%02x", string.byte(s)) end)
end

--- Gets the current save file name.</br>
--
-- Context: Any.
--
-- @param self Operator.
-- @return String, or nil.
Operators.single_player.get_save_filename = function(self)
	return self.data.save_filename
end

--- Sets the current save file name.</br>
--
-- Context: Any.
--
-- @param self Operator.
-- @param value String.
Operators.single_player.set_save_filename = function(self, value)
	self.data.save_name = self:decode_save_filename(value)
	self.data.save_filename = value
end

--- Gets the current save name.</br>
--
-- Context: Any.
--
-- @param self Operator.
-- @return String, or nil.
Operators.single_player.get_save_name = function(self)
	return self.data.save_name
end

--- Sets the current save name.</br>
--
-- Context: Any.
--
-- @param self Operator.
-- @param value String.
Operators.single_player.set_save_name = function(self, value)
	self.data.save_name = value
	self.data.save_filename = self:encode_save_filename(value)
end
