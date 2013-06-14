--- Colorspace conversions and helpers.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module system.color
-- @alias Color

local Class = require("system/class")

--- Colorspace conversions and helpers.
-- @type Color
local Color = Class("Color")

--- Converts an HSV color to RGB.
-- @param clss Color class.
-- @param hsv HSV color.
-- @return RGB color.
Color.hsv_to_rgb = function(clss, hsv)
	local h,s,v = hsv[1],hsv[2],hsv[3]
	local c = v * s
	local l = v - c
	local hh = h * 6
	local x = c * (1 - math.abs(hh % 2 - 1))
	if 0 <= hh and hh < 1 then return {c + l, x + l, l} end
	if 1 <= hh and hh < 2 then return {x + l, c + l, l} end
	if 2 <= hh and hh < 3 then return {l, c + l, x + l} end
	if 3 <= hh and hh < 4 then return {l, x + l, c + l} end
	if 4 <= hh and hh < 5 then return {x + l, l, c + l} end
	return {c + l, l, x + l}
end

--- Converts an RGB color to HSV.
-- @param clss Color class.
-- @param rgb RGB color.
-- @return HSV color.
Color.rgb_to_hsv = function(clss, rgb)
	local eps = 0.00000001
	local r,g,b = rgb[1],rgb[2],rgb[3]
	local v = math.max(math.max(r, g), b)
	local m = math.min(math.min(r, g), b)
	local c = v - m
	if c < eps then h = 0
	elseif v == r then h = ((g - b) / c % 6) / 6
	elseif v == g then h = ((b - r) / c + 2) / 6
	elseif v == b then h = ((r - g) / c + 4) / 6 end
	if c < eps then s = 0
	else s = c / v end
	return {h, s, v}
end

--- Converts a [0,1] color to [0,255]
-- @param clss Color class.
-- @param color color.
-- @return Color.
Color.float_to_ubyte = function(clss, color)
	if not color then return end
	return {color[1] * 255, color[2] * 255, color[3] * 255}
end

--- Converts a [0,255] color to [0,1]
-- @param clss Color class.
-- @param color color.
-- @return Color.
Color.ubyte_to_float = function(clss, color)
	if not color then return end
	return {color[1] / 255, color[2] / 255, color[3] / 255}
end

--- Multiplies the saturation of the RGB color.
--
-- rgb' = mult * (rgb - max) + max
--
-- @param clss Color class.
-- @param rgb RGB color.
-- @param mult Number.
-- @return RGB color.
Color.rgb_multiply_saturation = function(clss, rgb, mult)
	local v = math.max(math.max(rgb[1], rgb[2]), rgb[3])
	return {(rgb[1] - v) * mult + v, (rgb[2] - v) * mult + v, (rgb[3] - v) * mult + v}
end

--- Multiplies the value of the RGB color.
-- @param clss Color class.
-- @param rgb RGB color.
-- @param mult Number.
-- @return RGB color.
Color.rgb_multiply_value = function(clss, rgb, mult)
	return {rgb[1] * mult, rgb[2] * mult, rgb[3] * mult}
end

--- Sets the saturation of the RGB color.
--
-- mult * (min - max) + max = max * (1 - sat')
--
-- @param clss Color class.
-- @param rgb RGB color.
-- @param saturation Saturation.
-- @return RGB color.
Color.rgb_set_saturation = function(clss, rgb, saturation)
	local v = math.max(math.max(rgb[1], rgb[2]), rgb[3])
	local m = math.min(math.min(rgb[1], rgb[2]), rgb[3])
	if m == v then return {rgb[0], rgb[1], rgb[2]} end
	local mult = saturation * v / (v - m)
	return {(rgb[1] - v) * mult + v, (rgb[2] - v) * mult + v, (rgb[3] - v) * mult + v}
end

--- Sets the value of the RGB color.
-- @param clss Color class.
-- @param rgb RGB color.
-- @param value Value.
-- @return RGB color.
Color.rgb_set_value = function(clss, rgb, value)
	local v = math.max(math.max(rgb[1], rgb[2]), rgb[3])
	if v == 0 then return {value, value, value} end
	local mult = value / v
	return {rgb[1] * mult, rgb[2] * mult, rgb[3] * mult}
end

return Color
