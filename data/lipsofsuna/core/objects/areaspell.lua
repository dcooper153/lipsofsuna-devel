--- TODO:doc
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module core.objects.areaspell
-- @alias AreaSpell

local Class = require("system/class")
local Spell = require("core/objects/spell")

--- TODO:doc
-- @type AreaSpell
local AreaSpell = Class("AreaSpell", Spell)

--- Creates a new area spell.
-- @param clss Area spell class.
-- @param manager Object manager.
-- @param args Arguments.<ul>
--   <li>duration: Duration of the spell.</li>
--   <li>feat: Feat invoking the spell.</li>
--   <li>owner: Caster of the spell.</li>
--   <li>position: Position in world space.</li>
--   <li>spec: Spell spec.</li></ul>
-- @return Spell.
AreaSpell.new = function(clss, manager, args)
	local self = Spell.new(clss, manager, {feat = args.feat, owner = args.owner, spec = args.spec})
	self.physics:set_collision_mask(0)
	self.timer = 1
	self.physics:set_physics("static")
	self.radius = args.radius
	self.duration = args.duration
	self:set_position(args.position)
	self:set_visible(args.realized)
	return self
end

AreaSpell.contact_cb = function(self, args)
end

AreaSpell.update = function(self, secs)
	if self:has_server_data() then
		-- Update periodically.
		self.timer = self.timer + secs
		if self.timer < 1 then return end
		self.timer = self.timer - 1
		-- Apply the feat to each nearby object.
		local objs = Main.objects:find_by_point(self:get_position():copy():add_xyz(0,1,0), self.radius)
		for k,v in pairs(objs) do
			if v:get_visible() and v.class_name ~= "AreaSpell" then
				self.feat:apply{
					object = v,
					owner = self.owner,
					point = v.position,
					projectile = self}
			end
		end
		-- Detach after timeout.
		self.duration = self.duration - 1
		if self.duration <= 0 then
			self:detach()
		end
	end
	-- Update the base class.
	Spell.update(self, secs)
end

return AreaSpell


