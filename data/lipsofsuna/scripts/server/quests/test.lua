Dialog{name = "companion", main = function(self)
	local opts = {"About Suna", "About you", "Follow me", "Wait here", "Open inventory", "Change strategy", "Nevermind"}
	local ret = self:dialog("Hello!", opts)
	if ret == opts[1] then
		self:dialog("Who would have thought that there are some huge\n" ..
			"dungeons hidden below the World Tree? Not me, for one!", {"More"})
		self:dialog("So far, it looks like there isn't much to see around here.\n" ..
			"Mainly just some dumb tunnels leading nowhere.", {"More"})
		self:dialog("However, I got a feeling that we're still\n" ..
			"going to die here somehow.", {"End"})
	elseif ret == opts[2] then
		self:dialog("My name is " .. self.object.name, {"End"})
	elseif ret == opts[3] then
		self:dialog("Alright, sticking close!", {"End"})
		self.object.master = self.user
	elseif ret == opts[4] then
		self:dialog("Fine, whatever.", {"End"})
		self.object.master = nil
	elseif ret == opts[5] then
		self:dialog("Here you go.", {"End"})
		local inv = Inventory:find{owner = self.object}
		if inv then
			local user = self.user
			inv:subscribe{object = user, callback = function(args) user:inventory_cb(args) end}
		end
	elseif ret == opts[6] then
		local opts = {"Attack monsters", "Don't attack monsters", "Nevermind"}
		local ret = self:dialog("What's the plan, then?", opts)
		if ret == opts[1] then
			self:dialog("Let's get our hands dirty... with hot blood!", {"End"})
			self.object.neet = nil
		elseif ret == opts[2] then
			self:dialog("Yawn, when will we sleep?", {"End"})
			self.object.neet = true
		end
	end
end}
