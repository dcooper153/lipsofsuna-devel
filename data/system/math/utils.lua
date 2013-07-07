--- Math utils.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module system.math.utils
-- @alias MathUtils

local Class = require("system/class")

--- Math utils.
-- @type MathUtils
local MathUtils = Class("MathUtils")

--- Wraps the angle into the [-pi,pi] range.
-- @param self MathUtils class.
-- @param angle Angle in radians.
-- @return New angle in radians.
MathUtils.radian_wrap = function(self, angle)
	if angle < -math.pi then
		local y = -angle % (2 * math.pi)
		if y <= math.pi then return -y end
		return 2 * math.pi - y
	elseif angle > math.pi then
		local y = angle % (2 * math.pi)
		if y <= math.pi then return y end
		return y - 2 * math.pi
	end
	return angle
end

return MathUtils
