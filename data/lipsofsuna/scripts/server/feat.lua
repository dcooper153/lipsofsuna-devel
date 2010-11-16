--- Finds feats.
-- @param clss Feat class.
-- @param args Arguments.<ul>
--   <li>category: Category name.</li>
--   <li>name: Feat name.</li>
--   <li>id: Feat ID.</li>
--   <li>skills: Required skills.</li></ul>
-- @return Feat or nil if matching by name or ID. Table of feats otherwise.
Feat.find = function(clss, args)
	local r = Spec.find(clss, args)
	if not args or not args.skills then return r end
	-- Check for skills.
	if type(r) == "table" then
		local t = {}
		for k,v in pairs(r) do
			if v:usable(args) then
				t[v.name] = t
			end
		end
		return t
	end
	if not feat:usable(args) then return end
	return r
end

--- Performs a feat.<br/>
-- If called as a member function, performs the feat. If called as a class
-- function, looks for a feat by name and performs the one found.
-- @param self Feat or feat class.
-- @param args Arguments.<ul>
--   <li>name: Feat name.</li>
--   <li>user: Object using the feat. (required)</li>
--   <li>stop: True if stopped performing, false if started.</li></ul>
Feat.perform = function(self, args)
	local feat
	-- Find the feat.
	if args.name then
		feat = self:find(args)
		if not feat then return end
	else
		feat = self
	end
	-- Check for cooldown and requirements.
	if not args.stop then
		if args.user.cooldown then return end
		if not feat:usable(args) then return end
		for k,v in pairs(feat.required_reagents) do
			args.user:subtract_items{name = k, count = v}
		end
		local w = feat.required_skills["willpower"]
		if w then Skills:subtract{owner = args.user, skill = "willpower", value = w} end
		if feat.cooldown > 0 then
			args.user.cooldown = feat.cooldown
		end
	end
	-- Playback effects.
	if not args.stop then
		if feat.effect_sound then
			args.user:effect{effect = feat.effect_sound}
		end
		if feat.effect_animation then
			args.user:animate{animation = feat.effect_animation, weight = 10.0}
		end
	end
	-- Call the feat function.
	if feat.toggle or not args.stop then
		if feat.func then feat:func(args) end
	end
end

--- Unlocks a random feat to the player base.
-- @param self Feat class.
-- @param args Arguments.<ul>
--   <li>category: Category name or nil.</li></ul>
-- @return Feat or nil.
Feat.unlock = function(clss, args)
	local choices = {}
	-- Find the unlocked feats.
	if args and args.category then
		local cat = clss.dict_cat[args.category]
		if not cat then return end
		for k,v in pairs(cat) do
			if v.locked then table.insert(choices, v) end
		end
	else
		for k,v in pairs(clss.dict_id) do
			if v.locked then table.insert(choices, v) end
		end
	end
	-- Choose a random feat and unlock it.
	if #choices == 0 then return end
	local feat = choices[math.random(1,#choices)]
	feat.locked = false
	-- Inform clients.
	local packet = Packet(packets.FEAT_UNLOCK, "string", feat.name)
	for k,v in pairs(Player.clients) do
		v:send{packet = packet}
	end
	return feat
end

--- Checks if the feat can be used with the given skills.
-- @param self Feat.
-- @param args Arguments.<ul>
--   <li>skills: Skills of the user.</li>
--   <li>inventory: Inventory of the user.</li>
--   <li>user: Object whose skills and inventory to use.</li></ul>
-- @return True if usable.
Feat.usable = function(self, args)
	-- Check for skills.
	local skills = args.skills or Skills:find{owner = args.user}
	for k,v in pairs(self.required_skills) do
		if not skills then return end
		local val = skills:get_value{skill = k}
		if not val or val < v then return end
	end
	-- Check for reagents.
	local inventory = args.inventory or Inventory:find{owner = args.user}
	for k,v in pairs(self.required_reagents) do
		if not inventory then return end
		local item = inventory:find_object{name = k}
		if not item or item:get_count() < v then return end
	end
	-- Check for ammo.
	for k,v in pairs(self.required_ammo) do
		if not inventory then return end
		local item = inventory:find_object{name = k}
		if not item or item:get_count() < v then return end
	end
	-- Check for weapon.
	if self.required_weapon then
		if not inventory then return self.required_weapon == "melee" end
		local weapon = inventory:get_object{slot = "hand.R"}
		if not weapon then return self.required_weapon == "melee" end
		if not weapon.itemspec.categories[self.required_weapon] then return end
	end
	return true
end

