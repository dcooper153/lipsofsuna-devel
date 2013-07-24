Aistatespec{
	name = "combat",
	calculate = function(self)
		-- Check for enemies.
		for k,v in pairs(self.enemies) do return 1 end
	end,
	enter = function(self, prev)
		self:calculate_combat_ratings()
		self.target = self.best_enemy
		if prev ~= "combat" then
			local p = self.object.spec:get_personality()
			local s = p and p:get_phrase("angered")
			if s then self.object:action("say", s) end
			self.combat_taunt_timer = math.random(10, 30)
			self.object:set_combat_hint(true)
		end
	end,
	leave = function(self)
		self.object:set_combat_hint()
	end,
	update = function(self, secs)
		-- Check if the target is has died.
		if not self.target or self.target.dead or not self.target:get_visible() then
			self.ai_timer = self.object.spec.ai_update_delay
			return
		end
		-- Decide what combat action to perform next.
		if self.action_timer <= 0 then
			self:choose_combat_action()
		end
		-- Turn towards the target.
		local face = self.object:face_point{point = self.target:get_position(), secs = secs}
		if face < 0.5 then
			self.object:set_movement(0)
			return
		end
		-- Yell some taunts occasionally.
		self.combat_taunt_timer = (self.combat_taunt_timer or 60) - secs
		if self.combat_taunt_timer < 0 then
			local p = self.object.spec:get_personality()
			local s = p and p:get_phrase("combat")
			if s then self.object:action("say", s) end
			self.combat_taunt_timer = math.random(10, 30)
		end
	end}
