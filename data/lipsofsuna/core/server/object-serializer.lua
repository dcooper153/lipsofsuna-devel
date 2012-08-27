--- TODO:doc
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.server.object_serializer
-- @alias ObjectSerializer

local Class = require("system/class")
local Quaternion = require("system/math/quaternion")
local String = require("system/string")
local Vector = require("system/math/vector")

local decoders = {
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

local encoders = {
	["boolean"] = function(v)
		return v and "true" or "false"
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

--- TODO:doc
-- @type ObjectSerializer
local ObjectSerializer = Class("ObjectSerializer")

--- Creates a new object manager.
-- @param clss ObjectSerializer class.
-- @param fields List of serializable fields.
-- @return ObjectSerializer.
ObjectSerializer.new = function(clss, fields)
	local self = Class.new(clss)
	self.fields = fields
	return self
end

--- Encodes a value of the given type.
-- @param self ObjectSerializer.
-- @param type Type name.
-- @param value Database value.
-- @return Native value.
ObjectSerializer.decode_value = function(self, type, value)
	if value == nil then return end
	local func = decoders[type]
	if not func then return end
	return func(value)
end

--- Decodes a value of the given type.
-- @param self ObjectSerializer.
-- @param type Type name.
-- @param value Native value.
-- @return Database value.
ObjectSerializer.encode_value = function(self, type, value)
	if value == nil then return end
	local func = encoders[type]
	if not func then return end
	return func(value)
end

--- Deserializes an object.
-- @param self ObjectSerializer.
-- @param object Object to deserialize.
-- @param rows List of (name,value) pairs.
ObjectSerializer.read = function(self, object, rows)
	-- Create a dictionary.
	local l = {}
	for k,v in ipairs(rows) do
		l[v[1]] = v[2]
	end
	-- Load the fields in the order of the field list.
	for k,field in ipairs(self.fields) do
		local dbval = l[field.name]
		if dbval ~= nil then
			local value = self:decode_value(field.type, dbval)
			if value ~= nil then
				if field.set then
					field.set(object, value)
				else
					object[field.name] = value
				end
			end
		end
	end
end

--- Serializes an object.
-- @param self ObjectSerializer.
-- @param object Object to serialize.
-- @param query Database query function.
ObjectSerializer.write = function(self, object, query)
	-- Save the fields in the order of the field list.
	for k,field in ipairs(self.fields) do
		-- Get the value.
		local value
		if field.get then
			value = field.get(object)
		else
			value = object[field.name]
		end
		-- Write the field.
		local dbval = self:encode_value(field.type, value)
		if dbval ~= nil then
			query(field.name, dbval)
		end
	end
end

return ObjectSerializer


