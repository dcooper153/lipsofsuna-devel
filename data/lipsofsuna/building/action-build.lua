local Physics = require("system/physics")
local TerrainMaterialSpec = require("core/specs/terrain-material")

Actionspec{
	name = "build",
	categories = { ["build"] = true },
	start = function(action, move)
		if action.object.cooldown then return end
		-- Initialize timing.
		action.time = 0
		action.duration = action.object.spec.timing_build * 0.02
		-- Play the start effect.
		Main.vision:object_effect(action.object, "swing1")
		Main.vision:object_event(action.object, "object attack", {move = "stand", variant = math.random(0, 255)})
		-- Enable effect-over-time updates.
		return true
	end,
	update = function(action, secs)
		-- Wait for the delay to end.
		action.object.cooldown = 0.1
		action.time = action.time + secs
		if action.time <= action.duration then return true end
		-- Check for a correct weapon.
		local weapon = action.object:get_weapon()
		if not weapon then return end
		if not weapon.spec.construct_tile then return end
		local material = TerrainMaterialSpec:find_by_name(weapon.spec.construct_tile)
		if not material then return end
		-- Perform the ray cast.
		local src,dst = Main.combat_utils:get_attack_ray_for_actor(action.object)
		local r = Physics:cast_ray(src, dst)
		if not r or r.object then return end
		-- Check for sufficient materials.
		local radius = 0.5
		local terr = Main.terrain.terrain
		local tile = Vector(r.point.x / terr.grid_size, r.point.y, r.point.z / terr.grid_size)
		local materials = terr:count_column_materials(tile.x, tile.z, tile.y - radius, 2 * radius)
		local need = materials[0]
		if not need then return end
		need = math.floor(10 * need) / 10
		if need > weapon:get_count() then return end
		-- Build the tile.
		weapon:subtract(need)
		terr:add_stick_filter_id(tile.x, tile.z, tile.y - radius, 2 * radius, material.id, 0)
		Main.vision:world_effect(r.point, material.effect_build)
	end,
	get_score = function(action)
		return 1
	end}
