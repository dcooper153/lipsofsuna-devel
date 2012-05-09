Staticobject = Class(Object)
Staticobject.class_name = "Staticobject"

Staticobject.new = function(clss, args)
	local self = Object.new(clss, {id = args.id})
	self.static = true
	self.disable_saving = true
	for k,v in pairs(args) do self[k] = v end
	Client.options:apply_object(self)
	return self
end
