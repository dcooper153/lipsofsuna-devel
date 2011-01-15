print "INFO: Loading server scripts."

require "server/util"
require "server/serialize"
require "server/controls"
require "server/crafting"
require "server/dialog"
require "server/actions/moveitem"
require "server/feat"
require "server/sectors"
require "server/objects/object"
require "server/objects/creature"
require "server/objects/container"
require "server/objects/decayer"
require "server/objects/door"
require "server/objects/fragile"
require "server/objects/item"
require "server/objects/mover"
require "server/objects/obstacle"
require "server/objects/player"
require "server/npcs/lastboss"
require "server/admin"
require "server/quest"
require "server/quests/mourningadventurer"
require "server/quests/lipscitizen"
require "server/quests/peculiarpet"
require "server/quests/rootsofworld"
require "server/modifier"
require "server/modifiers/bleeding"
require "server/editing"
require "server/particles"
require "server/generator"

local map_version = "1"

restart = function()
	Network.closed = true
	for k,v in pairs(Network.clients) do
		Network:disconnect{object = v}
	end
	-- Remove previous map.
	Marker:reset()
	Sectors.instance:unload_world()
	print("Generating map...")
	Generator:generate()
	-- Save map.
	print("Saving map...")
	Sectors.instance:save_world(true)
	Sectors.instance:unload_world()
	Serialize:set_value("map_version", map_version)
	-- Discard events emitted during map generation so that they
	-- don't trigger when the game starts.
	Program:update()
	repeat until not Program:pop_event()
	Program:update()
	repeat until not Program:pop_event()
	print("Done")
	Network.closed = false
end

local m = Material:find{name = "granite1"}
Voxel.fill = m.id
if Settings.generate or Serialize:get_value("map_version") ~= map_version then
	restart()
end
Network:host{port = 10101}

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

