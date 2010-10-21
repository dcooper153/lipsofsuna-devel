--[[
Quest progress:
 0: Inactive.
 1: Asked about the worm.
 2: Touched the worm.
 3: Killed the worm.
 4: Angered the NPC.
 5: Completed.
Completion conditions:
 * The NPC has been killed.
 * The NPC has been angered and killed.
 * The worm has been killed and the NPC was told that the pet is still alive.
 * The worm has been killed and the NPC was told that the pet is dead.
--]]

local name = Names:random{race = "Aer", gender = "female"}

Dialog{name = "peculiarpetnpc", unique = true,
die = function(self)
	-- Find the quest.
	local quest = Quest:find{name = "Peculiar Pet"}
	if not quest then return end
	-- Update the quest.
	quest:update{status = "completed", progress = 5, text = name ..
		"has been killed."}
end,
main = function(self)
	local lines = {
		"What is that bloodworm doing here?",
		"Could you tell me about your pet?",
		"Goodbye.",
		"Because it's a bloodworm, no matter how you look at it.",
		"That isn't your pet.",
		"I wanted to make you suffer.",
		"In the belly of the worm.",
		"She was chased away by the worm."}
	-- Find the quest.
	local quest = Quest:find{name = "Peculiar Pet"}
	if not quest then return end
	-- Questions related to the worm.
	local qworm = function()
		self:line("Bloodworm? If that was supposed to be a joke, it wasn't funny.")
		self:line("Why does everyone keep calling my poor Puppy a worm?")
		local a = self:choice{lines[4], lines[3]}
		if a == lines[3] then return end
		self:line("You're crazy! Seeing things, I tell you. Get away from me!")
		if quest.progress == 0 then
			quest:update{status = "active", progress = 1, text = "We have been told by " ..
				name .. " that we are crazy because we see a bloodworm in place " ..
				"of her pet, Puppy."}
		end
		return true
	end
	-- Questions related to the pet.
	local qpet = function()
		self:line("She's such a sweet little furball. Puppy wouldn't harm a fly.")
		self:line("Lately, she's been a little...")
		self:line("(Pauses as if lost in thoughts)")
		self:line("I'd never forgive myself if she came into harm's way.")
		return true
	end
	-- Evil resolution.
	local qevil = function()
		self:line("I don't know why or how you did this but I will never forgive you!")
		quest:update{status = "active", progress = 4, text = quest.text ..
			" We have angered " .. name .. " by suggesting that we killed the" ..
			" pet and turned the corpse into a dead bloodworm to offend her."}
		-- Anger the NPC.
		self.object.species.ai_enable_combat = true
		self.object.species:set_factions{"evil"}
		return false
	end
	-- Good and neutral resolutions.
	local qgood = function()
		self:line("Then where is Puppy?")
		local a = self:choice{lines[7], lines[8]}
		if a == lines[7] then
			self:line("It can't be! No! Puppy... dead. My last friend... dead.")
			self:line("(Wields a knife and stabs herself.)")
			self:line("Puppy...")
			quest:update{status = "completed", progress = 5, text = quest.text ..
				" " .. name .. " has fallen into despair after realizing " ..
				"the tragic demise of her pet and has ended her life."}
			self.object:die()
		else
			self:line("I'm so happy that it's again safe for Puppy to return.")
			self:line("I'll teach you this feat in return for killing the worm.")
			Feat:unlock()
			self:line("I'll wait for Puppy to return, no matter how long it takes.")
			quest:update{status = "completed", progress = 5, text = quest.text ..
				" " .. name .. " was convinced that Puppy is still alive. " ..
				"She's waiting for the pet to return back home."}
			return
		end
	end
	-- Dialog main.
	local greet = "Hello!"
	while true do
		local a
		local c = true
		if self.object.dead then
			-- The NPC has been killed already.
			self:line("(She's dead.)")
			return
		end
		if quest.progress == 0 then
			-- The quest hasn't been started yet. The player can start the
			-- quest here by asking about the worm.
			self:line(greet)
			a = self:choice{lines[1], lines[3]}
			if a == lines[3] then return end
			if a == lines[1] then c = qworm() end
		elseif quest.progress <= 2 then
			-- The pet is alive and the quest has been started. We allow
			-- the players to ask some questions about the pet here.
			self:line(greet)
			a = self:choice{lines[1], lines[2], lines[3]}
			if a == lines[3] then return end
			if a == lines[1] then c = qworm() end
			if a == lines[2] then c = qpet() end
		elseif quest.progress == 3 then
			-- The worm was killed but the quest hasn't been completed yet.
			-- The player can complete the quest or anger the NPC here.
			self:line("What have you do done? You... my poor little Puppy...")
			self:line("But, this... what is this? Why a worm? Why?")
			a = self:choice{lines[5], lines[6], lines[3]}
			if a == lines[3] then return end
			if a == lines[5] then c = qgood() end
			if a == lines[6] then c = qevil() end
		elseif quest.progress == 4 then
			-- The NPC is angered and cannot be chatted with.
			return
		else
			-- The quest has been completed.
			self:line("I hope that Puppy is coming home soon.")
			self:choice{lines[3]}
			return
		end
		if not c then return end
		greet = "Anything else you wanted to talk about?"
	end
end}

Dialog{name = "peculiarpetworm", unique = true,
die = function(self)
	-- Find the quest.
	local quest = Quest:find{name = "Peculiar Pet"}
	if not quest then return end
	-- Update the quest. It's possible that the quest was already completed
	-- by the NPC being killed so we need to check for the progress here.
	if quest.progress < 3 then
		quest:update{status = "active", progress = 3, text = "The bloodworm posing as .. " ..
			name .. "'s pet has been killed."}
	end
end,
main = function(self)
	local opts = {"(Touch the monster.)", "(Leave the monster.)"}
	local msgs = {
		"(The foul beast stares you hungrily.)",
		"(The foul beast extends its tongue.)",
		"(The foul beast grins widely.)",
		"(The foul beast salivates blood.)"}
	-- Find the quest.
	local quest = Quest:find{name = "Peculiar Pet"}
	if not quest then return end
	-- Dialog main.
	if self.object.dead then
		-- The worm is already dead.
		self:line("(It's dead.)")
	else
		-- Allow the player to touch the worm.
		self:line(msgs[math.random(1, #msgs)])
		local a = self:choice(opts)
		if a == opts[1] then
			self:line("(The squirmy and stinky worm feels sticky and pulsing hot to touch.)")
			if quest.progress == 1 then
				quest:update{status = "active", progress = 2, text = quest.text ..
					" However, no matter how you look at it, the bloodworm" ..
					" really is a bloodworm. It looks like one, feels like" ..
					" one, and even stinks like one."}
			end
		end
	end
end}
