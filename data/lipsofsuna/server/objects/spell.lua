require "server/objects/object"

Spell = Class(Object)
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
	local self = Object.new(clss)
	self.dict_id[self.id] = self
	self.effect = args.effect
	self.feat = args.feat
	self.owner = args.owner
	self.power = args.power
	self.spec = args.spec
	self.gravity = Vector()
	self.model = args.spec.model
	self.physics = "rigid"
	return self
end

--- Handles physics contacts.
-- @param self Object.
-- @param result Contact result.
Spell.contact_cb = function(self, result)
	-- Make sure that applicable.
	if not self.realized then return end
	if result.object == self.owner then return end
	-- Create the arguments for collision callbacks.
	local args = {
		object = result.object,
		owner = self.owner,
		point = result.point,
		projectile = self,
		tile = result.tile}
	-- Call the collision callback of each effect.
	-- Effects can remove themselves from the feat by returning false.
	local left = 0
	for k,v in pairs(self.feat.effects) do
		local effect = Feateffectspec:find{name = v[1]}
		if effect and effect.ranged then
			args.value = v[2]
			args.object = self
			if not effect:ranged(args) then
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
	local ret = Object.fire(self, a)
	self.orig_rotation = self.rotation
	self.orig_velocity = self.velocity
	return ret
end

--- Saves the object to the database.
-- @param self Object.
Spell.save = function(self)
end

Spell.update = function(self, secs)
	-- Prevent impacts from altering movement.
	self.rotation = self.orig_rotation
	self.velocity = self.orig_velocity
end
