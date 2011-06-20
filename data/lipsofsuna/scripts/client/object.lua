require "client/speedline"

Object.physics_position_correction = Vector(0, 0, 0)

local oldanimate = Object.animate
Object.animate = function(self, args)
	if not self.animated and self.model then
		local m = self.model:copy()
		m:calculate_bounds()
		m:changed()
		self.model = m
		self.animated = true
	end
	oldanimate(self, args)
end

Object.animate_spec = function(self, name)
	-- Find the animation from the spec.
	if not self.spec then return end
	if not self.spec.animations then return end
	local a = self.spec.animations[name]
	if not a then return end
	-- Play the animation.
	self:animate{
		animation = a.animation,
		channel = a.channel,
		fade_in = a.fade_in or 0.5,
		fade_out = a.fade_out or 0.5,
		permanent = a.permanent,
		repeat_start = a.repeat_start,
		weight = a.weight or 2}
	return a
end

Object.detach = function(self)
	-- Hide special effects.
	if self.special_effects then
		for k,v in pairs(self.special_effects) do
			v.realized = false
		end
	end
	-- Hide speed lines.
	if self.speedline then
		self.speedline.realized = false
	end
	-- Hide equipment.
	if self.slots then
		for k,v in pairs(self.slots.slots) do
			v:detach()
		end
	end
	-- Hide self.
	self.realized = false
end

--- Creates a customized character model for the object.<br/>
-- This function is used by both the network events and the character creation
-- screen to create customized character objects.
-- @param self Object.
-- @param args Arguments.<ul>
--   <li>body_scale: Scale factor.</li>
--   <li>body_style: Body style array.</li>
--   <li>equipment: List of equipment.</li>
--   <li>face_style: Array of face morphing weights.</li>
--   <li>hair_color: Hair color array.</li>
--   <li>hair_style: Hair style string.</li>
--   <li>race: Race string.</li></ul>
Object.create_character_model = function(self, args)
	local lod = (Views.Options.inst.model_quality == 0)
	-- Get the species.
	local name = args.race
	local species = Species:find{name = name}
	if not species then return end
	-- Get the base meshes.
	local meshes = {skeleton = species.model}
	if species.models then
		for k,v in pairs(lod and species.models_lod or species.models) do
			meshes[k] = v
		end
	end
	-- Add hair model.
	if args.hair_style and args.hair_style ~= "" then
		meshes.hair = args.hair_style
	end
	-- Add equipment models.
	if args.equipment then
		for slot,name in pairs(args.equipment) do
			local spec = Itemspec:find{name = name}
			if spec and spec.equipment_models then
				local models = spec:get_equipment_models(species.equipment_class or species.name, lod)
				if models then
					for k,v in pairs(models) do
						meshes[k] = v
					end
				end
			end
		end
	end
	-- Create skeleton.
	local m = Model:load{file = meshes.skeleton}
	m = m:copy()
	-- Add other meshes.
	local has_head = not Bitwise:bchk(self.flags or 0, Protocol.object_flags.BEHEADED)
	local mesh_head = {eyes = true, head = true, hair = true}
	for k,v in pairs(meshes) do
		if k ~= "skeleton" and (has_head or not mesh_head[k]) then
			local tmp
			local ref = Model:load{file = v}
			-- Face customization.
			if args.face_style and (string.match(k, ".*head.*") or string.match(k, ".*eye.*")) then
				tmp = ref:copy()
				if args.face_style[1] then
					if args.face_style[1] < 0.5 then
						tmp:morph("cheekbone small", 1 - 2 * args.face_style[1], ref)
					elseif args.face_style[1] > 0.5 then
						tmp:morph("cheekbone big", 2 * args.face_style[1] - 1, ref)
					end
				end
				if args.face_style[2] then tmp:morph("cheek small", 1 - args.face_style[2], ref) end
				if args.face_style[3] then tmp:morph("chin rough", args.face_style[3], ref) end
				if args.face_style[4] then tmp:morph("chin round", args.face_style[4], ref) end
				if args.face_style[5] then tmp:morph("chin small", 1 - args.face_style[5], ref) end
				if args.face_style[6] then tmp:morph("eye inner", args.face_style[6], ref) end
				if args.face_style[7] then tmp:morph("eye outer", args.face_style[7], ref) end
				if args.face_style[8] then tmp:morph("eye small", 1 - args.face_style[8], ref) end
				if args.face_style[9] then tmp:morph("face thin", 1 - args.face_style[9], ref) end
				if args.face_style[10] then tmp:morph("face wrinkle", args.face_style[10], ref) end
				if args.face_style[11] then tmp:morph("jaw wide", args.face_style[11], ref) end
				if args.face_style[12] then tmp:morph("nose dull", 1 - args.face_style[12], ref) end
			end
			-- Body customization.
			if args.body_style then
				if not tmp then tmp = ref:copy() end
				if args.body_style[1] then tmp:morph("hips wide", args.body_style[1], ref) end
				if args.body_style[2] then tmp:morph("limbs muscular", args.body_style[2], ref) end
				if args.body_style[3] then
					if args.body_style[3] < 0.5 then
						tmp:morph("torso small", 1 - 2 * args.body_style[3], ref)
					elseif args.body_style[3] > 0.5 then
						tmp:morph("torso big", 2 * args.body_style[3] - 1, ref)
					end
				end
				if args.body_style[4] then tmp:morph("torso thick", args.body_style[4], ref) end
				if args.body_style[5] then tmp:morph("waist thick", args.body_style[5], ref) end
			end
			-- Merge to the character model.
			m:merge(tmp or ref)
		end
	end
	-- Colorize materials.
	m:edit_material{match_shader = "hair", diffuse = args.hair_color}
	m:edit_material{match_shader = "skin", diffuse = args.skin_color}
	m:edit_material{match_texture = "eye1", diffuse = args.eye_color}
	-- Recalculate bounding box.
	m:calculate_bounds()
	m:changed()
	self.model = m
	-- Apply body scale.
	local factor = args.body_scale or 0.5
	local scale = species.body_scale[1] * (1 - factor) + species.body_scale[2] * factor
	self.animated = true
	self:animate{animation = "empty", channel = Animation.CHANNEL_CUSTOMIZE,
		weight = 0, weight_scale = 1, permanent = true}
	self:edit_pose{channel = Animation.CHANNEL_CUSTOMIZE, node = "mover", scale = scale}
	-- Apply initial deformation.
	self:update_animations{secs = 0}
	self:deform_mesh()
