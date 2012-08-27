--- TODO:doc
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module system.ai
-- @alias Ai

local Class = require("system/class")
local Vector = require("system/math/vector")

if not Los.program_load_extension("ai") then
	error("loading extension `ai' failed")
end

------------------------------------------------------------------------------

--- TODO:doc
-- @type Ai
local Ai = Class("Ai")

--- Solves a path between two points.
-- @param self Ai class.
-- @param args Arguments.<ul>
--   <li>start: Start point in tiles.</li>
--   <li>target: End point in tiles.</li></ul>
-- @return Array of vectors or nil.
Ai.solve_path = function(self, args)
	local r = Los.ai_solve_path{start = args.start.handle, target = args.target.handle}
	if r then
		for k,v in pairs(r) do
			r[k] = Vector:new_from_handle(v)
		end
	end
	return r
end

return Ai


