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
require "server/npcs/lastboss"
require "server/admin"
require "server/quest"
require "server/quests/charasillusion"
require "server/quests/erinyes"
require "server/quests/lipscitizen"
require "server/quests/mourningadventurer"
require "server/quests/peculiarpet"
require "server/quests/portaloflips"
require "server/quests/portalofmidguard"
require "server/quests/rootsofworld"
require "server/quests/sanctuary"
require "server/modifier"
require "server/modifiers/bleeding"
require "server/modifiers/plague"
require "server/modifiers/sanctuary"
require "server/editing"
require "server/particles"
require "server/generator"

local m = Material:find{name = "granite1"}
Voxel.fill = m.id
Network:host{port = 10101}
if Settings.generate or
   Serialize:get_value("map_version") ~= Generator.map_version or
   Serialize:get_value("data_version") ~= Serialize.data_version then
	Generator:generate()
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
