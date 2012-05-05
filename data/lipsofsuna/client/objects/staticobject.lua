Staticobject = Class(Object)

Staticobject.new = function(clss, args)
	local self = Object.new(clss, {id = args.id})
	self.static = true
	self.disable_saving = true
	self.shadow_casting = Options.inst.shadow_casting_obstacles
	for k,v in pairs(args) do self[k] = v end
	return self
end
