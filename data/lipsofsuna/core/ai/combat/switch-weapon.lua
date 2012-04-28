Aiactionspec{
	name = "switch weapon",
	categories = {["combat"] = true, ["offensive"] = true},
	calculate = function(self, args)
		-- Check that the actor can switch weapons.
		if not args.spec.ai_enable_weapon_switch then return end
		-- Calculate the current and new weapon ratings.
		local curr,melee,ranged,throw = 1,1,0,0
		if weapon then curr,melee,ranged,throw = self:calculate_weapon_ratings(weapon) end
		-- Allow switching if the current weapon is worse than others.
		local best = math.max(self.melee_rating, self.ranged_rating, self.throw_rating)
		if (curr + 5 < best) or (curr == 0 and best > 0) then return 3 end
	end,
	perform = function(self, args)
		local slot = self.object.spec.weapon_slot
		if self.melee_rating > self.ranged_rating and self.melee_rating > self.throw_rating then
			if self.best_melee_weapon then
				self.object.inventory:equip_object(self.best_melee_weapon, slot)
			else
				self.object.inventory:unequip_slot(slot)
			end
		elseif self.ranged_rating > self.throw_rating then
			self.object.inventory:equip_object(self.best_ranged_weapon, slot)
		else
			self.object.inventory:equip_object(self.best_throw_weapon, slot)
		end
		self.action_timer = 1
	end}
