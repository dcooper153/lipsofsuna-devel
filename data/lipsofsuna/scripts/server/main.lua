print "INFO: Loading server scripts."

require "server/util"
require "server/account"
require "server/serialize"
require "server/controls"
require "server/chat"
require "server/config"
require "server/crafting"
require "server/dialog"
require "server/actions/moveitem"
require "server/feat"
require "server/sectors"
require "server/objects/object"
require "server/objects/creature"
require "server/objects/door"
require "server/objects/item"
require "server/objects/obstacle"
require "server/objects/player"
require "server/objects/spell"
require "server/npcs/lastboss"
require "server/admin"
require "server/quest"
require "server/modifier"
require "server/editing"
require "server/particles"
require "server/generator"
require "server/generator/main"
for k,v in pairs(File:scan_directory("scripts/server/influences")) do
	require("server/influences/" .. string.gsub(v, "([^.]*).*", "%1"))
end
for k,v in pairs(File:scan_directory("scripts/server/modifiers")) do
	require("server/modifiers/" .. string.gsub(v, "([^.]*).*", "%1"))
end

local m = Material:find{name = "granite1"}
Voxel.fill = m.id
Network:host{port = Config.inst.server_port}
if Settings.generate or
   Serialize:get_value("map_version") ~= Generator.map_version or
   Serialize:get_value("data_version") ~= Serialize.data_version then
	Generator.inst:generate()
	Serialize:set_value("data_version", Serialize.data_version)
else
	Serialize:load()
end

-- Simulate.
Eventhandler{type = "object-motion", func = function(self, event)
	Vision:event{type = "object-moved", object = event.object}
end}
Eventhandler{type = "object-contact", func = function(self, event)
	if event.self.contact_cb then
		event.self:contact_cb(event)
	end
end}

-- Visibility.
Eventhandler{type = "object-visibility", func = function(self, event)
	if event.visible then
		Vision:event{type = "object-shown", object = event.object}
	else
		Vision:event{type = "object-hidden", object = event.object}
	end
end}

Voxel.block_changed_cb = function(index, stamp)
	Vision:event{type = "voxel-block-changed", index = index, point = ARGH, stamp = stamp}
end

Program.sleep = 1/60

-- Main loop.
while not Program.quit do
	-- Update program state.
	Program:update()
	-- Handle events.
	local event = Program:pop_event()
	while event do
		Eventhandler:event(event)
		event = Program:pop_event()
	end
end

-- Save at exit.
Serialize:save()
