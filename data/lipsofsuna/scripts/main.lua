local oldrequire = require
require = function(arg)
	local s,e = string.find(arg, "core/")
	if s then
		Program:load_extension(string.sub(arg, e + 1))
	else
		oldrequire(arg)
	end
end

require "core/network"
require "core/tiles"
require "core/tiles-physics"
Voxel.blocks_per_line = 4
Voxel.tiles_per_line = 16
require "common/bitwise"
require "common/effect"
require "common/eventhandler"
require "common/faction"
require "common/feat"
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
require "common/species"
require "common/thread"
require "common/timer"

if Program.args == "--server" then
	require "server/main"
else
	require "client/main"
end
