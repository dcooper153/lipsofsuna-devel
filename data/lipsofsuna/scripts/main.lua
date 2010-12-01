local oldrequire = require
require = function(arg)
	local s,e = string.find(arg, "core/")
	if s then
		Program:load_extension(string.sub(arg, e + 1))
	else
		oldrequire(arg)
	end
end

if Program.args == "--help" or Program.args == "-h" then
	print("Usage: lipsofsuna lipsofsuna [options]\n")
	print("Options:")
	print("  --help                   Show this help message and exit.")
	print("  --host localhost <port>  Start a server and join it.")
	print("  --join <server> <port>   Join a remove server.")
	print("  --server                 Run as a dedicated server.")
	return
end

require "core/database"
require "core/network"
require "core/tiles"
require "core/tiles-physics"
Voxel.blocks_per_line = 2
Voxel.tiles_per_line = 14
require "common/bitwise"
require "common/effect"
require "common/eventhandler"
require "common/faction"
require "common/feat"
require "common/iconspec"
require "common/itemspec"
require "common/material"
require "common/model"
require "common/names"
require "common/object"
require "common/obstaclespec"
require "common/pattern"
require "common/protocol"
require "common/quest"
require "common/regionspec"
require "common/sectors"
require "common/species"
require "common/thread"
require "common/timer"

if Program.args == "--server" then
require "core/generator"
require "core/object-physics"
require "common/vision"
require "common/inventory"
require "common/skills"
else
require "core/graphics"
require "core/camera"
require "core/object-render"
require "core/render"
require "core/sound"
require "core/speech"
require "core/tiles-physics"
require "core/tiles-render"
require "core/reload"
require "core/widgets"
require "client/shader"
require "client/shaders/skeletal"
end

require "content/effects"
require "content/factions"
require "content/feats"
require "content/icons"
require "content/items"
require "content/materials"
require "content/obstacles"
require "content/patterns"
require "content/regions"
require "content/species"
require "content/quests/mourningadventurer"
require "content/quests/peculiarpet"
require "content/quests/rootsofworld"
require "content/quests/misc"

if Program.args == "--server" then
	require "server/main"
else
	require "client/main"
end
