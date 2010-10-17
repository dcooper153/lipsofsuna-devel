Client.title = "Lips of Suna"
Reload.enabled = true

Eventhandler{type = "select", func = function(self, args)
	args.object.selected = not args.object.selected
end}

Player = Class()
Player.light = Light{ambient = {1.0,1.0,1.0,0.3}, diffuse={1.0,1.0,1.0,1.0}, equation={3,0.4,0.04}}
Player.species = "aer" -- FIXME

local radian_wrap = function(x)
	if x < -math.pi then
		return math.pi - math.mod(x, math.pi)
	elseif x > math.pi then
		return -math.pi + math.mod(x, math.pi)
	else
		return x
	end
end

Player.camera = Camera{far = 60.0, mode = "third-person", near = 0.3}
Player.camera_tilt = 0
Player.camera_tilt_state = 0
Player.camera_turn = 0
Player.camera_turn_state = 0
Player.update_camera = function(clss, secs)
	-- Update turning.
	clss.camera_turn_state = clss.camera_turn_state + clss.camera_turn * secs
	clss.camera_turn_state = radian_wrap(clss.camera_turn_state)
	clss.camera_turn = 0
	-- Update tilting.
	clss.camera_tilt_state = clss.camera_tilt_state - clss.camera_tilt * secs
	clss.camera_tilt_state = radian_wrap(clss.camera_tilt_state)
	clss.camera_tilt = 0
	-- Set the target transformation.
	local turn = clss.camera_turn_state + clss.turn_state
	local tilt = clss.camera_tilt_state - clss.tilt_state
	Player.camera.target_position = Player.object.position + Vector(0, 2, 0)
	Player.camera.target_rotation = Quaternion:new_euler(turn, 0, tilt)
	-- Update the first person view.
	if Camera.mode == "first-person" then
		local node,rot = Player.object:find_node{name = "#camera"}
		if node then
			Player.camera.target_position = Player.object.position + Player.object.rotation * node
			-- TODO: Need a separate player model for first person view to allow intersections
		end
	end
	-- Interpolate.
	Player.camera.viewport = {Gui.scene.x, Gui.scene.y, Gui.scene.width, Gui.scene.height}
	Player.camera:update(secs)
end

Player.tilt = 0
Player.turn = 0
Player.tilt_state = 0
Player.turn_state = 0
Player.rotation_curr = Quaternion()
Player.rotation_prev = Quaternion()
Player.update_rotation = function(clss, secs)
	-- Update turning.
	clss.turn_state = clss.turn_state + clss.turn * secs
	clss.turn_state = radian_wrap(clss.turn_state)
	clss.object.rotation = Quaternion:new_euler(clss.turn_state, 0, 0)
	clss.turn = 0
	-- Update tilting.
	clss.tilt_state = clss.tilt_state + clss.tilt * secs
	clss.tilt_state = math.min(0.4 * math.pi, clss.tilt_state)
	clss.tilt_state = math.max(-0.4 * math.pi, clss.tilt_state)
	clss.tilt = 0
	-- Tilt the back of the character.
	local quat = Quaternion{axis = Vector(1,0,0), angle = clss.tilt_state}
	clss.object:edit_pose{channel = Animation.CHANNEL_CUSTOMIZE, node = "back", rotation = quat}
	-- Sync rotation with the server.
	local r = Quaternion:new_euler(clss.turn_state, 0, -clss.tilt_state)
	clss.rotation_curr = r
	if (clss.rotation_prev - r).length > 0.1 then
		clss.rotation_prev = r
		Network:send{packet = Packet(packets.PLAYER_TURN, "float", r.x, "float", r.y, "float", r.z, "float", r.w)}
	end
end

Player.update_pose = function(clss, secs)
	if clss.object then
		-- Create the customization animation.
		Player.object:animate{animation = "empty", channel = Animation.CHANNEL_CUSTOMIZE,
			weight = 10.0, permanent = true}
	end
end

