local Coroutine = require("system/coroutine")
local Material = require("system/material")
local Physics = require("system/physics")

local perform_attack = function(attacker)
	local weapon = attacker:get_weapon()
	if not weapon then return end
	Coroutine(function(t)
		-- Play the attack effect.
		Server:object_effect(attacker, "swing1")
		Server:object_event(attacker, "object attack", {move = "stand", variant = math.random(0, 255)})
		Coroutine:sleep(attacker.spec.timing_build * 0.02)
		-- Check for a correct weapon.
		if not weapon.spec.construct_tile then return end
		local material = TerrainMaterialSpec:find_by_name(weapon.spec.construct_tile)
		if not material then return end
		-- Perform the ray cast.
		local src,dst = attacker:get_attack_ray()
		local r = Physics:cast_ray(src, dst)
		if not r or r.object then return end
		-- Check for sufficient materials.
		local radius = 1.2
		local materials = Game.terrain.terrain:count_materials_in_sphere(r.point, radius)
		local need = materials[0]
		if not need then return end
		if need > weapon:get_count() then return end
		-- Build the tile.
		weapon:subtract(need)
		local p
		if r.normal.y > 0 then
			p = Vector(0,-radius/4,0):add(r.point)
		else
			p = Vector(0,radius/4,0):add(r.point)
		end
		Game.terrain.terrain:add_sphere_filter_id(p, radius, material.id, 0)
		Server:world_effect(r.point, material.effect_build)
	end)
end

Actionspec{
	name = "build",
	charge_start = function(user)
		perform_attack(user)
	end,
	func = function(feat, info, args)
		perform_attack(args.user)
	end}
