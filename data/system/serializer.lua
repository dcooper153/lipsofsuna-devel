--- Generic serialization of tables or class instances.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module system.serializer
-- @alias Serializer

local Class = require("system/class")
local Quaternion = require("system/math/quaternion")
local Serialize = require("system/serialize")
local String = require("system/string")
local Vector = require("system/math/vector")

--- Generic serialization of tables or class instances.
-- @type Serializer
local Serializer = Class("Serializer")

Serializer.__decoders = {
	["boolean"] = function(v)
		return v == "true"
	end,
	["number"] = function(v)
		return tonumber(v)
	end,
	["number list"] = function(v)
		local t = {}
		for k,v in ipairs(String.split(v, ",")) do
			local x = tonumber(v)
			if not x then return end
			t[k] = x
		end
		if #t == 0 then return end
		return t
	end,
	["lua"] = function(v)
		return Serialize:read(v)
	end,
	["quaternion"] = function(v)
		local t = String.split(v, ",")
		if #t ~= 4 then return end
		local x = tonumber(t[1])
		local y = tonumber(t[2])
		local z = tonumber(t[3])
		local w = tonumber(t[4])
		if not x or not y or not z or not w then return end
		return Quaternion(x, y, z, w)
	end,
	["string"] = function(v)
		return v
	end,
	["vector"] = function(v)
		local t = String.split(v, ",")
		if #t ~= 3 then return end
		local x = tonumber(t[1])
		local y = tonumber(t[2])
		local z = tonumber(t[3])
		if not x or not y or not z then return end
		return Vector(x, y, z)
	end}

Serializer.__encoders = {
	["boolean"] = function(v)
		return v and "true" or "false"
	end,
	["lua"] = function(v)
		return Serialize:write(v)
	end,
	["number"] = function(v)
		return string.format("%g", v)
	end,
	["number list"] = function(v)
		return table.concat(v, ",")
	end,
	["quaternion"] = function(v)
		return string.format("%g,%g,%g,%g", v.x, v.y, v.z, v.w)
	end,
	["string"] = function(v)
		return v
	end,
	["vector"] = function(v)
		return string.format("%g,%g,%g", v.x, v.y, v.z)
	end}

--- Creates a new serializer.
-- @param clss Serializer class.
-- @param fields List of fields.
-- @return Serializer.
Serializer.new = function(clss, fields)
	local self = Class.new(clss)
	self.__fields = fields
	return self
end

--- Adds a new serializable field.
-- @param self Serializer.
-- @param name Name string.
-- @param type Type string.
-- @param get Getter function. Nil to read directly from the table.
-- @param set Setter function. Nil to write directly to the table.
Serializer.add_field = function(self, name, type, get, set)
	table.insert(self.__fields,
	{
		name = name,
		type = type,
		get = get,
		set = set
	})
end

--- Encodes a value of the given type.
-- @param self Serializer.
-- @param type Type name.
-- @param value Storage value.
-- @return Native value.
Serializer.decode_value = function(self, type, value)
	if value == nil then return end
	local func = self.__decoders[type]
	if not func then return end
	return func(value)
end

--- Decodes a value of the given type.
-- @param self Serializer.
-- @param type Type name.
-- @param value Native value.
-- @return Storage value.
Serializer.encode_value = function(self, type, value)
	if value == nil then return end
	local func = self.__encoders[type]
	if not func then return end
	return func(value)
end

--- Deserializes a table.
-- @param self Serializer.
-- @param data Table to deserialize.
-- @param rows List of (name,value) pairs.
Serializer.read = function(self, data, rows)
	-- Create a dictionary.
	local l = {}
	for k,v in ipairs(rows) do
		l[v[1]] = v[2]
	end
	-- Load the fields in the order of the field list.
	for k,field in ipairs(self.__fields) do
		local rawval = l[field.name]
		if rawval ~= nil then
			local value = self:decode_value(field.type, rawval)
			if value ~= nil then
				if field.set then
					field.set(data, value)
				else
					data[field.name] = value
				end
			end
		end
	end
end

--- Serializes a table.
-- @param self Serializer.
-- @param data Table to serialize.
-- @param query Database query function.
Serializer.write = function(self, data, query)
	-- Save the fields in the order of the field list.
	for k,field in ipairs(self.__fields) do
		-- Get the value.
		local value
		if field.get then
			value = field.get(data)
		else
			value = data[field.name]
		end
		-- Write the field.
		local rawval = self:encode_value(field.type, value)
		if rawval ~= nil then
			query(field.name, rawval)
		end
	end
end

return Serializer
