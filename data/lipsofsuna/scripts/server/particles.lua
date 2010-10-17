Particles = {}

Particles.create = function(self, point, effect)
	Vision:event{type = "world-effect", point = point, effect = effect}
end

Particles.debug = function(self, point, effect)
	Vision:event{type = "particle-effect", point = point, effect = effects.DEFAULT}
end

Particles.ray = function(self, args)
	if not args.src or not args.dst then return end
	Vision:event{type = "particle-ray", point = args.src, src = args.src, dst = args.dst, life = args.life}
end
