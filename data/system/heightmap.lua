require "system/class"

if not Los.program_load_extension("heightmap") then
	error("loading extension `heightmap' failed")
end

------------------------------------------------------------------------------

Heightmap = Class()
Heightmap.class_name = "Heightmap"

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
	__userdata_lookup[self.handle] = self
	if args then
		for k,v in pairs(args) do self[k] = v end
	end
	return self
end

Heightmap.add_texture_layer = function()
end
