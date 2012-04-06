require "server/objects/spell"

MissileSpell = Class(Object)
MissileSpell.class_name = "MissileSpell"

--- Creates a new missile spell.
-- @param clss Spell class.
-- @param args Arguments.<ul>
--   <li>feat: Feat invoking the spell.</li>
--   <li>owner: Caster of the spell.</li>
--   <li>spec: Spell spec.</li></ul>
-- @return Spell.
MissileSpell.new = function(clss, args)
	local self = Object.new(clss, {feat = args.feat, owner = args.owner, spec = args.spec})
	self.gravity = Vector()
	self.model = args.spec.model
	self.physics = "rigid"
	self.speed = 3
	self:fire{collision = true, feat = args.feat, owner = args.owner, speed = self.speed}
	Spell.dict_id[self.id] = self
	return self
end

--- Handles physics contacts.
-- @param self Object.
-- @param result Contact result.
MissileSpell.contact_cb = function(self, result)
	-- Make sure that applicable.
	if not self.realized then return end
	if result.object == self.owner then return end
	-- Apply the effects.
	self.feat:apply{
		attacker = self.owner,
		point = result.point,
		projectile = self,
		target = result.object,
		tile = result.tile}
	-- Vanish.
	self:detach()
end

--- Saves the object to the database.
-- @param self Object.
MissileSpell.save = function(self)
end

MissileSpell.update = function(self, secs)
	-- Adjust rotation.
	-- Controlling is done by copying the rotation from the caster.
	self.rotation = self.owner.rotation * self.owner.tilt
	-- Adjust velocity.
	-- Velocity is smoothed but approaches the target value quickly.
	self.velocity = (self.velocity + self.rotation * Vector(0,0,-self.speed)) * 0.5
end