end

Object.set_anim = function(self, name, time)
	local args = {animation = name, fade_in = 0.3, fade_out = 0.3, time = t}
	local anim = self.spec and self.spec.animations[name]
	if anim then
		for k,v in pairs(anim) do args[k] = v end
	end
	self:animate(args)
end

Object.set_dialog = function(self, type, args)
	-- Update the dialog state.
	if type == "choice" then
		self.dialog = {type = type, choices = args}
	elseif type == "message" then
		self.dialog = {type = type, message = args}
	else
		self.dialog = nil
	end
	-- Update the dialog UI.
	if Gui.active_dialog == self.id then
		Gui:set_dialog(self.id)
	end
end

Object.set_skill = function(self, s, v, m)
	-- Update player skills.
	if self == Player.object then
		Gui.skills:update(s, v, m)
	end
	-- Display health changes.
	if s == "health" then
		if self.health then
			-- Show a health change text.
			local diff = v - self.health
			if math.abs(diff) > 2 then
				local code = (diff > 0 and 0x01 or 0x00) + (self == Player.object and 0x10 or 0x00)
				local colors = {
					[0x00] = {1,1,0,1},
					[0x01] = {0,1,1,1},
					[0x10] = {1,0,0,1},
					[0x11] = {0,1,0,1}}
				EffectObject{
					life = 3,
					object = self,
					position = Vector(0,2,0),
					realized = true,
					text = tostring(diff),
					text_color = colors[code],
					text_fade_time = 1,
					text_font = "medium",
					velocity = Vector(0,0.5,0)}
			end
			-- Quake the camera if the player was hurt.
			if self == Player.object and diff < -5 then
				Player:apply_quake(self.position, 0.01 * (5 - diff))
			end
		end
		self.health = v
		-- Set the correct collision shape.
		-- Dead creatures have a different collision shape. We switch between
		-- the two when the health changes between zero and non-zero.
		if self.health == 0 and self.animated then
			self.shape = "dead"
		else
			self.shape = "default"
		end
	end
end

