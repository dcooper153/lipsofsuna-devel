print "INFO: Loading server scripts."

require "common/unlocks"
require "server/util"
require "server/account"
require "server/serialize"
require "server/controls"
require "server/chat"
require "server/config"
require "server/crafting"
require "server/dialog"
require "server/event"
require "server/feat"
require "server/marker"
require "server/sectors"
require "server/admin"
require "server/quest"
require "server/modifier"
require "server/editing"
require "server/particles"
for k,v in pairs(File:scan_directory("server/actions")) do
	require("server/actions/" .. string.gsub(v, "([^.]*).*", "%1"))
end
for k,v in pairs(File:scan_directory("server/objects")) do
	require("server/objects/" .. string.gsub(v, "([^.]*).*", "%1"))
end
for k,v in pairs(File:scan_directory("server/influences")) do
	require("server/influences/" .. string.gsub(v, "([^.]*).*", "%1"))
end
require "server/generator"
require "server/generator/main"

Physics.GROUP_ACTORS = 0x0001
Physics.GROUP_ITEMS = 0x0002
Physics.GROUP_OBSTACLES = 0x1000
Physics.GROUP_STATICS = 0x2000
Physics.GROUP_HEIGHTMAP = 0x4000
Physics.GROUP_VOXELS = 0x8000

-- Initialize the heightmap.
if Map then Map:init() end

-- Initialize networking.
-- When the socket is up, we tell the local client that it can join.
Network:host{port = Config.inst.server_port}
Program:push_message{name = "join"}

-- Initialize the dynamic map.
if Settings.generate or Serialize:get_value("map_version") ~= Generator.map_version then
	Generator.inst:generate()
	Serialize:set_value("data_version", Serialize.data_version)
	Unlocks:init(Serialize.db)
else
	Serialize:load()
	Unlocks:init(Serialize.db)
	Unlocks:read_db()
end

-- Initialize unlock updates.
Unlocks:unlock("skill", "Health lv1")
Unlocks:unlock("skill", "Willpower lv1")
Unlocks:unlock("spell type", "ranged spell")
Unlocks:unlock("spell type", "spell on self")
Unlocks:unlock("spell effect", "fire damage")
Unlocks:unlock("spell effect", "light")
Unlocks:unlock("spell effect", "physical damage")
Unlocks:unlock("spell effect", "restore health")
Unlocks.changed = function(self)
	local p = Packet(packets.UNLOCK)
	self:write_packet(p)
	for k,v in pairs(Network.clients) do
		Network:send{client = v, packet = p}
	end
end
Unlocks:changed()

-- Initialize terrain updates.
Voxel.block_changed_cb = function(index, stamp)
	Vision:event{type = "voxel-block-changed", index = index, point = ARGH, stamp = stamp}
end

Program.sleep = 1/60
Program.profiling = {}

-- Main loop.
while not Program.quit do
	-- Update program state.
	local t1 = Program.time
	Program:update()
	local t2 = Program.time
	-- Handle events.
	local event = Program:pop_event()
	while event do
		Eventhandler:event(event)
		event = Program:pop_event()
	end
	local t3 = Program.time
	-- Store timings.
	Program.profiling.update = t2 - t1
	Program.profiling.event = t3 - t2
end

-- Save at exit.
Serialize:save()
