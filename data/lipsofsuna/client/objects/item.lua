Item = Class(Object)

Item.new = function(clss, args)
	local self = Object.new(clss, args)
	self.shadow_casting = Options.inst.shadow_casting_items
	if args and args.spec and args.spec.special_effects then
		Object.dict_active[self] = 0.1
	end
	return self
end
