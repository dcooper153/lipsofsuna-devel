Combat = Class()

--- Applies melee damage.
-- @param clss Combat class.
-- @param args Arguments.<ul>
--   <li>attacker: Attacking creature.</li>
--   <li>feat: Used feat.</li>
--   <li>point: Hit point in world space.</li>
--   <li>target: Attacked creature.</li>
--   <li>tile: Attacked tile or nil.</li>
--   <li>weapon: Used weapon.</li></ul>
Combat.apply_melee_hit = function(clss, args)
	-- Effects.
	if args.target then
		args.target:effect{effect = "thud-000"}
		if args.point then Particles:create(args.point, "default") end
	end
	-- Impulse.
	if args.target and args.attacker then
		args.target:impulse{impulse = args.attacker.rotation * Vector(0, 0, -100)}
	end
	-- Damage target.
	if args.target then
		args.target:damaged(clss:calculate_melee_damage(args))
	end
	if args.tile then
		if args.weapon and args.weapon.itemspec.name == "mattock" then
			Voxel:damage(args.tile, args.damage, "mattock")
		end
	end
end

--- Applies ranged damage.
-- @param clss Combat class.
-- @param args Arguments.<ul>
--   <li>attacker: Attacking creature.</li>
--   <li>feat: Used feat.</li>
--   <li>point: Hit point in world space.</li>
--   <li>projectile: Fired object. (required)</li>
--   <li>target: Attacked object or nil.</li>
--   <li>tile: Attacked tile or nil.</li></ul>
Combat.apply_ranged_hit = function(clss, args)
	-- Effects.
	if args.target then
		args.target:effect{effect = "thud-000"}
		if args.point then Particles:create(args.point, "default") end
	end
	-- Impulse.
	if args.target and args.projectile then
		args.target:impulse{impulse = args.projectile.rotation * Vector(0, 0, -100)}
	end
	-- Damage target.
	if args.target then
		args.target:damaged(clss:calculate_ranged_damage(args))
	end
	if args.tile then
		-- TODO: Terrain hits
	end
	-- Destroy projectile.
	if args.projectile then
		args.projectile:die()
	end
end

--- Applies spell damage.
-- @param clss Combat class.
-- @param args Arguments.<ul>
--   <li>attacker: Attacking creature.</li>
--   <li>feat: Used feat.</li>
--   <li>point: Hit point in world space.</li>
--   <li>target: Attacked creature.</li></ul>
Combat.apply_spell_hit = function(clss, args)
	-- Damage target.
	local damage = clss:calculate_spell_damage(args)
	if damage > 0 then
		args.target:damaged(damage)
	elseif damage < 0 then
		local skills = args.target.skills
		if skills and skills:has_skill{skill = "health"} then
			local val = skills:get_value{skill = "health"}
			local max = skills:get_maximum{skill = "health"}
			skills:set_value{skill = "health", value = math.max(val - damage, max)}
		end
	end
end

--- Calculates the damage of a melee attack.
-- @param clss Combat class.
-- @param args Arguments.<ul>
--   <li>attacker: Attacking creature.</li>
--   <li>feat: Used feat.</li>
--   <li>point: Hit point in world space.</li>
--   <li>target: Attacked creature. (required)</li>
--   <li>weapon: Used weapon.</li></ul>
-- @return Damage.
Combat.calculate_melee_damage = function(clss, args)
	-- Feat bonus.
	local feat = args.feat or Feat:find{name = "attack"}
	local damage = feat.inflict_damage
	-- Skill bonus.
	local skills = Skills:find{owner = args.attacker}
	if skills and skills:has_skill{skill = "strength"} then
		damage = damage + 0.2 * skills:get_value{skill = "strength"}
	end
	-- Weapon bonus.
	if args.weapon then
		damage = damage + 10
	end
	return damage
end

--- Calculates the damage of a ranged attack.
-- @param clss Combat class.
-- @param args Arguments.<ul>
--   <li>attacker: Attacking creature.</li>
--   <li>feat: Used feat.</li>
--   <li>point: Hit point in world space.</li>
--   <li>projectile: Fired object. (required)</li>
--   <li>target: Attacked creature. (required)</li></ul>
-- @return Damage.
Combat.calculate_ranged_damage = function(clss, args)
	-- Feat bonus.
	local feat = args.feat or Feat:find{name = "attack"}
	local damage = feat.inflict_damage
	-- Skill bonus.
	local skills = Skills:find{owner = args.attacker}
	if skills and skills:has_skill{skill = "dexterity"} then
		damage = damage + 0.2 * skills:get_value{skill = "dexterity"}
	end
	-- Weapon bonus.
	damage = damage + 10
	return damage
end

--- Calculates the damage of a spell.
-- @param clss Combat class.
-- @param args Arguments.<ul>
--   <li>attacker: Attacking creature.</li>
--   <li>feat: Used feat.</li>
--   <li>point: Hit point in world space.</li>
--   <li>target: Attacked creature. (required)</li></ul>
-- @return Damage.
Combat.calculate_spell_damage = function(clss, args)
	-- Feat bonus.
	local feat = args.feat or Feat:find{name = "attack"}
	local damage = feat.inflict_damage
	-- Skill bonus.
	local skills = Skills:find{owner = args.attacker}
	if skills and skills:has_skill{skill = "willpower"} then
		damage = damage + 0.2 * skills:get_value{skill = "willpower"}
	end
	-- Weapon penalty.
	local slots = Slots:find{owner = args.attacker}
	if slots then
		local weaponl = slots:get_object{slot = "hand.L"}
		local weaponr = slots:get_object{slot = "hand.R"}
		if weaponl or weaponr then
			-- TODO: Should only get penalty from metal weapons.
			damage = damage * 0.5
		end
	end
	return damage
end
