require "system/class"

if not Los.program_load_extension("heightmap") then
	error("loading extension `heightmap' failed")
end

------------------------------------------------------------------------------

Heightmap = Class()
Heightmap.class_name = "Heightmap"

--- Creates a new heightmap.
-- @param clss Heightmap class.
-- @param args Arguments.
-- @return New heightmap.
Heightmap.new = function(clss, args)
	local self = Class.new(clss)
	local pos = args and args.position and args.position.handle
	local size = args and args.size
	local spacing = args and args.spacing
	self.handle = Los.heightmap_new(pos, size, spacing)
	__userdata_lookup[self.handle] = self
	if args then
		for k,v in pairs(args) do self[k] = v end
	end
	return self
end
