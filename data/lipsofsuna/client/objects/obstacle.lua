require "client/objects/remote"

Obstacle = Class(RemoteObject)

Obstacle.new = function(clss, args)
	local self = RemoteObject.new(clss, args)
	if args and args.spec and args.spec.special_effects then
		self:activate(0.1)
	end
	Client.options:apply_object(self)
	return self
end
