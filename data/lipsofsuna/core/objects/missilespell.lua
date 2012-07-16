require(Mod.path .. "spell")

MissileSpell = Class(Spell)
MissileSpell.class_name = "MissileSpell"

--- Creates a new missile spell.
-- @param clss Missile spell class.
-- @param args Arguments.<ul>
--   <li>feat: Feat invoking the spell.</li>
--   <li>owner: Caster of the spell.</li>
--   <li>spec: Spell spec.</li></ul>
-- @return Spell.
MissileSpell.new = function(clss, args)
	local self = Spell.new(clss, {feat = args.feat, owner = args.owner, spec = args.spec})
	self.speed = 3
	return self
end

MissileSpell.update = function(self, secs)
	if self:has_server_data() then
		-- Adjust rotation.
		-- Controlling is done by copying the rotation from the caster.
		self.rotation = self.owner.rotation * self.owner.tilt
		self.orig_rotation = self.rotation
		-- Adjust velocity.
		-- Velocity is smoothed but approaches the target value quickly.
		self.velocity = (self.velocity + self.rotation * Vector(0,0,-self.speed)) * 0.5
		self.orig_velocity = self.velocity
	end
	-- Update the base class.
	Spell.update(self, secs)
end
