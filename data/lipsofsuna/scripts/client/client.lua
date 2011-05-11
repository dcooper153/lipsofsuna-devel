Client.title = "Lips of Suna"
Reload.enabled = true

Eventhandler{type = "select", func = function(self, args)
	args.object.selected = not args.object.selected
end}

Eventhandler{type = "object-model", func = function(self, args)
	--args.object:update_model{model = args.model}
end}

Protocol:add_handler{type = "ADMIN_PRIVILEGE", func = function(event)
	local ok,b = event.packet:read("bool")
	if not ok then return end
	Gui:set_admin(b)
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
	local ok,i,a,t = event.packet:read("uint32", "string", "float")
	if not ok then return end
	local o = Object:find{id = i}
	if not o then return end
	local args = {animation = a, fade_in = 0.3, fade_out = 0.3, time = t}
	local anim = o.spec and o.spec.animations[a]
	if anim then
		for k,v in pairs(anim) do args[k] = v end
	end
	o:animate(args)
end}

Protocol:add_handler{type = "OBJECT_DAMAGE", func = function(args)
end}

Protocol:add_handler{type = "OBJECT_DEAD", func = function(args)
	local ok,i,b = args.packet:read("uint32", "bool")
	if not ok then return end
	-- Find the object.
	local obj = Object:find{id = i}
	if not obj then return end
	-- Update death status.
	if obj.dead == b then return end
	obj.dead = b
	if obj.rotation_real then
		obj:update_rotation(obj.rotation_real)
	end
end}

Protocol:add_handler{type = "OBJECT_FEAT", func = function(event)
	local ok,i,a = event.packet:read("uint32", "string")
	if not ok then return end
	-- Find the object.
	local obj = Object:find{id = i}
	if not obj then return end
	if not obj.spec then return end
	if not obj.spec.animations then return end
	-- Find the feat animation.
	local anim = Featanimspec:find{name = a}
	if not anim then return end
	-- Find the character animation.
	-- The animation of the feat may be overridden by a weapon specific
	-- animation if there's a weapon in the slot used by the feat.
	local animation = anim.animation
	if anim.slot and obj.equipment then
		local weapon = Itemspec:find{name = obj.equipment[anim.slot]}
		if weapon and weapon.animation_attack then
			animation = weapon.animation_attack
		end
	end
	-- Play the animation.
	obj:animate_spec(animation)
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
			sound_delay = effect.sound_delay,
			sound_pitch = effect.sound_pitch,
			sound_positional = effect.sound_positional,
			realized = true}
		-- Quake the camera.
		Player:apply_quake(obj.position, effect.quake)
	end
end}

Protocol:add_handler{type = "OBJECT_HIDDEN", func = function(event)
	local ok,i = event.packet:read("uint32")
	if ok then
		local o = Object:find{id = i}
		if o then o:detach() end
	end
end}

Protocol:add_handler{type = "OBJECT_MOVED", func = function(event)
	local ok,i,x,y,z,tilt,rx,ry,rz,rw,vx,vy,vz = event.packet:read("uint32", "float", "float", "float", "float", "float", "float", "float", "float", "float", "float", "float")
	if ok then
		local o = Object:find{id = i}
		if not o then return end
		-- Apply position correction.
		-- Objects controlled by physics would normally float in the air
		-- due to collision margins so we need to apply compensation.
		local p = Vector(x, y, z)
		if o.type == "item" or o.type == "species" then p = p + Object.physics_position_correction end
		-- Set the target interpolation position.
		o:set_motion_state(p, Quaternion(rx, ry, rz, rw), Vector(vx, vy, vz), tilt)
	end
end}

Protocol:add_handler{type = "OBJECT_SELF", func = function(event)
	local ok,i = event.packet:read("uint32")
	if ok then
		local o = Object:find{id = i}
		if o then
			o.collision_group = Physics.GROUP_PLAYER
			Player.species = o.race or "aer"
			Camera.mode = "third-person"
			Gui.skills:set_species(o.spec)
			Views.Feats.inst:set_race(Player.species)
			Views.Feats.inst:show(1)
		end
		Player.object = o
	end
end}

