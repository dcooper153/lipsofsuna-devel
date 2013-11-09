-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.

local Bitwise = require("system/bitwise")
local Item = require("core/objects/item")
local Physics = require("system/physics")
local PhysicsConsts = require("core/server/physics-consts")
local Simulation = require("core/client/simulation")

local make_flags = function(list)
	local res = {}
	for k,v in ipairs(list) do
		res[v] = 2^(k-1)
	end
	return res
end
local FlagType = make_flags{
	"SELF", "SPEC", "ACTOR", "NAME", "COUNT", "POSITION", "ROTATION", "HEAD", "SKIN",
	"EYE", "HAIR", "BODY", "FACE", "ANIMS", "EQUIPMENT", "STATS", "DIALOG", "SPEEDLINE",
	"PROFILE", "COMBAT"}

-- The number of fields is so high that an exception is made for message
-- type: the server passes the object itself and the client accepts either
-- the passed object or a table that contains the decoded object data.
Main.messaging:register_message{
	name = "object shown",
	server_to_client_encode = function(self, mine, o)
		local add = function(data, type, value)
			table.insert(data, type)
			table.insert(data, value)
		end
		-- Header.
		local flags = 0
		local data = {"uint32", o:get_id(), "uint32", flags}
		-- Spec.
		if o.spec.type ~= "object" then
			flags = flags + FlagType.SPEC
			add(data, "string", o.spec.type)
			add(data, "string", o.spec.name)
		else
			add(data, "string", o:get_model_name())
		end
		-- Self.
		if mine then
			flags = flags + FlagType.SELF
		end
		-- Actor.
		if o.spec.type == "actor" then
			flags = flags + FlagType.ACTOR
			add(data, "bool", o.dead or false)
			add(data, "float", o:get_tilt_angle())
		end
		-- Animation profile.
		if o.animation_profile then
			flags = flags + FlagType.PROFILE
			add(data, "string", o.animation_profile)
		end
		-- Name.
		if o.name then
			add(data, "string", o.name)
			flags = flags + FlagType.NAME
		end
		-- Count.
		if o:get_count() > 1 then
			flags = flags + FlagType.COUNT
			add(data, "uint32", o:get_count())
		end
		-- Position.
		local pos = o:get_position()
		if pos.x > 1 or pos.y > 1 or pos.z > 1 then
			flags = flags + FlagType.POSITION
			add(data, "float", pos.x)
			add(data, "float", pos.y)
			add(data, "float", pos.z)
		end
		-- Rotation.
		local rot = o:get_rotation()
		if math.abs(rot.w) < 0.99 then
			flags = flags + FlagType.ROTATION
			add(data, "float", rot.x)
			add(data, "float", rot.y)
			add(data, "float", rot.z)
			add(data, "float", rot.w)
		end
		-- Head style.
		if o.head_style then
			flags = flags + FlagType.HEAD
			add(data, "string", o.head_style or "")
		end
		-- Skin style.
		if o.skin_style then
			flags = flags + FlagType.SKIN
			add(data, "string", o.skin_style or "")
			add(data, "uint8", o.skin_color and o.skin_color[1] or 255)
			add(data, "uint8", o.skin_color and o.skin_color[2] or 255)
			add(data, "uint8", o.skin_color and o.skin_color[3] or 255)
		end
		-- Eye style.
		if o.eye_style or o.eye_color then
			flags = flags + FlagType.EYE
			add(data, "string", o.eye_style or "")
			add(data, "uint8", o.eye_color and o.eye_color[1] or 255)
			add(data, "uint8", o.eye_color and o.eye_color[2] or 255)
			add(data, "uint8", o.eye_color and o.eye_color[3] or 255)
		end
		-- Hair style.
		if o.hair_style or o.hair_color then
			flags = flags + FlagType.HAIR
			add(data, "string", o.hair_style or "")
			add(data, "uint8", o.hair_color and o.hair_color[1] or 255)
			add(data, "uint8", o.hair_color and o.hair_color[2] or 255)
			add(data, "uint8", o.hair_color and o.hair_color[3] or 255)
		end
		-- Body style.
		if o.body_scale or o.body_sliders then
			flags = flags + FlagType.BODY
			add(data, "uint8", o.body_scale or 128)
			add(data, "uint16", o.body_sliders and #o.body_sliders or 0)
			for k,v in ipairs(o.body_sliders or {}) do
				add(data, "uint8", o.body_sliders[k])
			end
		end
		-- Face style.
		if o.brow_style or o.face_style or o.mouth_style or o.face_sliders then
			flags = flags + FlagType.FACE
			add(date, "string", o.brow_style or "")
			add(date, "string", o.face_style or "")
			add(date, "string", o.mouth_style or "")
			add(data, "uint16", #o.face_sliders)
			for k,v in ipairs(o.face_sliders) do
				add(data, "uint8", o.face_sliders[k])
			end
		end
		-- Animations.
		if o.animations then
			local tmp = {}
			for k,v in pairs(o.animations) do
				if v[1] then
					table.insert(tmp, {v[1], Program:get_time() - v[2]})
				end
			end
			if #tmp > 0 then
				add(data, "uint8", #tmp)
				for k,v in ipairs(tmp) do
					add(data, "string", v[1])
					add(data, "float", v[2])
				end
				flags = flags + FlagType.ANIMS
			end
		end
		-- Equipment.
		if o.inventory then
			local tmp = {}
			for slot,index in pairs(o.inventory.equipped) do
				local item = o.inventory:get_object_by_index(index)
				if item then
					table.insert(tmp, {index, slot, item.spec.name, item:get_count()})
				end
			end
			if #tmp > 0 then
				add(data, "uint8", #tmp)
				for k,v in ipairs(tmp) do
					add(data, "uint32", v[1])
					add(data, "string", v[2])
					add(data, "string", v[3])
					add(data, "uint32", v[4])
				end
				flags = flags + FlagType.EQUIPMENT
			end
		end
		-- Stats.
		if o.stats then
			local tmp = {}
			for name,stat in pairs(o.stats.stats) do
				if stat.prot == "public" or self == o then
					table.insert(tmp, {name, math.ceil(stat.value), math.ceil(stat.maximum)})
				end
			end
			if #tmp > 0 then
				add(data, "uint8", #tmp)
				for k,v in ipairs(tmp) do
					add(data, "string", v[1])
					add(data, "int32", v[2])
					add(data, "int32", v[3])
				end
				flags = flags + FlagType.STATS
			end
		end
		-- Dialog.
		if o.dialog and o.dialog.event then
			local e = o.dialog.event
			if e.choices then
				add(data, "uint8", 0)
				add(data, "uint8", #e.choices)
				for k,v in ipairs(e.choices) do
					add(data, "string", v)
				end
			else
				add(data, "uint8", 1)
				add(data, "string", e.character or "")
				add(data, "string", e.message)
			end
			flags = flags + FlagType.DIALOG
		end
		-- Speed line.
		if o.speedline then
			flags = flags + FlagType.SPEEDLINE
		end
		-- Combat hint.
		if o.get_combat_hint and o:get_combat_hint() then
			flags = flags + FlagType.COMBAT
		end
		-- Rewrite the flags.
		data[4] = flags
		return data
	end,
	server_to_client_decode = function(self, packet)
		local args,ok = {}
		-- Header.
		ok,args.id,args.flags = packet:read("uint32", "uint32")
		if not ok then return end
		-- Spec.
		if Bitwise:band(args.flags, FlagType.SPEC) ~= 0 then
			ok,args.type,args.spec = packet:resume("string", "string")
			if not ok then return end
		else
			ok,args.model = packet:resume("string")
			if not ok then return end
			args.type = "object"
		end
		-- Self.
		if Bitwise:band(args.flags, FlagType.SELF) ~= 0 then
			args.self = true
		end
		-- Actor.
		if Bitwise:band(args.flags, FlagType.ACTOR) ~= 0 then
			ok,args.dead,args.tilt = packet:resume("bool", "float")
			if not ok then return end
		end
		-- Animation profile.
		if Bitwise:band(args.flags, FlagType.PROFILE) ~= 0 then
			ok,args.animation_profile = packet:resume("string")
		end
		-- Name.
		if Bitwise:band(args.flags, FlagType.NAME) ~= 0 then
			ok,args.name = packet:resume("string")
			if not ok then return end
		end
		-- Count.
		if Bitwise:band(args.flags, FlagType.COUNT) ~= 0 then
			ok,args.count = packet:resume("uint32")
			if not ok then return end
		end
		-- Position.
		if Bitwise:band(args.flags, FlagType.POSITION) ~= 0 then
			local ok,x,y,z = packet:resume("float", "float", "float")
			if not ok then return end
			args.position = Vector(x,y,z)
		end
		-- Rotation.
		if Bitwise:band(args.flags, FlagType.ROTATION) ~= 0 then
			local ok,x,y,z,w = packet:resume("float", "float", "float", "float")
			if not ok then return end
			args.rotation = Quaternion(x,y,z,w)
		end
		-- Head style.
		if Bitwise:band(args.flags, FlagType.HEAD) ~= 0 then
			ok,args.head_style = packet:resume("string")
			if not ok then return end
		end
		-- Skin style.
		if Bitwise:band(args.flags, FlagType.SKIN) ~= 0 then
			local ok,a,b,c,d = packet:resume("string", "uint8", "uint8", "uint8")
			if not ok then return end
			args.skin_style = a
			args.skin_color = {b, c, d}
		end
		-- Eye style.
		if Bitwise:band(args.flags, FlagType.EYE) ~= 0 then
			local ok,a,b,c,d = packet:resume("string", "uint8", "uint8", "uint8")
			if not ok then return end
			args.eye_style = a
			args.eye_color = {b, c, d}
		end
		-- Hair style.
		if Bitwise:band(args.flags, FlagType.HAIR) ~= 0 then
			local ok,a,b,c,d = packet:resume("string", "uint8", "uint8", "uint8")
			if not ok then return end
			args.hair_style = a
			args.hair_color = {b, c, d}
		end
		-- Body style.
		if Bitwise:band(args.flags, FlagType.BODY) ~= 0 then
			local ok,scale,count = packet:resume("uint8", "uint16")
			if not ok then return end
			args.body_scale = scale
			args.body_sliders = {}
			for i = 1,count do
				local ok,val = packet:resume("uint8")
				if not ok then return end
				args.body_sliders[i] = val
			end
		end
		-- Face style.
		if Bitwise:band(args.flags, FlagType.FACE) ~= 0 then
			local ok,brow,face,mouth,count = packet:resume("string", "string", "string", "uint16")
			if not ok then return end
			args.brow_style = (brow ~= "" and brow or nil)
			args.face_style = (face ~= "" and face or nil)
			args.mouth_style = (mouth ~= "" and mouth or nil)
			args.face_sliders = {}
			for i = 1,count do
				local ok,val = packet:resume("uint8")
				if not ok then return end
				args.face_sliders[i] = val
			end
		end
		-- Animations.
		if Bitwise:band(args.flags, FlagType.ANIMS) ~= 0 then
			args.animations = {}
			local ok,num = packet:resume("uint8")
			if not ok then return end
			for i=1,num do
				local ok,anim,time = packet:resume("string", "float")
				if not ok then return end
				table.insert(args.animations, {anim, time})
			end
		end
		-- Equipment.
		if Bitwise:band(args.flags, FlagType.EQUIPMENT) ~= 0 then
			args.equipment = {}
			local ok,num = packet:resume("uint8")
			if not ok then return end
			for i=1,num do
				local ok,index,slot,name,count = packet:resume("uint32", "string", "string", "uint32")
				if not ok then return end
				table.insert(args.equipment, {index, slot, name, count})
			end
		end
		-- Stats.
		if Bitwise:band(args.flags, FlagType.STATS) ~= 0 then
			args.stats = {}
			local ok,num = packet:resume("uint8")
			if not ok then return end
			for i=1,num do
				local ok,name,value,max = packet:resume("string", "int32", "int32")
				if not ok then return end
				table.insert(args.stats, {name, value, max})
			end
		end
		-- Dialog.
		if Bitwise:band(args.flags, FlagType.DIALOG) ~= 0 then
			local ok,type = packet:resume("uint8")
			if not ok then return end
			if type == 0 then
				args.dialog_type = "choice"
				args.dialog_choices = {}
				local ok,num = packet:resume("uint8")
				if not ok then return end
				for i = 1,num do
					local ok,choice = packet:resume("string")
					if not ok then return end
					table.insert(args.dialog_choices, choice)
				end
			elseif type == 1 then
				args.dialog_type = "message"
				ok,args.dialog_message = packet:resume("string")
				if not ok then return end
			else
				args.dialog_type = "none"
			end
		end
		-- Speed lines.
		if Bitwise:band(args.flags, FlagType.SPEEDLINE) ~= 0 then
			args.speedline = true
		end
		-- Combat hint.
		if Bitwise:band(args.flags, FlagType.COMBAT) ~= 0 then
			args.combat_hint = true
		end
		return {args.self, args}
	end,
	server_to_client_handle = function(self, mine, args)
		local update_player_object = function(o)
			o.physics:set_collision_group(PhysicsConsts.GROUP_PLAYERS)
			Client:set_player_object(o)
			Ui:set_state("play")
		end
		-- Only set the render model in single player.
		if args.class then
			-- Find the object.
			local o = Main.objects:find_by_id(args:get_id())
			if not o then return end
			-- Self.
			if o.client == -1 then
				update_player_object(o)
			end
			-- Show the object.
			o.render:init(o)
			-- Animations.
			if o.animations then
				for k,v in pairs(o.animations) do
					if v[1] then
						o.render:add_animation(v[1], Program:get_time() - v[2])
					end
				end
			end
			-- Speed lines.
			if o.speedline then
				o.render:add_speedline()
			end
			-- Combat hint.
			if o.get_combat_hint and o:get_combat_hint() then
				Client.music:set_combat_hint(o, true)
			end
			return
		end
		-- Hide old objects.
		local old = Main.objects:find_by_id(args.id)
		if old then old:detach() end
		-- Spec.
		local spec
		if args.type == "item" then
			spec = Itemspec:find_by_name(args.spec)
		elseif args.type == "obstacle" then
			spec = Obstaclespec:find_by_name(args.spec)
		elseif args.type == "actor" then
			spec = Actorspec:find_by_name(args.spec)
		elseif args.type == "spell" then
			spec = Spellspec:find_by_name(args.spec)
		end
		local o = Simulation:create_object_by_spec(spec, args.id)
		-- Self.
		if args.self then
			update_player_object(o)
		end
		-- Actor.
		if args.tilt then
			o:set_tilt_angle(args.tilt)
			o.dead = args.dead
			if args.self then
				Client:set_player_dead(args.dead)
			end
		end
		-- Animation profile.
		if args.animation_profile then
			o.animation_profile = args.animation_profile
		end
		-- Name.
		if args.name then
			o.name = args.name
		end
		-- Count.
		if args.count then
			o:set_count(args.count)
		end
		-- Position.
		if args.position then
			o:set_position(args.position)
		end
		-- Rotation.
		if args.rotation then
			o:set_rotation(args.rotation)
		end
		-- Head style.
		if args.head_style then
			o.head_style = args.head_style
		end
		-- Skin style.
		if args.skin_style then
			o.skin_style = args.skin_style
			o.skin_color = args.skin_color
		end
		-- Eyes style.
		if args.eye_style then
			o.eye_style = args.eye_style
			o.eye_color = args.eye_color
		end
		-- Hair style.
		if args.hair_style then
			o.hair_style = args.hair_style
			o.hair_color = args.hair_color
		end
		-- Body style.
		if args.body_sliders then
			o.body_scale = args.body_scale
			o.body_sliders = args.body_sliders
		end
		-- Face style.
		if args.face_style then
			o.brow_style = args.brow_style
			o.face_style = args.face_style
			o.mouth_style = args.mouth_style
			o.face_sliders = args.face_sliders
		end
		-- Show the object.
		o:set_visible(true)
		o.render:init(o)
		-- Animations.
		if args.animations then
			for k,v in ipairs(args.animations) do
				o.render:add_animation(v[1], v[2])
			end
		end
		-- Equipment.
		if args.equipment then
			for k,v in ipairs(args.equipment) do
				local spec = Itemspec:find{name = v[3]}
				if spec then
					local item = Item(o.manager)
					item:set_spec(spec)
					item:set_count(v[4])
					o.inventory:set_object(v[1], item)
					o.inventory:equip_index(v[1], v[2])
				end
			end
		end
		-- Stats.
		if args.stats then
			for k,v in ipairs(args.stats) do
				o:set_stat(v[1], v[2], v[3])
			end
		end
		-- Dialog.
		if args.dialog_type == "choice" then
			o:set_dialog("choice", args.dialog_choices)
		elseif args.dialog_type == "message" then
			o:set_dialog("message", args.dialog_message)
		elseif args.dialog_type == "none" then
			o:set_dialog("none")
		end
		-- Speed lines.
		if args.speedline then
			o.render:add_speedline()
		end
		-- Combat hint.
		if args.combat_hint then
			Client.music:set_combat_hint(o, true)
		end
	end}