Eventhandler{type = "tick", func = function(self, args)
	if Player.object then
		Player:update_pose(args.secs)
		Player:update_rotation(args.secs)
		Player:update_camera(args.secs)
		-- Update the light ball.
		Player.light.position = Player.object.position + Player.object.rotation * Vector(0, 2, -5)
		Player.light.enabled = true
		-- Sound playback.
		Sound.listener_position = Player.camera.position
		Sound.listener_rotation = Player.camera.rotation
		-- TODO: Sound.listener_velocity = ...
	end
end}

Eventhandler{type = "object-model", func = function(self, args)
	--args.object:update_model{model = args.model}
end}

Protocol:add_handler{type = "FEAT_UNLOCK", func = function(event)
	local ok,n = event.packet:read("string")
	if ok then
		local feat = Feat:find{name = n}
		if not feat then return end
		Gui.chat_history:append{text = "Unlocked feat " .. feat.name}
		feat.locked = false
	end
end}

Protocol:add_handler{type = "INVENTORY_CREATED", func = function(event)
	local ok,id,size,own = event.packet:read("uint32", "uint8", "bool")
	if ok then Container:create(id, size, own) end
end}

Protocol:add_handler{type = "INVENTORY_CLOSED", func = function(event)
	local ok,id = event.packet:read("uint32")
	if ok then Container:close(id) end
end}

Protocol:add_handler{type = "INVENTORY_ITEM_ADDED", func = function(event)
	local ok,id,slot,model,name = event.packet:read("uint32", "uint8", "uint32", "string")
	if ok then Container:insert_item(id, slot, model, name) end
end}

Protocol:add_handler{type = "INVENTORY_ITEM_REMOVED", func = function(event)
	local ok,id,slot = event.packet:read("uint32", "uint8")
	if ok then Container:remove_item(id, slot) end
end}

Protocol:add_handler{type = "MESSAGE", func = function(event)
	local ok,msg = event.packet:read("string")
	if ok then
		Gui.chat_history:append{text = msg}
	end
end}

Protocol:add_handler{type = "PARTICLE_RAY", func = function(event)
	local ok,sx,sy,sz,dx,dy,dz,t = event.packet:read("float", "float", "float", "float", "float", "float", "float")
	if ok then
		--Effect:ray{src = Vector(sx, sy, sz), dst = Vector(dx, dy, dz), life = 0.5}
	end
end}

-- Vision management.

Protocol:add_handler{type = "OBJECT_ANIMATED", func = function(event)
	local ok,i,a,c,perm,time,weig = event.packet:read("uint32", "string", "uint8", "bool", "float", "float")
	if ok then
		local o = Object:find{id = i}
		if o then
			local anim = (a ~= "" and a or nil)
			local chan = (c < 255 and c or nil)
			o:animate{animation = anim, channel = chan, fade_in = 0.3, fade_out = 0.3, permanent = perm, time = time, weight = weig}
		end
	end
end}

Protocol:add_handler{type = "OBJECT_DAMAGE", func = function(args)
end}

Protocol:add_handler{type = "OBJECT_EFFECT", func = function(event)
	local ok,i,e = event.packet:read("uint32", "string")
	if ok then
		-- Find the object.
		local obj = Object:find{id = i}
		if not obj then return end
		-- Find the effect.
		local effect = Effect:find{name = e}
		if not effect then return end
		-- Create an effect object.
		EffectObject{
			model = effect.model,
			object = obj,
			sound = effect.sound,
			realized = true}
	end
end}

Protocol:add_handler{type = "OBJECT_HIDDEN", func = function(event)
	local ok,i = event.packet:read("uint32")
	if ok then
		local o = Object:find{id = i}
		if o then o.realized = false end
	end
end}

Protocol:add_handler{type = "OBJECT_MOVED", func = function(event)
	local ok,i,x,y,z,rx,ry,rz,rw = event.packet:read("uint32", "float", "float", "float", "float", "float", "float", "float")
	if ok then
		local o = Object:find{id = i}
		if o then
			o.position = Vector(x, y, z)
			o.rotation = Quaternion(rx, ry, rz, rw)
		end
	end
end}

Protocol:add_handler{type = "OBJECT_SELF", func = function(event)
	local ok,i = event.packet:read("uint32")
	if ok then
		local o = Object:find{id = i}
		if o then
			Player.species = o.race or "aer"
			Camera.mode = "third-person"
		end
		Player.object = o
	end
end}

