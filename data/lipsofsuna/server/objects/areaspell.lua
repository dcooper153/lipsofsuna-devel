require "server/objects/spell"

AreaSpell = Class(Spell)
AreaSpell.class_name = "AreaSpell"

--- Creates a new area spell.
-- @param clss Area spell class.
-- @param args Arguments.<ul>
--   <li>duration: Duration of the spell.</li>
--   <li>feat: Feat invoking the spell.</li>
--   <li>owner: Caster of the spell.</li>
--   <li>position: Position in world space.</li>
--   <li>spec: Spell spec.</li></ul>
-- @return Spell.
AreaSpell.new = function(clss, args)
	local self = Spell.new(clss, {feat = args.feat, owner = args.owner, spec = args.spec})
	self.collision_mask = 0
	self.timer = 0
	self.physics = "static"
	self.duration = args.duration
	self.position = args.position
	self.realized = args.realized
	return self
end

AreaSpell.contact_cb = function(self, args)
end

AreaSpell.update = function(self, secs)
	-- Update periodically.
	self.timer = self.timer + secs
	if self.timer < 0.1 then return end
	self.timer = self.timer - 0.1
	-- Apply the feat to each nearby object.
	local objs = Object:find{point = self.position + Vector(0,1), radius = 1.3}
	for k,v in pairs(objs) do
		if v.realized and v.class_name ~= "AreaSpell" then
			self.feat:apply{
				object = v,
				owner = self.owner,
				point = v.position,
				projectile = self}
		end
	end
	-- Detach after timeout.
	self.duration = self.duration - 0.1
	if self.duration <= 0 then
		self:detach()
	end
end
