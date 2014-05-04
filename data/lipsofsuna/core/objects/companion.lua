--- Companion object.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.objects.companion
-- @alias Companion

local Actor = require("core/objects/actor")
local Class = require("system/class")

--- Companion object.
-- @type Companion
local Companion = Class("Companion", Actor)

--- Creates a new companion object.
-- @param clss Companion class.
-- @param manager Object manager.
-- @param id Unique object ID. Nil for a random free one.
-- @return Companion.
Companion.new = function(clss, manager, id)
	local self = Actor.new(clss, manager, id)
	return self
end

--- Respawns the companion.
-- @param self Object.
Companion.respawn = function(self)
	if not self.owner then return end
	self:set_position(self.owner:get_position():copy())
	self:set_visible(true)
end

--- Updates the state of the companion.
-- @param self Object.
-- @param secs Seconds since the last update.
Companion.update_server = function(self, secs)
	-- Hide ownerless companions.
	if not self.owner or not self.owner:get_visible() then
		self:detach()
		return
	end
	-- Calculate the distance to the owner.
	local pos
	local pos_owner = self.owner:get_position()
	if self:get_visible() then
		pos = self:get_position()
	else
		pos = pos_owner:copy():add(100000, 0, 0)
	end
	-- Respawn near the owner if too far away.
	if (pos - pos_owner).length > 10 then
		if not self:get_combat_active() and not self.owner:get_combat_active() then
			self:respawn()
		end
	end
	-- Resurrect when combat ends.
	if self:get_dead() and not self.owner:get_combat_active() then
		self:set_dead(false)
	end
	-- Update the base class.
	Actor.update_server(self, secs)
end

--- Sets the owner of the companion.
-- @param self Object.
-- @param owner Owner object.
Companion.set_owner = function(self, owner)
	self.owner = owner
end

Companion.get_storage_type = function(self)
	return "companion"
end

return Companion