Protocol:add_handler{type = "OBJECT_SHOWN", func = function(event)
	local ok,i,t,m,n,x,y,z,rx,ry,rz,rw = event.packet:read("uint32", "uint8", "string", "string", "float", "float", "float", "float", "float", "float", "float")
	if not ok then return end
	-- Create the object.
	local type = (t == 0 and "creature") or (t == 1 and "item") or (t == 2 and "obstacle") or "object"
	local o = Object{id = i, model = m, name = n, position = Vector(x, y, z), position_smoothing = 0.5, rotation = Quaternion(rx, ry, rz, rw), rotation_smoothing = 0.5, realized = true, type = type}
	-- Apply optional customizations.
	local ok,ra,ge,hs,hc = event.packet:resume("string", "string", "string", "string")
	if ok then
		o.race = ra
		o.gender = ge
		o.hair_style = hs
		o.hair_color = hc
	else
		o.race = nil
		o.gender = nil
		o.hair_style = nil
		o.hair_color = nil
	end
	-- Rebuild the model.
	o:update_model()
end}

Protocol:add_handler{type = "OBJECT_SKILL", func = function(event)
	local ok,i,s,v,m = event.packet:read("uint32", "string", "int32", "int32")
	if ok then
		-- Find the affected object.
		local o = Object:find{id = i}
		if not o then return end
		-- Update player skills.
		if o == Player.object then
			Skills:update(s, v, m)
		end
		-- Display health changes.
		if s == "health" then
			if o.health then
				local diff = v - o.health
				if math.abs(diff) >= 2 then
					local red = {1,0,0,1}
					local green = {0,1,0,1}
					EffectObject{
						life = 3,
						object = o,
						position = Vector(0,2,0),
						realized = true,
						text = tostring(diff),
						text_color = diff > 0 and green or red,
						text_fade_time = 1,
						text_font = "medium",
						velocity = Vector(0,0.5,0)}
				end
			end
			o.health = v
		end
	end
end}

Protocol:add_handler{type = "OBJECT_SLOT", func = function(event)
	local ok,i,slot,node,model,name = event.packet:read("uint32", "string", "string", "string", "string")
	if ok then
		local o = Object:find{id = i}
		if not o then return end
		model = (model ~= "" and model or nil)
		if node ~= "" then
			-- Add-on equipment.
			local slots = Slots:find{owner = o} or Slots:new{owner = o}
			slots:set_object{node = node, model = model}
		else
			-- Replacer equipment.
			local special =
			{
				["upperbody"] = true,
				["lowerbody"] = true,
				["feet"] = true
			}
			if special[slot] then
				o.equipment = o.equipment or {}
				o.equipment[slot] = name
				o:update_model()
			end
		end
	end
end}

Protocol:add_handler{type = "OBJECT_SPEECH", func = function(event)
	local ok,i,m = event.packet:read("uint32", "string")
	if ok then
		local o = Object:find{id = i}
		if o then
			local n = o.name or "???"
			Speech:add{object = o, message = m}
			Gui.chat_history:append{text = "<" .. n .. "> " .. m}
			Sound:effect{object = o, effect = "spring-000"}
		end
	end
end}

Protocol:add_handler{type = "VOXEL_DIFF", func = function(event)
	Voxel:set_block{packet = event.packet}
end}

Protocol:add_handler{type = "WORLD_EFFECT", func = function(event)
	local ok,t,x,y,z = event.packet:read("string", "float", "float", "float")
	if ok then
		-- Find the effect.
		local effect = Effect:find{name = t}
		if not effect then return end
		-- Create the effect object.
		local o = Object{position=Vector(x, y, z), model = effect.model, realized = true}
		o:particle_animation{loop = false}
		-- Attach an optional sound effect.
		if effect.sound then
			Sound:effect{object = o, effect = effect.sound, pitch = 0.8+0.4*math.random()}
		end
		-- Remove the object when the playback has ended.
		Timer{delay = 10, func = function(self)
			o.realized = false
			self:disable()
		end}
	end
end}
