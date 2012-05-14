Protocol:add_handler{type = "OBJECT_ANIMATED", func = function(event)
	local ok,id,anim,time = event.packet:read("uint32", "string", "float")
	if not ok then return end
	local o = Object:find{id = id}
	if not o then return end
	o:set_anim(anim, time)
end}

Protocol:add_handler{type = "OBJECT_BEHEADED", func = function(event)
	local ok,i,a,t = event.packet:read("uint32")
	if not ok then return end
	local o = Object:find{id = i}
	if not o then return end
	-- Remove the head.
	o.flags = Bitwise:bor(o.flags, Protocol.object_flags.BEHEADED)
	o:request_model_rebuild()
	-- Play a particle effect.
	Effect:play_object("behead1", o, "#neck")
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
	if obj == Client.player_object then
		Client:set_player_dead(b)
	end
end}

Protocol:add_handler{type = "OBJECT_DIALOG_CHOICE", func = function(args)
	-- Parse the packet.
	local ok,id,init,n = args.packet:read("uint32", "bool", "uint8")
	if not ok then return end
	local choices = {}
	for i = 1,n do
		local ok,m = args.packet:resume("string")
		if not ok then return end
		table.insert(choices, m)
	end
	-- Find the object.
	local obj = Object:find{id = id}
	if not obj then return end
	-- Update the dialog.
	obj:set_dialog("choice", choices)
	if init and (Ui.state == "play" or Ui.state == "world/object") then
		Client.active_dialog_object = obj
		Ui.state = "dialog"
	end
end}

Protocol:add_handler{type = "OBJECT_DIALOG_MESSAGE", func = function(args)
	-- Parse the packet.
	local ok,id,init,c,m = args.packet:read("uint32", "bool", "string", "string")
	if not ok then return end
	-- Find the object.
	local obj = Object:find{id = id}
	if not obj then return end
	-- Update the dialog.
	obj:set_dialog("message", {character = c, message = m})
	if init and (Ui.state == "play" or Ui.state == "world/object") then
		Client.active_dialog_object = obj
		Ui.state = "dialog"
	end
end}

Protocol:add_handler{type = "OBJECT_DIALOG_NONE", func = function(args)
	-- Parse the packet.
	local ok,i = args.packet:read("uint32")
	if not ok then return end
	-- Find the object.
	local obj = Object:find{id = i}
	if not obj then return end
	-- Update the dialog.
	obj:set_dialog("none")
	if obj == Client.active_dialog_object and Ui.state == "dialog" then
		Ui:pop_state()
	end
end}

Protocol:add_handler{type = "OBJECT_FEAT", func = function(event)
	local ok,i,a,move = event.packet:read("uint32", "string", "uint8")
	if not ok then return end
	-- Find the object.
	local obj = Object:find{id = i}
	if not obj then return end
	if not obj.spec then return end
	if not obj.spec.animations then return end
	-- Find the feat type.
	local anim = Feattypespec:find{name = a}
	if not anim then return end
	-- Determine the character animation.
	-- The animation of the feat may be overridden by a weapon specific
	-- animation if there's a weapon in the slot used by the feat.
	local weapon = anim.slot and obj.inventory:get_object_by_slot(anim.slot)
	local animation = anim.animation
	if weapon and weapon.spec.animation_attack then
		animation = weapon.spec.animation_attack
	end
	-- Add a speedline effect for the weapon.
	if weapon and weapon.spec.effect_attack_speedline then
		weapon:add_speedline{delay = 0.3, duration = 0.8}
	end
	-- Melee feats may further override the animation since controls affect
	-- what move the player performs. This is indicated with the move variable.
	if move ~= 0 then
		local map = {
			"attack stand",
			"attack left",
			"attack right",
			"attack back",
			"attack front"}
		animation = map[move] or animation
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
		-- Play the effect.
		Effect:play_object(e, obj)
	end
end}

