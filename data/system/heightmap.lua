local Class = require("system/class")

if not Los.program_load_extension("heightmap") then
	error("loading extension `heightmap' failed")
end

------------------------------------------------------------------------------

local Heightmap = Class("Heightmap")

--- Creates a new heightmap.
-- @param clss Heightmap class.
-- @param args Arguments.<ul>
--   <li>heights: Image containing the weights.</li>
--   <li>position: Position vector in world space.</li>
--   <li>size: Number of vertices along an axis.</li>
--   <li>scaling: Height scaling in world space units.</li>
--   <li>spacing: Vertex spacing in world space units.</li></ul>
-- @return New heightmap.
Heightmap.new = function(clss, args)
	local self = Class.new(clss)
	local pos = args and args.position and args.position.handle
	local size = args and args.size
	local spacing = args and args.spacing
	local scaling = args and args.scaling
	local heights = args and args.heights and args.heights.handle
	self.handle = Los.heightmap_new(pos, size, spacing, scaling, heights)
	if args then
		for k,v in pairs(args) do self[k] = v end
	end
	return self
end

Heightmap.add_texture_layer = function()
end

--- Gets the height of the heightmap at the given point.
-- @param self Heightmap.
-- @param point Point in world space. X and Z values are used.
-- @param clamp False to disable clamping.
-- @return World space Y value.
Heightmap.get_height = function(self, point, clamp)
	return Los.heightmap_get_height(self.handle, point.handle, clamp)
end

return Heightmap
