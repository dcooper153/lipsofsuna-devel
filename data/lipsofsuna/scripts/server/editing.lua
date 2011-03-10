Protocol:add_handler{type = "EDIT_TILE", func = function(event)
	local ok,x,y,z,t = event.packet:read("uint32", "uint32", "uint32", "uint8")
	if ok then
		Voxel:set_tile(Vector(x, y, z), t)
	end
end}