Protocol:add_handler{type = "OBJECT_HIDDEN", func = function(event)
	local ok,i = event.packet:read("uint32")
	if not ok then return end
	-- Find the object.
	local obj = Object:find{id = i}
	if not obj then return end
	-- Hide the object.
	if obj.static then return end
	obj:detach(true)
	-- Close the dialog when the object is hidden.
	if obj == Client.active_dialog_object and Ui.state == "dialog" then
		Ui:pop_state()
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
		if o.type == "item" or o.type == "actor" then p = p + Object.physics_position_correction end
		-- Set the target interpolation position.
		o:set_motion_state(p, Quaternion(rx, ry, rz, rw), Vector(vx, vy, vz), tilt)
		-- Adjust time scaling of movement animations.
		if o.spec and o.spec.speed_walk then
			local a = o:get_animation{channel = 1}
			if a then
				local ref_speed
				local ref_scale
				local map = {
					["run"] = o.spec.speed_run,
					["run left"] = o.spec.speed_run,
					["run right"] = o.spec.speed_run,
					["strafe left"] = o.spec.speed_run,
					["strafe right"] = o.spec.speed_run,
					["walk"] = o.spec.speed_walk,
					["walk back"] = o.spec.speed_run}
				for k,v in pairs(o.spec.animations) do
					if v.animation == a.animation and map[k] then
						ref_speed = map[k]
						ref_scale = v.time_scale or 1
						break
					end
				end
				if ref_speed then
					local speed = Vector(vx, vt, vz).length
					local scale = speed / math.max(0.1, ref_speed)
					a.time_scale = ref_scale * scale
					o:animate(a)
				end
			end
		end
	end
end}

