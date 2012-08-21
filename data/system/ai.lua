local Class = require("system/class")
local Vector = require("system/math/vector")

if not Los.program_load_extension("ai") then
	error("loading extension `ai' failed")
end

------------------------------------------------------------------------------

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
