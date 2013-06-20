local File = require("system/file")
local TerrainGenerator = require("landscape/generator/terrain-generator")

for k,v in pairs(File:scan_directory(Mod.path .. "sectors")) do
	require(Mod.path .. "sectors/" .. string.gsub(v, "([^.]*).*", "%1"))
end

Main.game_start_hooks:register(5, function()
	if Main.game.terrain then
		Main.game.terrain.generate_hooks:register(20, TerrainGenerator.generate)
	end
end)