Protocol:add_handler{type = "OBJECT_SHOWN", func = function(event)
	local debug = function() end
	--local debug = function(...) print(string.format(...)) end
	-- Creation flags.
	local ok,id,flags = event.packet:read("uint32", "uint32")
	if not ok then return end
	debug("SHOWOBJ %d %d", id, flags)
	-- Hide old objects.
	local old = Object:find{id = id}
	if old then old:detach() end
	-- Spec.
	local spec,model,type
	if Bitwise:band(flags, Protocol.object_show_flags.SPEC) ~= 0 then
		local ok,type_,name = event.packet:resume("string", "string")
		if not ok then return end
		type = type_
		if type == "item" then spec = Itemspec:find{name = name}
		elseif type == "obstacle" then spec = Obstaclespec:find{name = name}
		elseif type == "actor" then spec = Actorspec:find{name = name}
		elseif type == "spell" then spec = Spellspec:find{name = name} end
		debug("  SPEC %s %s", type, name)
		if not spec then return end
		model = spec.model
	else
		local ok,m = event.packet:resume("string")
		if not ok then return end
		model = m
		type = "object"
	end
	-- Create the object.
	local o
	local o_args = {collision_group = Physics.GROUP_OBJECT, flags = Bitwise:band(flags, 0xFF),
		id = id, model = model, spec = spec, type = type}
	if type == "actor" then
		o = Actor(o_args)
	elseif type == "item" then
		o = Item(o_args)
	elseif type == "spell" then
		o = Spell(o_args)
	else
		o = Obstacle(o_args)
	end
	-- Apply the preset.
	if spec and spec.preset then
		local preset = Actorpresetspec:find{name = spec.preset}
		if preset then
			local copy_table = function(t)
				if not t then return end
				local u = {}
				for k,v in pairs(t) do u[k] = v end
				return u
			end
			local copy_color = function(t)
				local u = copy_table(t)
				if not u then return end
				return Color:hsv_to_rgb(u)
			end
			o.body_style = copy_table(preset.body)
			o.eye_color = copy_color(preset.eye_color)
			o.eye_style = preset.eye_style
			o.face_style = copy_table(preset.face)
			o.hair_color = copy_color(preset.hair_color)
			o.hair_style = preset.hair_style
			o.head_style = preset.head_style
			o.body_scale = preset.height
			o.skin_color = copy_color(preset.skin_color)
			o.skin_style = preset.skin_style
		end
	end
	-- Self.
	if Bitwise:band(flags, Protocol.object_show_flags.SELF) ~= 0 then
		debug("  SELF")
		o.collision_group = Physics.GROUP_PLAYER
		Client.player_object = o
		Client.mouse_smoothing = Client.options.mouse_smoothing
	end
	-- Actorspec.
	if Bitwise:band(flags, Protocol.object_show_flags.ACTOR) ~= 0 then
		debug("  ACTOR")
		local ok,dead,tilt = event.packet:resume("bool", "float")
		if not ok then return end
		o.dead = dead
		o.tilt = tilt
		if o == Client.player_object then
			Client:set_player_dead(dead)
		end
	end
	-- Position.
	if Bitwise:band(flags, Protocol.object_show_flags.POSITION) ~= 0 then
		debug("  POSITION")
		local ok,x,y,z = event.packet:resume("float", "float", "float")
		if not ok then return end
		debug("    %f %f %f", x, y, z)
		local p = Vector(x,y,z)
		if type == "item" or type == "actor" then p = p + Object.physics_position_correction end
		o.position = p
	end
	-- Rotation.
	if Bitwise:band(flags, Protocol.object_show_flags.ROTATION) ~= 0 then
		debug("  ROTATION")
		local ok,x,y,z,w = event.packet:resume("float", "float", "float", "float")
		if not ok then return end
		debug("    %f %f %f %f", x, y, z, w)
		o.rotation = Quaternion(x,y,z,w)
	end
	-- Name.
	if Bitwise:band(flags, Protocol.object_show_flags.NAME) ~= 0 then
		debug("  NAME")
		local ok,name = event.packet:resume("string")
		if not ok then return end
		o.name = name
		debug("    %q", name)
	end
	-- Animations.
	if Bitwise:band(flags, Protocol.object_show_flags.ANIMS) ~= 0 then
		debug("  ANIMATIONS")
		local ok,num = event.packet:resume("uint8")
		if not ok then return end
		debug("    %d", num)
		for i=1,num do
			local ok,anim,time = event.packet:resume("string", "float")
			if not ok then return end
			debug("    %s %f", anim, time)
			o:set_anim(anim, time)
		end
	end
	-- Equipment.
	if Bitwise:band(flags, Protocol.object_show_flags.SLOTS) ~= 0 then
		debug("  SLOTS")
		local ok,num = event.packet:resume("uint8")
		if not ok then return end
		debug("    %d", num)
		for i=1,num do
			local ok,index,slot,name,count = event.packet:resume("uint32", "string", "string", "uint32")
			if not ok then return end
			debug("    %d %s %s %d", index, slot, name, count)
			local spec = Itemspec:find{name = name}
			if spec then
				o.inventory:set_object(index, Item{spec = spec, count = count})
				o.inventory:equip_index(index, slot)
			end
		end
	end
	-- Skills.
	if Bitwise:band(flags, Protocol.object_show_flags.SKILLS) ~= 0 then
		debug("  SKILLS")
		local ok,num = event.packet:resume("uint8")
		if not ok then return end
		debug("    %d", num)
		for i=1,num do
			local ok,skill,value,max = event.packet:resume("string", "int32", "int32")
			if not ok then return end
			debug("    %s %d %d", skill, value, max)
			o:set_stat(skill, value, max)
		end
	end
	-- Body style.
	if Bitwise:band(flags, Protocol.object_show_flags.BODY_STYLE) ~= 0 then
		debug("  BODY STYLE")
		local ok,a,b,c,d,e,f,g,h,i,j
		-- Scale.
		ok,a = event.packet:resume("uint8")
		if not ok then return end
		o.body_scale = a / 255
		debug("    SCALE %d", a)
		-- Style.
		ok,a,b,c,d,e,f,g,h,i = event.packet:resume("uint8", "uint8", "uint8", "uint8", "uint8",
			"uint8", "uint8", "uint8", "uint8", "uint8")
		if not ok then return end
		o.body_style = {a / 255, b / 255, c / 255, d / 255, e / 255,
			f / 255, g / 255, h / 255, i / 255, i / 255}
		debug("    STYLE %d %d %d %d %d %d %d %d %d", a, b, c, d, e, f, g, h, i, j)
		-- Skin.
		ok,a,b,c,d = event.packet:resume("string", "uint8", "uint8", "uint8")
		if not ok then return end
		o.skin_style = a
		o.skin_color = {b / 255, c / 255, d / 255}
		debug("    SKIN %s %d %d %d", a, b, c, d)
	end
	-- Head style.
	if Bitwise:band(flags, Protocol.object_show_flags.HEAD_STYLE) ~= 0 then
		debug("  HEAD STYLE")
		local ok,head,a,b,c,d,e,f,g,h,i,j,k,l,m,n,p
		-- Eyes.
		ok,head,a,b,c,d = event.packet:resume("string", "string", "uint8", "uint8", "uint8")
		if not ok then return end
		o.head_style = head
		debug("    HEAD %s", head)
		o.eye_style = a
		o.eye_color = {b / 255, c / 255, d / 255}
		debug("    EYE %s %d %d %d", a, b, c, d)
		-- Face.
		ok,a,b,c,d,e,f,g,h,i,j,k,l,m,n,p = event.packet:resume("uint8", "uint8", "uint8", "uint8", "uint8",
			"uint8", "uint8", "uint8", "uint8", "uint8", "uint8", "uint8", "uint8", "uint8", "uint8")
		if not ok then return end
		o.face_style = {a / 255, b / 255, c / 255, d / 255, e / 255, f / 255, g / 255,
			h / 255, i / 255, j / 255, k / 255, l / 255, m / 255, n / 255, p / 255}
		debug("    FACE %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d", a, b, c, d, e, f, g, h, i, j, k, l, m, n, p)
		-- Hair.
		ok,a,b,c,d = event.packet:resume("string", "uint8", "uint8", "uint8")
		if not ok then return end
		o.hair_style = a
		o.hair_color = {b / 255, c / 255, d / 255}
		debug("    HAIR %s %d %d %d", a, b, c, d)
	end
	-- Dialog.
	if Bitwise:band(flags, Protocol.object_show_flags.DIALOG) ~= 0 then
		debug("  DIALOG")
		local ok,a
		ok,a = event.packet:resume("uint8")
		if not ok then return end
		if a == 0 then
			-- Choices.
			local choices = {}
			ok,a = event.packet:resume("uint8")
			debug("    CHOICES %d", a)
			if not ok then return end
			for i = 1,a do
				ok,a = event.packet:resume("string")
				if not ok then return end
				debug("      %s", a)
				table.insert(choices, a)
			end
			o:set_dialog("choice", choices)
		elseif a == 1 then
			-- Line.
			debug("    MESSAGE", a)
			ok,a = event.packet:resume("string")
			if not ok then return end
			o:set_dialog("message", a)
			debug("      %s", a)
		else
			-- None.
			debug("    NONE", a)
			o:set_dialog("none")
		end
	end
	-- Count.
	if Bitwise:band(flags, Protocol.object_show_flags.COUNT) ~= 0 then
		debug("  COUNT")
		local ok,a
		ok,a = event.packet:resume("uint32")
		if not ok then return end
		o.count = a
	end
	-- Set the spec model.
	debug("  OK")
	o.flags = Bitwise:band(flags, 0xFF)
	o:set_model()
	o:update_rotation(o.rotation, o.tilt)
	o.realized = true
	-- Initialize speed lines.
	if Bitwise:band(flags, Protocol.object_flags.SPEEDLINE) ~= 0 then
		o:add_speedline()
	end
end}

Protocol:add_handler{type = "OBJECT_SKILL", func = function(event)
	local ok,id,skill,value,max = event.packet:read("uint32", "string", "int32", "int32")
	if not ok then return end
	local o = Object:find{id = id}
	if not o then return end
	o:set_stat(skill, value, max)
end}

Protocol:add_handler{type = "OBJECT_SPEECH", func = function(event)
	local ok,i,m = event.packet:read("uint32", "string")
	if not ok then return end
	local o = Object:find{id = i}
	if not o then return end
	local n = o.name or o.spec.name
	Client:add_speech_text{object = o, name = o.name or o.spec.name, text = m}
end}
