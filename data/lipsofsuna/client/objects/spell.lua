Spell = Class(Object)
Spell.class_name = "Spell"

Spell.new = function(clss, args)
	local self = Object.new(clss, args)
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