Object.set_slot = function(self, slot, spec, count)
	if not self.slots then self.slots = Slots() end
	local slots = self.slots
	spec = Itemspec:find{name = spec}
	-- Update the model.
	if not spec then
		-- Missing spec.
		slots:set_object{slot = slot}
		if self.equipment and self.equipment[slot] then
			self.equipment[slot] = nil
			if self.realized then self:update_model() end
		end
	elseif spec.equipment_models then
		-- Replacer equipment.
		slots:set_object{slot = slot}
		self.equipment = self.equipment or {}
		self.equipment[slot] = spec.name
		if self.realized then self:update_model() end
	else
		-- Add-on equipment.
		slots:set_object{slot = slot, model = spec.model, spec = spec}
		self.equipment = self.equipment or {}
		self.equipment[slot] = spec.name
		if self.realized then self:update_model() end
	end
	-- Equip animations.
	local a
	if spec then
		a = self:animate_spec(spec.animation_hold)
	end
	if self.equipment_animations then
		if not a and self.equipment_animations[slot] then
			self:animate{channel = self.equipment_animations[slot].channel}
		end
		self.equipment_animations[slot] = a
	else
		self.equipment_animations = {[slot] = a}
	end
end

Object.update = function(self, secs)
	-- Update slots.
	if self.slots then
		local species = self.spec
		for name,object in pairs(self.slots.slots) do
			local slot = species and species.equipment_slots[name]
			if slot and slot.node and self.realized then
				-- Show slot.
				local p,r = self:find_node{name = slot.node, space = "world"}
				if p then
					local h = object:find_node{name = "#handle"}
					if h then p = p - r * h end
					object.position = p
					object.rotation = r
					object.realized = true
					object:update(secs)
				else
					object:detach()
				end
			else
				-- Hide slot.
				object:detach()
			end
		end
	end
	-- Update special effects.
	if self.realized then
		if self.special_effects then
			local p = self.position
			for k,v in pairs(self.special_effects) do
				local n = self:find_node{name = v.slot}
				if n then p = p + self.rotation * n end
				v.position = p
			end
		end
	end
	-- Update speed lines.
	if self.speedline then
		self.speedline:update(secs)
	end
end

Object.update_model = function(self)
	if not Model.morph then return end
	-- Instantiate the model for creatures.
	if self.type == "creature" and self.model and self.model.name then
		self.model = self.model:copy()
	end
	-- Character customizations.
	if self.spec and self.spec.models then
		self:create_character_model{
			body_scale = self.body_scale,
			body_style = self.body_style,
			equipment = self.equipment,
			eye_color = self.eye_color,
			eye_style = self.eye_style,
			face_style = self.face_style,
			hair_color = self.hair_color,
			hair_style = self.hair_style,
			race = self.spec.name,
			skin_color = self.skin_color,
			skin_style = self.skin_style}
	end
	-- Create the customization animation.
	if self.spec and (self.spec.models or self.spec.tilt_bone) then
		self:animate{animation = "empty", channel = Animation.CHANNEL_TILT,
			weight = 1000, permanent = true}
	end
	-- Create special effects.
	if self.spec and self.spec.special_effects and #self.spec.special_effects then
		self.special_effects = {}
		for k,v in pairs(self.spec.special_effects) do
			local fx = Object{model = v.model, position = self.position, realized = true, slot = "#" .. v.slot}
			table.insert(self.special_effects, fx)
		end
	else
		self.special_effects = nil
	end
end

--- Updates the rotation and tilt of the object.
-- @param self Object.
-- @param quat Rotation quaternion.
-- @param tilt Tilt angle in radians.
Object.update_rotation = function(self, quat, tilt)
	self.rotation = quat
	self.tilt = tilt
	local spec = self.spec
	if not self.dead and spec and spec.tilt_bone then
		self:edit_pose{channel = Animation.CHANNEL_TILT, node = spec.tilt_bone, rotation = Quaternion{axis = Vector(1,0,0), angle = -tilt}}
	end
end

Object.update_motion_state = function(self, tick)
	if not self.interpolation then return end
	-- Damp velocity to reduce overshoots.
	self.interpolation = self.interpolation + tick
	if self.interpolation > 0.3 then
		self.velocity = self.velocity * 0.93
	end
	-- Apply position change predicted by the velocity.
	self.position = self.position + self.velocity * tick
	-- Correction prediction errors over time.
	self.position = self.position + self.correction * 0.07
	self.correction = self.correction * 0.93
end

Object.set_motion_state = function(self, pos, rot, vel, tilt)
	-- Store the prediction error so that it can be corrected over time.
	if (pos - self.position).length < 5 then
		self.correction = pos - self.position
	else
		self.position = pos
		self.correction = Vector()
	end
	-- Store the current velocity so that we can predict future movements.
	self.velocity = Vector(vx, vy, vz)
	self.interpolation = 0
	-- Set rotation unless controlled by the local player.
	if self ~= Player.object then
		self:update_rotation(rot, tilt)
	end
end
