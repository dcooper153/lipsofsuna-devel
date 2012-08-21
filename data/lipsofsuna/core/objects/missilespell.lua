local Class = require("system/class")
local Spell = require("core/objects/spell")

local MissileSpell = Class("MissileSpell", Spell)

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
		self:set_rotation(self.owner:get_rotation() * self.owner.tilt)
		self.orig_rotation = self:get_rotation()
		-- Adjust velocity.
		-- Velocity is smoothed but approaches the target value quickly.
		self:set_velocity((self:get_velocity() + self:get_rotation() * Vector(0,0,-self.speed)) * 0.5)
		self.orig_velocity = self:get_velocity()
	end
	-- Update the base class.
	Spell.update(self, secs)
end

return MissileSpell
