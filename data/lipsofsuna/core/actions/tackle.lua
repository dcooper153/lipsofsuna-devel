local Coroutine = require("system/coroutine")

Actionspec{
	name = "tackle",
	func = function(feat, info, args)
		-- Setup the collision callback.
		local ground_hits = 0
		local callback = function(self, result)
			-- Forgive a few ground contacts.
			if not result.object then
				ground_hits = ground_hits + 1
				if ground_hits < 30 then return end
			end
			-- Stop the tackle motion.
			args.user:set_contact_events(false)
			self.contact_cb = nil
			-- Apply the feat effects.
			local args = {
				object = result.object,
				owner = self,
				point = result.point,
				tile = result.tile}
			feat:play_effects_impact(args)
			feat:apply_touch(args)
			-- Bounce the target strongly.
			if result.object then
				result.object:impulse{impulse = self:get_rotation() * Vector(0, 400, -400)}
			end
		end
		args.user.contact_cb = callback
		args.user:set_contact_events(true)
		-- Initialize the tackle motion
		args.user:jump()
		local vel = args.user:get_rotation() * Vector(0,0.25,-1) * 10
		args.user:set_velocity(vel)
		feat:play_effects(args)
		-- Maintain the tackle motion.
		Coroutine(function(t)
			for i = 1,10 do
				Coroutine:sleep(0.1)
				if not args.user:get_visible() then break end
				if args.user.contact_cb ~= callback then break end
				args.user:set_velocity(vel)
			end
			args.user:set_contact_events(false)
			args.user.contact_cb = nil
		end)
	end}
