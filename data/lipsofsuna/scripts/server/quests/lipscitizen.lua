Dialog{name = "lipscitizen", main = function(self)
	if self.object.dead then return end
	self:line("Greetings!")
	while true do
		local opts = {"Tell me about the town.", "Anything intreresting going on?", "Goodbye."}
		local a = self:choice(opts)
		if a == opts[1] then
			self:line("The deserted town was here already when the first heroes arrived.")
			self:line("It's almost as if the town appeared out of nowhere when the World Tree fell.")
		elseif a == opts[2] then
			local quest = Quest:find{name = "Peculiar Pet"}
			if quest.status ~= "completed" then
				self:line(Quest.peculiar_pet_npc_name .. " has been acting strange lately.")
				self:line("She lives in the north-west corner of the town.")
			else
				self:line("Nothing apart from the end of the world.")
			end
		else
			self:line("Farewell!")
			return
		end
	end
end}
