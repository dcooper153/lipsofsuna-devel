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
	-- Hide self.
	self.realized = false
end

--- Creates a customized character model for the object.<br/>
-- This function is used by both the network events and the character creation
-- screen to create customized character objects.
-- @param self Object.
-- @param args Arguments.<ul>
--   <li>body_scale: Scale factor.</li>
--   <li>bust_scale: Bust scale factor.</li>
--   <li>equipment: List of equipment.</li>
--   <li>face_style: Array of face morphing weights.</li>
--   <li>hair_color: Hair color string.</li>
--   <li>hair_style: Hair style string.</li>
--   <li>nose_scale: Nose scale factor.</li>
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
	for k,v in pairs(meshes) do
		if k ~= "skeleton" then
			if string.match(k, ".*head.*") then
				local ref = Model:load{file = v}
				local tmp = ref:copy()
				if args.face_style then
					if args.face_style[1] then tmp:morph("face round", args.face_style[1], ref) end
					if args.face_style[2] then tmp:morph("face rough", args.face_style[2], ref) end
					if args.face_style[3] then tmp:morph("face frown", args.face_style[3], ref) end
				end
				m:merge(tmp)
			else
				m:merge(Model:load{file = v})
			end
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
	-- Apply custom deformations.
	self.animated = true
	self:animate{animation = "empty", channel = Animation.CHANNEL_CUSTOMIZE,
		weight = 0, weight_scale = 1, permanent = true}
	self:edit_pose{channel = Animation.CHANNEL_CUSTOMIZE, node = "mover", scale = args.body_scale or 1}
	self:edit_pose{channel = Animation.CHANNEL_CUSTOMIZE, node = "nose", scale = args.nose_scale or 1}
	self:edit_pose{channel = Animation.CHANNEL_CUSTOMIZE, node = "breast.L", scale = args.bust_scale or 1}
	self:edit_pose{channel = Animation.CHANNEL_CUSTOMIZE, node = "breast.R", scale = args.bust_scale or 1}
	self:edit_pose{channel = Animation.CHANNEL_CUSTOMIZE, node = "breastspikiness.L", scale = args.bust_scale or 1}
	self:edit_pose{channel = Animation.CHANNEL_CUSTOMIZE, node = "breastspikiness.R", scale = args.bust_scale or 1}
	-- Apply initial deformation.
	self:update_animations{secs = 0}
	self:deform_mesh()
end

Object.update = function(self, secs)
	-- Update slots.
	if self.slots then
		local species = Species:find{name = self.race}
		for name,object in pairs(self.slots.slots) do
			local slot = species and species.equipment_slots[name]
			if slot and slot.node and self.realized then
				-- Show slot.
				local p,r = self:find_node{name = slot.node, space = "world"}
				local h = object:find_node{name = "#handle"}
				if h then p = p - r * h end
				object.position = p
				object.rotation = r
				object.realized = true
				object:update(secs)
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
	-- Instantiate the model for creatures.
	if self.type == "creature" and self.model and self.model.name then
		self.model = self.model:copy()
	end
	-- Character customizations.
	if self.spec and self.spec.models then
		self:create_character_model{
			body_scale = self.body_scale,
			bust_scale = self.bust_scale,
			equipment = self.equipment,
			eye_color = self.eye_color,
			eye_style = self.eye_style,
			face_style = self.face_style,
			hair_color = self.hair_color,
			hair_style = self.hair_style,
			nose_scale = self.nose_scale,
			race = self.race,
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
