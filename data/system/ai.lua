require "system/tiles"

if not Los.program_load_extension("ai") then
	error("loading extension `ai' failed")
end

------------------------------------------------------------------------------

Ai = Class()

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
			r[k] = Class.new(Vector, {handle = v})
		end
	end
	return r
end