Protocol:add_handler{type = "OBJECT_SHOWN", func = function(event)
	local ok,i,f,t,s,m,n,x,y,z,tilt,rx,ry,rz,rw = event.packet:read("uint32", "uint8", "string", "string", "string", "string", "float", "float", "float", "float", "float", "float", "float", "float")
	if not ok then return end
	-- Get the object specification.
	local spec
	if t == "item" then spec = Itemspec:find{name = s}
	elseif t == "obstacle" then spec = Obstaclespec:find{name = s}
	elseif t == "species" then spec = Species:find{name = s} end
	if spec and n == "" then n = spec.name end
	-- Apply position correction.
	-- Objects controlled by physics would normally float in the air
	-- due to collision margins so we need to apply compensation.
	local p = Vector(x, y, z)
	if t == "item" or t == "species" then p = p + Object.physics_position_correction end
	-- Create the object.
	local o = Object{id = i, model = m, name = n, position = p, spec = spec, type = t,
		collision_group = Physics.GROUP_OBJECT}
	if t == "species" then o.race = s end
	-- Apply optional customizations.
	local ok,de,bo,no,bu,eye,eyer,eyeg,eyeb,face1,face2,face3,face4,face5,hair,hairr,hairg,hairb,skin,skinr,sking,skinb = event.packet:resume(
		"bool", "float", "float", "float",
		"string", "uint8", "uint8", "uint8",
		"uint8", "uint8", "uint8", "uint8", "uint8",
		"string", "uint8", "uint8", "uint8",
		"string", "uint8", "uint8", "uint8")
	if ok then
		o.dead = de
		o.body_scale = bo
		o.bust_scale = bu
		o.nose_scale = no
		o.eye_style = eye
		o.eye_color = {eyer / 255, eyeg / 255, eyeb / 255}
		o.face_style = {face1 / 255, face2 / 255, face3 / 255, face4 / 255, face5 / 255}
		o.hair_style = hair
		o.hair_color = {hairr / 255, hairg / 255, hairb / 255}
		o.skin_style = skin
		o.skin_color = {skinr / 255, sking / 255, skinb / 255}
	else
		o.dead = nil
		o.body_scale = nil
		o.bust_scale = nil
		o.nose_scale = nil
		o.eye_style = nil
		o.eye_color = nil
		o.hair_style = nil
		o.hair_color = nil
		o.skin_style = nil
		o.skin_color = nil
	end
	-- Rebuild the model.
	o:update_model()
	o:update_rotation(Quaternion(rx, ry, rz, rw), tilt)
	o.realized = true
	-- Initialize speed lines.
	if Bitwise:band(f, Protocol.object_flags.SPEEDLINE) ~= 0 then
		o.speedline = Speedline(o)
	end
end}

Protocol:add_handler{type = "OBJECT_SKILL", func = function(event)
	local ok,i,s,v,m = event.packet:read("uint32", "string", "int32", "int32")
	if ok then
		-- Find the affected object.
		local o = Object:find{id = i}
		if not o then return end
		-- Update player skills.
		if o == Player.object then
			Gui.skills:update(s, v, m)
		end
		-- Display health changes.
		if s == "health" then
			if o.health then
				-- Show a health change text.
				local diff = v - o.health
				if math.abs(diff) > 2 then
					local code = (diff > 0 and 0x01 or 0x00) + (o == Player.object and 0x10 or 0x00)
					local colors = {
						[0x00] = {1,1,0,1},
						[0x01] = {0,1,1,1},
						[0x10] = {1,0,0,1},
						[0x11] = {0,1,0,1}}
					EffectObject{
						life = 3,
						object = o,
						position = Vector(0,2,0),
						realized = true,
						text = tostring(diff),
						text_color = colors[code],
						text_fade_time = 1,
						text_font = "medium",
						velocity = Vector(0,0.5,0)}
				end
				-- Quake the camera if the player was hurt.
				if o == Player.object and diff < -5 then
					Player:apply_quake(o.position, 0.01 * (5 - diff))
				end
			end
			o.health = v
			-- Set the correct collision shape.
			-- Dead creatures have a different collision shape. We switch between
			-- the two when the health changes between zero and non-zero.
			if o.health == 0 and o.animated then
				o.shape = "dead"
			else
				o.shape = "default"
			end
		end
	end
end}

Protocol:add_handler{type = "OBJECT_SLOT", func = function(event)
	local ok,i,count,spec,slot = event.packet:read("uint32", "uint32", "string", "string")
	if ok then
		local o = Object:find{id = i}
		if not o then return end
		if not o.slots then o.slots = Slots() end
		local slots = o.slots
		spec = Itemspec:find{name = spec}
		-- Update the model.
		if not spec then
			-- Missing spec.
			slots:set_object{slot = slot}
			if o.equipment and o.equipment[slot] then
				o.equipment[slot] = nil
				o:update_model()
			end
		elseif spec.equipment_models then
			-- Replacer equipment.
			slots:set_object{slot = slot}
			o.equipment = o.equipment or {}
			o.equipment[slot] = spec.name
			o:update_model()
		else
			-- Add-on equipment.
			slots:set_object{slot = slot, model = spec.model, spec = spec}
			o.equipment = o.equipment or {}
			o.equipment[slot] = spec.name
			o:update_model()
		end
		-- Equip animations.
		local a
		if spec then
			a = o:animate_spec(spec.animation_hold)
		end
		if o.equipment_animations then
			if not a and o.equipment_animations[slot] then
				o:animate{channel = o.equipment_animations[slot].channel}
			end
			o.equipment_animations[slot] = a
		else
			o.equipment_animations = {[slot] = a}
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
		EffectObject{
			model = effect.model,
			position = Vector(x,y,z),
			sound = effect.sound,
			sound_delay = effect.sound_delay,
			sound_pitch = effect.sound_pitch,
			sound_positional = effect.sound_positional,
			realized = true}
		-- Quake the camera.
		Player:apply_quake(Vector(x,y,z), effect.quake)
	end
end}
