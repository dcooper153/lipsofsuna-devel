Protocol:add_handler{type = "EDIT_TILE", func = function(event)
	local ok,x,y,z,t,d,r = event.packet:read("uint32", "uint32", "uint32", "uint8", "uint8", "uint8")
	if ok then
		Voxel:set_tile{point = Vector(x, y, z), tile = Tile{terrain = t, damage = d, rotation = r}}
	end
end}
