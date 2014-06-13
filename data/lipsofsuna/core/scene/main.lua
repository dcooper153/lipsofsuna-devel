local MovementPrediction = require("core/scene/movement-prediction")
local SceneObject = require("core/scene/scene-object")

Main.main_start_hooks:register(10, function(secs)
	Main.objects.object_created_hooks:register(10, function(object)
		object.render = SceneObject()
		if Main.game.enable_prediction then
			object.prediction = MovementPrediction()
		end
	end)
	Main.objects.object_detached_hooks:register(10, function(object)
		object.render:clear()
	end)
	Main.objects.object_update_hooks:register(10, function(object, secs)
		-- Interpolate the position.
		if object.prediction and object.prediction.enabled then
			object.prediction:update(secs)
			object:set_position(object.prediction:get_predicted_position())
			if object.dead or object ~= Main.client.player_object then
				object:set_rotation(object.prediction:get_predicted_rotation())
				object:set_tilt_angle(object.prediction:get_predicted_tilt())
			end
		end
		-- Update the render object.
		object.render:set_position(object:get_position())
		object.render:set_rotation(object:get_rotation())
	end)
end)

Main.update_hooks:register(10, function(secs)
	local p = Main.client.player_object
	if p then
		-- Refresh the active portion of the map.
		p:refresh()
		-- Notify the terrain manager of the view center.
		Main.terrain:set_view_center(p:get_position())
		-- FIXME
		Main.client.lighting:set_dungeon_mode(false)
	end
end)
