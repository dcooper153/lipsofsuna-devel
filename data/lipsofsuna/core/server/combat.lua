--- Damage calculation and applying helpers.
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.server.combat
-- @alias Combat

local Class = require("system/class")
local Damage = require("core/server/damage")

--- Damage calculation and applying helpers.
-- @type Combat
local Combat = Class("Combat")

--- Calculates and applies the damage of a melee impact.
-- @param self Combat class.
-- @param attacker Actor who cast the spell.
-- @param weapon Item used for the attack, or nil.
-- @param point Impact point in world space.
-- @param defender Hit object, or nil.
-- @param tile Hit tile, or nil.
Combat.apply_melee_impact = function(self, attacker, weapon, point, defender, tile)
	-- Calculate the damage.
	local damage = Damage()
	if weapon then
		damage:add_item_influences(weapon, attacker.skills)
	else
		damage:add_barehanded_influences(attacker.skills)
	end
	damage:apply_attacker_physical_modifiers(attacker)
	damage:apply_attacker_charge(attacker:get_attack_charge())
	if defender then
		damage:apply_defender_armor(defender)
		damage:apply_defender_vulnerabilities(defender)
	end
	-- Play impact effects.
	for name in pairs(damage:get_impact_effects()) do
		Server:world_effect(point, name)
	end
	-- Apply object damage.
	if defender then
		-- Knockback the defender.
		defender.physics:impulse(Vector(0,0,-100):transform(attacker:get_rotation()))
		-- Stagger the attacker.
		if defender.blocking then
			if Program:get_time() - defender.blocking > defender.spec.blocking_delay then
				attacker.cooldown = (attacker.cooldown or 0) + 1
				attacker:animate("stagger")
			end
		end
		-- Damage the defender.
		local args = {owner = attacker, object = defender}
		for name,value in pairs(damage.influences) do
			local effect = Feateffectspec:find{name = name}
			if effect and effect.touch then
				args.value = value
				effect:touch(args)
			end
		end
	end
	-- Apply tile damage.
	if tile and weapon then
		-- Break the tile.
		if weapon.spec.categories["mattock"] then
			self:destroy_terrain_sphere(attacker, point, tile, 2)
			return
		end
		-- Damage the weapon.
		if weapon.spec.damage_mining then
			if not weapon:damaged{amount = 2 * weapon.spec.damage_mining * math.random(), type = "mining"} then
				attacker:send_message("The " .. weapon.spec.name .. " broke!")
			end
		end
	end
end

--- Applies the damage of a ranged impact.
-- @param self Combat class.
-- @param attacker Actor who cast the spell.
-- @param projectile Item used as the projectile.
-- @param damage Damage information.
-- @param point Impact point in world space.
-- @param defender Hit object, or nil.
-- @param tile Hit tile, or nil.
Combat.apply_ranged_impact = function(self, attacker, projectile, damage, point, defender, tile)
	-- Apply defender's status.
	if defender then
		damage:apply_defender_armor(defender)
		damage:apply_defender_vulnerabilities(defender)
	end
	-- Play impact effects.
	for name in pairs(damage:get_impact_effects()) do
		Server:world_effect(point, name)
	end
	-- Apply object damage.
	if defender then
		-- Knockback the defender.
		defender.physics:impulse(Vector(0,0,-100):transform(projectile:get_rotation()))
		-- Damage the defender.
		local args = {owner = attacker, object = defender}
		for name,value in pairs(damage.influences) do
			local effect = Feateffectspec:find{name = name}
			if effect and effect.touch then
				args.value = value
				effect:touch(args)
			end
		end
	end
end

--- Calculates the damage of a ranged impact.
-- @param self Combat class.
-- @param attacker Actor who cast the spell.
-- @param weapon Item used for the attack, or nil.
-- @param projectile Item used as the projectile.
-- @return Damage information.
Combat.calculate_ranged_damage = function(self, attacker, weapon, projectile)
	local damage = Damage()
	if weapon then
		damage:add_item_influences(weapon, attacker.skills)
	end
	if projectile then
		damage:add_item_influences(projectile, attacker.skills)
	end
	damage:apply_attacker_charge(attacker:get_attack_charge())
	return damage
end

