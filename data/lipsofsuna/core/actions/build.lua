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
		--feat:play_effects(args)
		Coroutine:sleep(attacker.spec.timing_build * 0.02)
		-- Check for a correct weapon.
		if not weapon.spec.construct_tile then return end
		-- Check for sufficient materials.
		local m = Material:find{name = weapon.spec.construct_tile}
		local need = weapon.spec.construct_tile_count or 1
		local have = weapon:get_count()
		if not m or need > have then return end
		-- Perform the ray cast.
		local src,dst = attacker:get_attack_ray()
		local r = Physics:cast_ray(src, dst)
		if not r or r.object then return end
		-- Find the affected tile.
		local t,p = Utils:find_build_point(r.point, attacker)
		if not t then return end
		-- Build the tile.
		weapon:subtract(need)
		Voxel:set_tile(p, m:get_id())
		Server:world_effect(p * Voxel.tile_size, m.effect_build)
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
