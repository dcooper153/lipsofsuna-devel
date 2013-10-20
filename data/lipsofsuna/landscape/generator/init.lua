local File = require("system/file")
local TerrainGenerator = require("landscape/generator/terrain-generator")

Main.game_start_hooks:register(5, function()
	if Main.terrain then
		Main.terrain_generator = TerrainGenerator(Main.terrain)
		Main.terrain.generate_hooks:register(20, function(chunk)
			Main.terrain_generator:generate(chunk)
		end)
	end
end)

Main.game_end_hooks:register(5, function()
	if Main.terrain_generator then
		Main.terrain_generator = nil
	end
end)

Main.game_load_hooks:register(5, function(db)
	if Main.terrain_generator then
		Main.terrain_generator:load(db)
	end
end)

Main.game_save_hooks:register(5, function(db, erase)
	if Main.terrain_generator then
		Main.terrain_generator:save(db, erase)
	end
end)
