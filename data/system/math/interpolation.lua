--- TODO:doc
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module system.math.interpolation
-- @alias Interpolation

local Class = require("system/class")
local Vector = require("system/math/vector")

--- Interpolate between samples.<br/>
--
-- The interpolation functions in this module assume that spacing between
-- samples on the time axis is always one. If this is not the case, the
-- caller should calculate a time offset that fulfills the requirement.
--
-- @type Interpolation
local Interpolation = Class("Interpolation")

------------------------------------------------------------------------------

--- Performs Catmull-Rom interpolation for the sample set.
-- @param self Interpolation class.
-- @param time Time offset.
-- @param points List of numbers.
-- @return Number.
Interpolation.interpolate_samples_catmull_1d = function(self, time, points)
	local t,p0,p1,p2,p3 = self:select_segment_4(time, points)
	if not t then return p0 end
	return self:interpolate_segment_catmull(t, p0, p1, p2, p3)
end

--- Performs Catmull-Rom interpolation for the sample set.
-- @param self Interpolation class.
-- @param time Time offset.
-- @param points List of vectors.
-- @return Vector.
Interpolation.interpolate_samples_catmull_2d = function(self, time, points)
	local t,p0,p1,p2,p3 = self:select_segment_4(time, points)
	if not t then return p0 end
	local x = self:interpolate_segment_catmull(t, p0.x, p1.x, p2.x, p3.x)
	local y = self:interpolate_segment_catmull(t, p0.y, p1.y, p2.y, p3.y)
	return Vector(x, y)
end

--- Performs Catmull-Rom interpolation for the sample set.
-- @param self Interpolation class.
-- @param time Time offset.
-- @param points List of vectors.
-- @return Vector.
Interpolation.interpolate_samples_catmull_3d = function(self, time, points)
	local t,p0,p1,p2,p3 = self:select_segment_4(time, points)
	if not t then return p0 end
	local x = self:interpolate_segment_catmull(t, p0.x, p1.x, p2.x, p3.x)
	local y = self:interpolate_segment_catmull(t, p0.y, p1.y, p2.y, p3.y)
	local z = self:interpolate_segment_catmull(t, p0.z, p1.z, p2.z, p3.z)
	return Vector(x, y, z)
end

--- Performs cubic interpolation for the sample set.
-- @param self Interpolation class.
-- @param time Time offset.
-- @param points List of numbers.
-- @return Number.
Interpolation.interpolate_samples_cubic_1d = function(self, time, points)
	local t,p0,p1,p2,p3 = self:select_segment_4(time, points)
	if not t then return p0 end
	return self:interpolate_segment_cubic(t, p0, p1, p2, p3)
end

--- Performs cubic interpolation for the sample set.
-- @param self Interpolation class.
-- @param time Time offset.
-- @param points List of vectors.
-- @return Vector.
Interpolation.interpolate_samples_cubic_2d = function(self, time, points)
	local t,p0,p1,p2,p3 = self:select_segment_4(time, points)
	if not t then return p0 end
	local x = self:interpolate_segment_cubic(t, p0.x, p1.x, p2.x, p3.x)
	local y = self:interpolate_segment_cubic(t, p0.y, p1.y, p2.y, p3.y)
	return Vector(x, y)
end

--- Performs cubic interpolation for the sample set.
-- @param self Interpolation class.
-- @param time Time offset.
-- @param points List of vectors.
-- @return Vector.
Interpolation.interpolate_samples_cubic_3d = function(self, time, points)
	local t,p0,p1,p2,p3 = self:select_segment_4(time, points)
	if not t then return p0 end
	local x = self:interpolate_segment_cubic(t, p0.x, p1.x, p2.x, p3.x)
	local y = self:interpolate_segment_cubic(t, p0.y, p1.y, p2.y, p3.y)
	local z = self:interpolate_segment_cubic(t, p0.z, p1.z, p2.z, p3.z)
	return Vector(x, y, z)
end

--- Performs linear interpolation for the sample set.
-- @param self Interpolation class.
-- @param time Time offset.
-- @param points List of numbers.
-- @return Number.
Interpolation.interpolate_samples_linear_1d = function(self, time, points)
	local t,p1,p2 = self:select_segment_2(time, points)
	if not t then return p1 end
	return self:interpolate_segment_linear(t, p1, p2)
