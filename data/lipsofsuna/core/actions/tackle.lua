local Damage = require("arena/damage")

Actionspec{
	name = "tackle",
	categories = { ["melee"] = true },
	start = function(action, move)
		if action.object.cooldown then return end
		-- Setup the collision callback.
		local ground_hits = 0
		local callback = function(self, result)
			-- Forgive a few ground contacts.
			if not result.object then
				ground_hits = ground_hits + 1
				if ground_hits < 30 then return end
			end
			-- Stop the tackle motion.
			action.object.physics:set_contact_events(false)
			self.contact_cb = nil
			-- Calculate the damage.
			local defender = result.object
			local damage = Damage()
			damage:add_unarmed_modifiers(self.skills)
			damage:apply_attacker_physical_modifiers(self)
			damage:apply_defender_armor(defender)
			damage:apply_defender_blocking(defender)
			damage:apply_defender_vulnerabilities(defender)
			-- Apply the damage.
			Main.combat_utils:apply_damage(self, defender, result.tile, damage, result.point)
			-- Bounce the target strongly.
			if defender then
				defender.physics:impulse(Vector(0,400,-400):transform(self:get_rotation()))
			end
		end
		action.object.contact_cb = callback
		action.object.physics:set_contact_events(true)
		-- Initialize the tackle motion
		action.object:action("jump")
		action.velocity = action.object:get_rotation() * Vector(0,0.25,-1) * 10
		action.object:set_velocity(action.velocity)
		--feat:play_effects(args)
		-- Initialize timing.
		action.timer = 0
		action.updates = 0
		-- Enable effect-over-time updates.
		return true
	end,
	update = function(action, secs)
		-- Check for cancellation.
		action.object.cooldown = 1 + 3 * math.random()
		if not action.object:get_visible() then return end
		if action.object.contact_cb ~= callback then return end
		-- Wait for the next tick.
		action.timer = action.timer + secs
		if action.timer < 0.1 then return true end
		action.timer = action.timer - 0.1
		-- Check if done.
		action.updates = action.updates + 1
		if action.updates == 10 then
			action.object.physics:set_contact_events(false)
			action.object.contact_cb = nil
			return
		end
		-- Maintain the velocity.
		action.object:set_velocity(action.velocity)
		return true
	end,
	get_range = function(action)
		return 10
	end,
	get_score = function(action)
		return 1
	end}
