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
		args.target:damaged(clss:calculate_physical_damage(args))
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
		args.target:damaged(clss:calculate_physical_damage(args))
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
--   <li>feat: Used feat or nil for attack.</li>
--   <li>point: Hit point in world space.</li>
--   <li>projectile: Fired object or nil.</li>
--   <li>target: Attacked creature or nil.</li>
--   <li>weapon: Used weapon or nil.</li></ul>
-- @return Damage.
Combat.calculate_physical_damage = function(clss, args)
	-- Base damage.
	-- The base damage depends on the feat and the type of weapon and ammunition used.
	local feat = args.feat or Feat:find{name = "attack"}
	local spec1 = args.weapon and args.weapon.itemspec or {}
	local spec2 = args.projectile and args.projectile.itemspec or {}
	local damage = feat.inflict_damage
	if spec1 or spec2 then
		damage = damage + (spec1.damage or 0) + (spec2.damage or 0)
	else
		damage = damage + 3
	end
	-- Damage bonus from skills.
	-- The bonus damage depends on the type of weapon and ammunition used.
	local bonuses
	local skills = Skills:find{owner = args.attacker}
	if not skills then return damage end
	if spec1 or spec2 then
		bonuses = {
			dexterity = (spec1.damage_bonus_dexterity or 0) + (spec2.damage_bonus_dexterity or 0),
			health = (spec1.damage_bonus_health or 0) + (spec2.damage_bonus_health or 0),
			intelligence = (spec1.damage_bonus_intelligence or 0) + (spec2.damage_bonus_intelligence or 0),
			perception = (spec1.damage_bonus_percention or 0) + (spec2.damage_bonus_percention or 0),
			strength = (spec1.damage_bonus_strength or 0) + (spec2.damage_bonus_strength or 0),
			willpower = (spec1.damage_bonus_willpower or 0) + (spec2.damage_bonus_willpower or 0)}
	else
		bonuses = {
			dexterity = 0.2,
			strength = 0.2,
			willpower = 0.1}
	end
	for k,v in pairs(bonuses) do
		if skills:has_skill{skill = k} then
			damage = damage + v * skills:get_value{skill = k}
		end
	end
	-- If there were penalties, make sure we do at least 1 point of damage.
	damage = math.max(1, damage)
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
