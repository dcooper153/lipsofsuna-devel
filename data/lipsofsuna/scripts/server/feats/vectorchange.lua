local feat = Feat:find{name = "vectorchange"}
feat.func = function(self, args)
	-- Reverse velocities of objects.
	for i,obj in pairs(Object:find{point = args.user.position, radius = self.radius}) do
		v = obj.velocity
		obj.velocity = obj.velocity * -1.0
	end
end
