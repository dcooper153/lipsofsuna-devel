Item = Class(Object)

Item.new = function(clss, args)
	local self = Object.new(clss, args)
	if args and args.spec and args.spec.special_effects then
		Object.dict_active[self] = 0.1
	end
	Client.options:apply_object(self)
	return self
end
