--- Finite impulse response filter.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module system.filter.fir
-- @alias FirFilter

local Class = require("system/class")

if not Los.program_load_extension("filter") then
	error("loading extension `filter' failed")
end

------------------------------------------------------------------------------

--- Finite impulse response filter.
-- @type FirFilter
local FirFilter = Class("FirFilter")

--- Creates a new filter.
-- @param clss FirFilter class.
-- @param coeffs Table of coefficients.
-- @return FirFilter.
FirFilter.new = function(clss, coeffs)
	local self = Class.new(clss)
	self.handle = Los.fir_filter_new(#coeffs, coeffs)
	if not self.handle then
		error("invalid filter coefficients")
	end
	return self
end

--- Adds a sample into the delay line.
-- @param self FirFilter.
-- @param sample Number.
FirFilter.add = function(self, sample)
	Los.fir_filter_add(self.handle, sample)
end

--- Gets the current filter output.
-- @param self FirFilter.
-- @return Number.
FirFilter.get = function(self)
	return Los.fir_filter_get(self.handle)
end

return FirFilter
