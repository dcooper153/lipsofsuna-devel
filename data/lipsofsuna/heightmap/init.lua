Program:add_path(Mod.path)

Map = Class()
Map.class_name = "Map"

Map.init = function(self)
	if self.initialized then return end
	self.initialized = true
	local heights = Image("terrainheight1.png")
	self.heightmap = Heightmap{position = Vector(1024, 1700, 1024), size = 513, scaling = 160, spacing = 4, heights = heights}
	if not Settings.server then
		self.heightmap:add_texture_layer{size = 10, name = "soil1",
			diffuse = "soil1", specular = "soil1s",
			normal = "soil1n", height = "soil1h"}
		self.heightmap:add_texture_layer{size = 10, name = "grass1",
			diffuse = "grass1", specular = "soil1s",
			normal = "grass1n", height = "soil1h", blend = "terrainsplat1"}
		self.heightmap:add_texture_layer{size = 30, name = "granite1",
			diffuse = "granite1", specular = "granite1s",
			normal = "granite1n", height = "soil1h", blend = "terrainsplat2"}
	end
end

Staticspec{
	name = "statictree1",
	model = "tree2"}