end

--- Performs linear interpolation for the sample set.
-- @param self Interpolation class.
-- @param time Time offset.
-- @param points List of vectors.
-- @return Vector.
Interpolation.interpolate_samples_linear_2d = function(self, time, points)
	local t,p1,p2 = self:select_segment_2(time, points)
	if not t then return p1 end
	local x = self:interpolate_segment_linear(t, p1.x, p2.x)
	local y = self:interpolate_segment_linear(t, p1.y, p2.y)
	return Vector(x, y)
end

--- Performs linear interpolation for the sample set.
-- @param self Interpolation class.
-- @param time Time offset.
-- @param points List of vectors.
-- @return Vector.
Interpolation.interpolate_samples_linear_3d = function(self, time, points)
	local t,p1,p2 = self:select_segment_2(time, points)
	if not t then return p1 end
	local x = self:interpolate_segment_linear(t, p1.x, p2.x)
	local y = self:interpolate_segment_linear(t, p1.y, p2.y)
	local z = self:interpolate_segment_linear(t, p1.z, p2.z)
	return Vector(x, y, z)
end

------------------------------------------------------------------------------

--- Performs Catmull-Rom interpolation between points p1 and p2.
-- @param self Interpolation class.
-- @param t Time offset between [0,1].
-- @param p0 Point.
-- @param p1 Point.
-- @param p2 Point.
-- @param p3 Point.
-- @return Point.
Interpolation.interpolate_segment_catmull = function(self, t, p0, p1, p2, p3)
	local a = -0.5 * p0 + 1.5 * p1 - 1.5 * p2 + 0.5 * p3
	local b = p0 - 2.5 * p1 + 2 * p2 - 0.5 * p3
	local c = -0.5 * p0 + 0.5 * p2
	return a * t^3 + b * t^2 + c * t + p1
end

--- Performs cubic interpolation between points p1 and p2.
-- @param self Interpolation class.
-- @param t Time offset between [0,1].
-- @param p0 Point.
-- @param p1 Point.
-- @param p2 Point.
-- @param p3 Point.
-- @return Point.
Interpolation.interpolate_segment_cubic = function(self, t, p0, p1, p2, p3)
	local a = p3 - p2 - p0 + p1
	local b = p0 - p1 - a
	local c = p2 - p0
	return a * t^3 + b * t^2 + c * t + p1
end

--- Performs linear interpolation between points p1 and p2.
-- @param self Interpolation class.
-- @param t Time offset between [0,1].
-- @param p1 Point.
-- @param p2 Point.
-- @return Point.
Interpolation.interpolate_segment_linear = function(self, t, p1, p2)
	return p1 * (1 - t) + p2 * t
end

------------------------------------------------------------------------------

--- Finds a segment for linear interpolation.<br/>
--
-- If the time offset is past either end of the spline, then nil and the
-- corresponding end point is returned instead of the normal return value.
-- Hence, when the time is nil, the result of interpolation is that point.
--
-- @param self Interpolation class.
-- @param t Time offset.
-- @param p List of points.
-- @return Time, point, point.
Interpolation.select_segment_2 = function(self, t, p)
	-- Handle out of bounds cases.
	local i = math.floor(t) + 1
	local l = #p
	if i < 1 then return nil,p[1] end
	if i >= l then return nil,p[l] end
	-- Return the segment.
	return t - i + 1, p[i], p[i + 1]
end

--- Finds a segment for cubic interpolation.<br/>
--
-- If the time offset is past either end of the spline, then nil and the
-- corresponding end point is returned instead of the normal return value.
-- Hence, when the time is nil, the result of interpolation is that point.
--
-- @param self Interpolation class.
-- @param t Time offset.
-- @param p List of points.
-- @return Time, point, point, point, point.
Interpolation.select_segment_4 = function(self, t, p)
	-- Handle out of bounds cases.
	local i = math.floor(t) + 1
	local l = #p
	if i < 1 then return nil,p[1] end
	if i >= l then return nil,p[l] end
	-- Return the segment.
	local p1 = p[i]
	local p2 = p[i + 1]
	local p0 = p[i - 1] or 2 * p1 - p2
	local p3 = p[i + 2] or 2 * p2 - p1
	return t - i + 1, p0, p1, p2, p3
end

return Interpolation


