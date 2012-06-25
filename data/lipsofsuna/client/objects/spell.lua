require "client/objects/remote"

Spell = Class(RemoteObject)
Spell.class_name = "Spell"

Spell.new = function(clss, args)
	local self = RemoteObject.new(clss, args)
	if args and args.spec then
		if args.spec.particle then
			self.particle = args.spec.particle
		elseif args.spec.model then
			self.model = args.spec.model
		end
	end
	self.spec = nil -- FIXME: Why does setting this make the object disable?
	return self
end

Spell.detach = function(self, soft)
	if soft then
		-- Delayed removal to allow particle fading.
		self.destroy_timer = 5
		self.particle_emitting = false
	else
		-- Forced instant removal.
		RemoteObject.detach(self)
	end
end

Spell.update = function(self, secs)
	-- Call base update.
	RemoteObject.update(self, secs)
	-- Handle delayed destruction.
	if self.destroy_timer then
		self.destroy_timer = self.destroy_timer - secs
		if self.destroy_timer <= 0 then
			self:detach()
		end
	end
end
