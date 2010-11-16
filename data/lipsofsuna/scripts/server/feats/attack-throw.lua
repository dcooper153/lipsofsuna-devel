local feat = Feat:find{name = "attack-throw"}
feat.func = function(self, args)
	-- Get weapon.
	local weapon = args.user:get_item{slot = "hand.R"}
	-- Perform ranged attack.
	Attack:ranged{user = args.user, delay = 2, start = 0.5, func = function(user, point)
		local proj = weapon:fire{
			collision = false,
			feat = self,
			point = point + Vector(0.2, 0.3, -1.5),
			owner = args.user,
			speed = 10,
			timer = weapon.itemspec.destroy_timer}
		if weapon.itemspec.categories["boomerang"] then
			-- Setup boomerang mode.
			proj.rotated = 0
			proj.rotation = Quaternion{axis = Vector(0,0,1), angle = -0.5 * math.pi}
			proj:animate{animation = "fly", channel = 2, permanent = true}
			proj.gravity = Vector(0,2,0)
			proj.timer = Timer{delay = 0, func = function(self, secs)
				-- Adjust velocity vector.
				local m = 1.55 * math.pi
				local r = math.min(secs * 1.3 * math.pi, m - proj.rotated)
				proj.velocity = Quaternion{axis = Vector(0,1,0), angle = r} * proj.velocity
				-- Stop after a while.
				proj.rotated = proj.rotated + r
				if proj.rotated >= m then
					self:disable()
					proj.gravity = Config.gravity
				end
			end}
			proj.contact_cb = function(self, result)
				if result.object == proj.owner then
					-- Owner catch.
					if not proj.owner:get_item{slot = "hand.R"} then
						proj.owner:get_item{slot = "hand.R", object = proj}
					else
						proj.owner:add_item{object = proj}
					end
				else
					-- Damage target.
					Combat:apply_melee_hit{
						attacker = args.user,
						feat = Feat:find{name = "attack"},
						point = result.point,
						target = result.object,
						tile = result.tile,
						weapon = proj}
				end
				-- Disable boomerang mode.
				proj.timer:disable()
				proj.gravity = Config.gravity
				proj:animate{channel = 2}
				proj.contact_cb = nil
			end
		end
	end}
end
