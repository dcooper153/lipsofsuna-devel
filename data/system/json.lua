--- Provides access to the physics simulation.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module system.physics
-- @alias Physics

local Class = require("system/class")
local Vector = require("system/math/vector")

local json = require("system/json/json")

------------------------------------------------------------------------------

--- Provides JSON encoding and decoding capabilities.
-- @type Json
local Json = Class("Json")

--- Encodes a JSON string.
-- @param self Json class.
-- @param str String.
-- @return Object,nil on success. Nil,string on error.
Json.decode = function(self, str)
	local ok,res = pcall(json.decode, str)
	if not ok then
		res = string.gsub(res, "system/json/json.lua:[^:]*: ", "")
		return nil, res
	else
		return res
	end
end

--- Encodes an object.
-- @param self Json class.
-- @param object Object.
-- @return JSON string.
Json.encode = function(self, object)
	return json.encode(object)
end

return Json