--- Applies the effect of a ranged spell on impact.
-- @param self Combat class.
-- @param attacker Actor who cast the spell.
-- @param projectile Spell object used as the projectile.
-- @param effect Effectspec name.
-- @param point Impact point in world space.
-- @param defender Hit object, or nil.
-- @param tile Hit tile, or nil.
-- @return True if the effect is still alive.
Combat.apply_ranged_spell_impact = function(self, attacker, projectile, effect, point, defender, tile)
	-- Find the effect spec.
	local espec = Feateffectspec:find{name = effect}
	if not espec then return end
	-- Calculate the damage.
	local damage = Damage(espec.influences)
	damage:apply_defender_vulnerabilities(defender)
	-- Play impact effects.
	for name in pairs(damage:get_impact_effects()) do
		Server:world_effect(point, name)
	end
	-- Knockback the defender.
	if defender then
		defender.physics:impulse(Vector(0,0,-100):transform(projectile:get_rotation()))
	end
	-- Apply the damage.
	local absorb
	local args = {owner = attacker, object = defender, tile = tile}
	for name,value in pairs(damage.influences) do
		local effect = Feateffectspec:find{name = name}
		if effect and effect.ranged then
			args.value = value
			if not effect:ranged(args) then
				absorb = true
			elseif absorb == nil then
				absorb = false
			end
		end
	end
	return not absorb
end

--- Destroys terrain at the given impact point.
-- @param self Combat class.
-- @param attacker Actor who cast the spell.
-- @param tile Grid coordinates of the hit tile.
-- @param radius Radius of the destroyed sphere, in grid units.
Combat.destroy_terrain_sphere = function(self, attacker, point, tile, radius)
	-- FIXME: Old terrain.
	if tile.y ~= 0 then
		Voxel:damage(attacker, tile)
		return
	end
	-- Play the collapse effect.
	-- TODO: Should use material specs.
	Server:world_effect(point, "collapse2")
	-- Change the tile type.
	-- TODO: Should use material specs.
	local r = radius
	local cx = point.x / Game.terrain.grid_size
	local cz = point.z / Game.terrain.grid_size
	local x0 = math.floor(cx - r)
	local x1 = math.floor(cx + r)
	local z0 = math.floor(cz - r)
	local z1 = math.floor(cz + r)
	local f = function(dx, dz)
		local d = math.sqrt(dx^2 + dz^2) / r
		if d > 1 then return 0 end
		return math.cos(d * math.pi / 2) * r * Game.terrain.grid_size
	end
	for z = z0,z1 do
		for x = x0,x1 do
			local y = point.y
			local y00 = f(x - cx, z - cz)
			local y10 = f(x - cx + 1, z - cz)
			local y01 = f(x - cx, z - cz + 1)
			local y11 = f(x - cx + 1, z - cz + 1)
			Game.terrain.terrain:add_stick_corners(x, z,
				y - y00, y - y10, y - y01, y - y11,
				y + y00, y + y10, y + y01, y + y11, 0)
		end
	end
	-- Smoothen the modified columns.
	for z = z0-1,z1+1 do
		for x = x0-1,x1+1 do
			Game.terrain.terrain:calculate_smooth_normals(x, z)
		end
	end
	-- Create items.
	-- TODO: Should use material specs.
		--[[
		if m.mining_materials and user then
			for k,v in pairs(m.mining_materials) do
				for i = 1,v do
					local spec = Itemspec:find{name = k}
					local item = Item{spec = spec}
					user.inventory:merge_or_drop_object(item)
				end
			end
		end
		--]]
	-- Spawn random monsters.
	-- TODO: Should use material specs.
		--[[
		if not n and math.random() < 0.01 then
			local spec = Actorspec:random{category = "mining"}
			if spec then
				local offset = (point + Vector(0.5,0.1,0.5)) * Voxel.tile_size
				local object = Actor{random = true, spec = spec, position = offset, realized = true}
			end
		end
		--]]
	return true
end

--- Destroys terrain at the given impact point.
-- @param self Combat class.
-- @param attacker Actor who cast the spell.
-- @param tile Grid coordinates of the hit tile.
Combat.destroy_terrain_stick = function(self, attacker, point, tile, height)
	-- Play the collapse effect.
	-- TODO: Should use material specs.
	Server:world_effect(point, "collapse2")
	-- Change the tile type.
	-- TODO: Should use material specs.
	Game.terrain.terrain:add_stick(tile.x, tile.z, point.y - height / 2, height, 0)
	-- Smoothen the modified columns.
	for z = tile.z-1,tile.z+1 do
		for x = tile.x-1,tile.x+1 do
			Game.terrain.terrain:calculate_smooth_normals(x, z)
		end
	end
	return true
end

return Combat
