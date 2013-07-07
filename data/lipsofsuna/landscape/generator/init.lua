local File = require("system/file")
local TerrainGenerator = require("landscape/generator/terrain-generator")

Main.game_start_hooks:register(5, function()
	if Main.terrain then
		Main.terrain.generate_hooks:register(20, TerrainGenerator.generate)
	end
end)
