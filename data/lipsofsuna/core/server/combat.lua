local Class = require("system/class")
local Damage = require("core/server/damage")

local Combat = Class()

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
		defender:impulse{impulse = attacker.rotation * Vector(0, 0, -100)}
		-- Stagger the attacker.
		if defender.blocking then
			if Program.time - defender.blocking > defender.spec.blocking_delay then
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
			Voxel:damage(args.owner, args.tile)
		end
		-- Damage the weapon.
		if weapon.spec.damage_mining then
			if not weapon:damaged{amount = 2 * weapon.spec.damage_mining * math.random(), type = "mining"} then
				attacker:send_message("The " .. weapon.spec.name .. " broke!")
			end
		end
	end
end

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
		defender:impulse{impulse = projectile.rotation * Vector(0, 0, -100)}
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

return Combat
