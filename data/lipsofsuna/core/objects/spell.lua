require(Mod.path .. "simulation")

Spell = Class(SimulationObject)
Spell.class_name = "Spell"
Spell.dict_id = setmetatable({}, {__mode = "kv"})

--- Creates a new spell.
-- @param clss Spell class.
-- @param args Arguments.<ul>
--   <li>effect: Effect type.</li>
--   <li>feat: Feat invoking the spell.</li>
--   <li>model: Model name.</li>
--   <li>owner: Caster of the spell.</li>
--   <li>position: Position in world space.</li>
--   <li>power: Effect power</li>
--   <li>spec: Spell spec.</li></ul>
-- @return Spell.
Spell.new = function(clss, args)
	local self = SimulationObject.new(clss)
	self.dict_id[self.id] = self
	self.effect = args.effect
	self.feat = args.feat
	self.owner = args.owner
	self.power = args.power
	self.spec = args.spec
	self.gravity = Vector()
	self:set_model_name(args.spec.model)
	self.physics = "rigid"
	return self
end

--- Handles physics contacts.
-- @param self Object.
-- @param result Contact result.
Spell.contact_cb = function(self, result)
	-- Make sure that applicable.
	if not self:get_visible() then return end
	if result.object == self.owner then return end
	-- Create the arguments for collision callbacks.
	local args = {
		object = result.object,
		owner = self.owner,
		point = result.point,
		projectile = self,
		tile = result.tile}
	-- Apply an impulse to the target.
	self.feat:apply_impulse(args)
	self.feat:play_effects_impact(args)
	-- Call the collision callback of each effect.
	-- Effects can remove themselves from the feat by returning false.
	local left = 0
	for k,v in pairs(self.feat.effects) do
		local effect = Feateffectspec:find{name = v[1]}
		if effect and effect.ranged then
			local feat = Feat{animation = self.feat.animation, effects = {v}}
			if not feat:apply_ranged(args) then
				self.feat.effects[k] = nil
			else
				left = left + 1
			end
		else
			self.feat.effects[k] = nil
		end
	end
	-- Detach if no effects were left.
	if left == 0 then
		self:detach()
	end
end

Spell.fire = function(self, args)
	local a = args or {}
	a.collision = true
	a.feat = self.feat
	a.owner = self.owner
	local ret = SimulationObject.fire(self, a)
	self.orig_rotation = self.rotation
	self.orig_velocity = self.velocity
	return ret
end

Spell.update = function(self, secs)
	if not self:get_visible() then return end
	if self:has_server_data() then
		-- Prevent impacts from altering movement.
		self.rotation = self.orig_rotation
		self.velocity = self.orig_velocity
	end
	-- Update the base class.
	SimulationObject.update(self, secs)
end
