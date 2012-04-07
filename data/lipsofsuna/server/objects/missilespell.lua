require "server/objects/spell"

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
	-- Adjust rotation.
	-- Controlling is done by copying the rotation from the caster.
	self.rotation = self.owner.rotation * self.owner.tilt
	-- Adjust velocity.
	-- Velocity is smoothed but approaches the target value quickly.
	self.velocity = (self.velocity + self.rotation * Vector(0,0,-self.speed)) * 0.5
end
