local Client = require("core/client/client")
local Physics = require("system/physics")
local Sound = require("system/sound")

Main.main_start_hooks:register(10, function(secs)
	-- Footstep sounds.
	Main.objects.object_update_hooks:register(9, function(object, secs)
		-- Check for an applicable actor.
		if not object.animated then return end
		local spec = object.spec
		if not spec or not spec.footstep_sound then return end
		-- Check for an applicable animation.
		if not object.render then return end
		local anim = object.render:get_animation{channel = 1}
		if not anim then return end
		if anim.animation == "dead" or anim.animation == "idle" then return end
		-- Find the foot anchors.
		-- These are needed for foot position tracking so that we know when and
		-- where to play the positional sound.
		local lnode = object:find_node{name = "#foot.L", space = "world"}
		if not lnode then return end
		local rnode = object:find_node{name = "#foot.R", space = "world"}
		if not rnode then return end
		-- Ground check.
		-- We don't want to play footsteps if the character is flying.
		if not Physics:cast_ray(lnode, Vector(0,-spec.footstep_height):add(lnode)) and
		   not Physics:cast_ray(rnode, Vector(0,-spec.footstep_height):add(rnode)) then
			object.lfoot_prev = nil
			object.rfoot_prev = nil
			return
		end
		-- Left foot.
		-- We play the sound when the node crosses from the local positive Z axis
		-- to the negative. Using the Y distance to the ground is too error prone
		-- so this approximation is the best approach I have found so far.
		object.lfoot_prev = object.lfoot_curr
		object.lfoot_curr = object:find_node{name = "#foot.L"}
		if object.lfoot_prev and object.lfoot_curr.z < 0 and object.lfoot_prev.z >= 0 then
			Client.effects:play_world(spec.footstep_sound, lnode)
		end
		-- Right foot.
		-- Works the same way with the left foot.
		object.rfoot_prev = object.rfoot_curr
		object.rfoot_curr = object:find_node{name = "#foot.R"}
		if object.rfoot_prev and object.rfoot_curr.z < 0 and object.rfoot_prev.z >= 0 then
			Client.effects:play_world(spec.footstep_sound, rnode)
		end
	end)
end)

Client:register_update_hook(15, function(secs)
	Main.timing:start_action("sound")
	-- Listener position.
	if Client.player_object then
		local p,r = Client.player_object:find_node{name = "#neck", space = "world"}
		if p then
			Sound:set_listener_position(p)
			Sound:set_listener_rotation(r)
		else
			Sound:set_listener_position(Client.player_object:get_position() + Vector(0,1.5,0))
			Sound:set_listener_rotation(Client.player_object:get_rotation())
		end
		local vel = Client.player_object:get_velocity()
		Sound:set_listener_velocity(vel)
	end
end)
