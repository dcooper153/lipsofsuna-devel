local Heightmap = require("system/heightmap")

if not Los.program_load_extension("heightmap-render") then
	error("loading extension `heightmap-render' failed")
end

------------------------------------------------------------------------------

--- Adds a texture layer.
-- @param self Heightmap.
-- @param args Arguments.<ul>
--   <li>blend: Blend map texture name.</li>
--   <li>diffuse: Diffuse texture name.</li>
--   <li>height: Height texture name.</li>
--   <li>name: Unique name.</li>
--   <li>normal: Normal texture name.</li>
--   <li>size: World size.</li>
--   <li>specular: Specular texture name.</li></ul>
Heightmap.add_texture_layer = function(self, args)
	Los.heightmap_add_texture_layer(self.handle, args.size,
		args.name, args.diffuse, args.specular, args.normal, args.height, args.blend)
end

--- Rebuilds the heightmap.
-- @param self Heightmap.
Heightmap.rebuild = function(self)
	Los.heightmap_rebuild(self.handle)
end

--- Sets the visibility of the heightmap.
-- @param self Heightmap class.
-- @return Boolean.
Heightmap.get_visible = function(self)
	local v = rawget(self, "__visible")
	return v ~= nil and v or true
end

--- Sets the visibility of the heightmap.
-- @param self Heightmap class.
-- @param value Boolean.
Heightmap.set_visible = function(self, value)
	rawset(self, "__visible", value)
	Los.heightmap_set_visible(self.handle, value)
end

return Heightmap
