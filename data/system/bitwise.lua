--- TODO:doc
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module system.bitwise
-- @alias Bitwise

local Class = require("system/class")

--- TODO:doc
-- @type Bitwise
local Bitwise = Class("Bitwise")

Bitwise.bchk = function(clss, x, y)
	return x % (y + y) >= y
end

Bitwise.band = function(clss, x, y)
	local v = 0
	for b = 0,31 do
		if Bitwise:bchk(x,2^b) and Bitwise:bchk(y,2^b) then v = v + 2^b end
	end
	return v
end

Bitwise.bor = function(clss, x, y)
	local v = 0
	for b = 0,31 do
		if Bitwise:bchk(x,2^b) or Bitwise:bchk(y,2^b) then v = v + 2^b end
	end
	return v
end

return Bitwise


