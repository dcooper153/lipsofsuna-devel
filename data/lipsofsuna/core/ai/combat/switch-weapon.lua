local AiActionSpec = require("core/specs/aiaction")

AiActionSpec{
	name = "switch weapon",
	categories = {["combat"] = true, ["offensive"] = true},
	calculate = function(self, args)
		-- Check that the actor can switch weapons.
		if not args.spec.ai_enable_weapon_switch then return end
		-- Calculate the current weapon rating.
		local curr = 1
		local weapon = self.object:get_weapon()
		if weapon then
			curr = self:calculate_weapon_ratings(weapon)
		end
		-- Calculate the best weapon rating.
		local best = 0
		for k,item in pairs(self.object.inventory.stored) do
			local value = self:calculate_weapon_ratings(item)
			if value >= best then
				best = value
				args.best_weapon = item
			end
		end
		-- Allow switching if the current weapon is worse than others.
		if args.best_weapon == weapon then return end
		if (curr + 5 < best) or (curr == 0 and best > 0) then return 3 end
	end,
	perform = function(self, args)
		local slot = self.object.spec.weapon_slot
		self.object.inventory:equip_object(args.best_weapon, slot)
		self.action_timer = 1
	end}
