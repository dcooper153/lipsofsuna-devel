Attack = {}

--- Gets the blade position in the requested equipment slot.
-- @param clss Object.
-- @param args Arguments.<ul>
--   <li>object: Object whose blade to find. (required)</li>
--   <li>slot: Equipment slot name.</li></ul>
-- @return Blade position relative to the object.
Attack.find_blade_point = function(clss, args)
	-- Find weapon node in the owner.
	local name = args.slot or "hand.R"
	local node,rot = args.object:find_node{name = "#" .. name}
	if not node then return end
	-- Find weapon object in the slot.
	local weapon = args.object:get_item{slot = slot}
	if not weapon then return node end
	-- Find blade node in the weapon.
	local blade = slot.object:find_node{name = "#blade"}
	if not blade then return node end
	return node + rot * blade
end

--- Performs a ranged attack.
-- @param clss Attack class.
-- @param args Arguments.<ul>
--   <li>delay: Attack cooldown delay.</li>
--   <li>func: Callback function called when firing. (required)</li>
--   <li>user: Attacking object. (required)</li>
--   <li>radius: Radius of the collision test sphere.</li>
--   <li>slot: Slot or node name that serves as the attack point.</li>
--   <li>start: Time into animation when to shoot.</li></ul>
Attack.ranged = function(clss, args)
	local t0 = args.start or 1
	local t1 = args.delay or 1
	Thread(function()
		-- Sleep some.
		local t = 0.0
		while t < t0 do t = t + coroutine.yield() end
		-- Call callback.
		local pos = clss:find_blade_point{object = args.user, slot = args.slot}
		local ret,err = pcall(args.func, args.user, pos)
		if not ret then print("Attack.ranged: " .. err) end
		-- Cooldown.
		while t < t1 do t = t + coroutine.yield() end
	end)
end

--- Performs a ray cast.
-- @param clss Attack class.
-- @param args Arguments.<ul>
--   <li>func: Callback function called when hit something. (required)</li>
--   <li>length: Length of the cast lay.</li>
--   <li>radius: Radius of the collision test sphere.</li>
--   <li>start: Start position relative to the user.</li>
--   <li>user: Attacking object. (required)</li>
-- @return Sweep result or nil.
Attack.ray = function(clss, args)
	local src = args.start or Vector()
	local dst = src - Vector(0, 0, args.length or 1)
	return args.user:sweep_sphere{src = src, dst = dst, radius = args.radius or 0.3}
end

--- Performs an attack sweep.
-- @param clss Attack class.
-- @param args Arguments.<ul>
--   <li>func: Callback function called when hit something. (required)</li>
--   <li>user: Attacking object. (required)</li>
--   <li>radius: Radius of the collision test sphere.</li>
--   <li>slot: Slot or node name that serves as the attack point.</li>
--   <li>start: Time into animation when to begin testing.</li>
--   <li>stop: Time into animation when to stop testing.</li></ul>
Attack.sweep = function(clss, args)
	local r = args.radius or 0.3
	local t0 = args.start or 0.1
	local t1 = args.stop or 0.5
	Thread(function()
		local t = 0.0
		while t < t0 do t = t + coroutine.yield() end
		local prev = clss:find_blade_point{object = args.user, slot = args.slot}
		local prevp = nil
		while t < t1 do
			-- Sweep segment.
			local curr = clss:find_blade_point{object = args.user, slot = args.slot}
			local currp = args.user.position + args.user.rotation * curr
			local result = args.user:sweep_sphere{src = prev, dst = curr, radius = r}
			-- Check for hits.
			if result then
				Particles:ray{src = prevp, dst = result.point, life = 0.2}
				local fract = (t - t0) / (t1 - t0)
				local ret,err = pcall(args.func, fract, result)
				if not ret then print("Attack.sweep: " .. err) end
				break
			end
			-- Speed lines.
			Particles:ray{src = prevp, dst = currp, life = 0.2}
			prevp = currp
			-- Next segment.
			t = t + coroutine.yield()
			if not args.user.realized then break end
			prev = curr
		end
	end)
end
