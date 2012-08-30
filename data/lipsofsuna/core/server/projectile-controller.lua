--- Provides projectile movement to objects.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.server.projectile_controller
-- @alias ProjectileController

local Class = require("system/class")
local Combat = require("core/server/combat")

--- Provides projectile movement to objects.
-- @type ProjectileController
local ProjectileController = Class("ProjectileController")

--- Creates a new projectile controller.
-- @param clss ProjectileController class.
-- @param attacker Actor who fired the projectile.
-- @param projectile Contrelled item.
-- @param damage Damage information.
-- @param speed Flight speed.
-- @param speedline True to instructs clients to render a speedline effect.
-- @return ProjectileController.
ProjectileController.new = function(clss, attacker, projectile, damage, speed, speedline)
	local self = Class.new(clss)
	self.attacker = attacker
	self.projectile = projectile
	self.damage = damage
	self.speed = speed or 20
	self.speedline = speedline
	return self
end

--- Activates the controller and attaches it to the object.
-- @param self ProjectileController.
ProjectileController.attach = function(self)
	-- Set the projectile controls.
	self.projectile.controller = self
	self.projectile.physics:set_gravity(self.projectile.spec.gravity_projectile)
	self.projectile.speedline = speedline
	-- Add the projectile to the world.
	local src,dst = self.attacker:get_attack_ray()
	self.projectile:detach()
	self.projectile:set_position(src)
	if self.attacker.tilt then
		self.projectile:set_rotation(self.attacker:get_rotation() * self.attacker.tilt)
	else
		self.projectile:set_rotation(self.attacker:get_rotation():copy())
	end
	self.projectile:set_velocity(dst:subtract(src):normalize():multiply(self.speed))
	self.projectile:set_visible(true)
	-- Enable the destruction timer or contact events.
	if self.projectile.spec.destroy_timer then
		self.destruction_timer = self.projectile.spec.destroy_timer
		self.projectile.physics:set_contact_events(false)
	else
		self.projectile.physics:set_contact_events(true)
	end
end

--- Deactivates the controller and removes it from the object.
-- @param self ProjectileController.
ProjectileController.detach = function(self)
	self.projectile.physics:set_contact_events(false)
	self.projectile.controller = nil
	self.projectile.physics:set_gravity(self.projectile.spec.gravity)
	self.projectile:detach()
end

--- Handles physics contacts.
-- @param self ProjectileController.
-- @param result Contact result.
ProjectileController.handle_contact = function(self, result)
	-- Apply the damage.
	if result.object == self.attacker then return end
	Combat:apply_ranged_impact(self.attacker, self.projectile, self.damage, result.point, result.object, result.tile)
	-- Detach the projectile.
	self:detach()
end

--- Updates the controller.
-- @param self ProjectileController.
-- @param secs Seconds since the last update.
ProjectileController.update = function(self, secs)
	-- Update the destruction timer.
	if self.destruction_timer then
		self.destruction_timer = self.destruction_timer - secs
		if self.destruction_timer <= 0 then
			self.projectile:die()
			self:detach()
		end
	end
end

return ProjectileController
