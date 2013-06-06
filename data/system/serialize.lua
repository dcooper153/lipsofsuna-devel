--- Generic serialization of Lua types.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module system.serialize
-- @alias Serialize

local Class = require("system/class")

------------------------------------------------------------------------------

--- Generic serialization of Lua types.
-- @type Serialize
local Serialize = Class("Serialize")

local serialize
serialize = function(val)
	if type(val) == "string" then
		-- Write an escaped and quoted string.
		return string.format("%q", val)
	elseif type(val) == "table" then
		if val.class then
			-- Write custom class instances.
			if val.write then
				return val:write()
			else
				return tostring(val)
			end
		else
			-- Write a table of serialized values.
			local str
			for k,v in pairs(val) do
				local ks
				local vs = serialize(v)
				if string.match(k, "[^a-zA-Z_]") then
					ks = string.format("[%s]", serialize(k))
				else
					ks = k
				end
				if ks and vs and ks ~= "nil" and vs ~= "nil" then
					if str then
						str = string.format("%s,%s=%s", str, ks, vs)
					else
						str = string.format("%s=%s", ks, vs)
					end
				end
			end
			return string.format("{%s}", str or "")
		end
	elseif type(val) == "number" or type(val) == "boolean" then
		-- Convert primite types to strings.
		return tostring(val)
	else
		-- Default to nil for others.
		return "nil"
	end
end

--- Serializes the value.
-- @param value Value.
-- @return String.
Serialize.write = function(self, value)
	serialize(value)
end

return Serialize
