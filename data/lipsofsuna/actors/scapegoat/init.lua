Dialogspec{name = "scapegoat",
	{"branch", cond_dead = true,
		{"loot"}}}

Dialogspec{name = "scapegoat death",
	{"branch generate", function(self)
		-- Adds a branch for a random quest.
		-- One scapegoat can be the mark for multiple kill quests. This function
		-- is called for each quest. It updates the quest status so that the
		-- player can claim the reward from the main actor of the quest.
		local add = function(branch, var_name, quest_name)
			-- Find the quest.
			local quest = Quest:find{name = quest_name}
			if not quest then return end
			-- Get information on the main quest actor.
			local actor = self.object.variables[var_name .. "_mark_actor"] or "the contact person"
			local marker = self.object.variables[var_name .. "_mark_marker"]
			-- Append a branch for this quest.
			table.insert(branch,
				{"branch", check = {{"quest active", quest_name}},
					{"var clear", var_name .. "_mark_actor"},
					{"var clear", var_name .. "_mark_marker"},
					{"var clear", var_name .. "_mark_quest"},
					{"flag", var_name .. "_mark_killed"},
					{"quest", quest_name, status = "active", marker = marker, text = string.format("%s has been killed. Return to %s for the reward.", self.object.spec.name, actor)}
				})
		end
		-- Create a branch for each quest involving the actor.
		if not self.object.variables then return end
		local branch = {"branch",
			{"flag clear", "scapegoat_alive_" .. self.object.spec.name}}
		for k,v in pairs(self.object.variables) do
			if string.match(k, ".*_mark_quest$") then
				add(branch, string.sub(k, 0, -12), v)
			end
		end
		return branch
	end}}

Species{
	name = "Bobbles",
	categories = {"scapegoat"},
	base = "aer",
	dialog = "scapegoat",
	marker = "bobbles",
	eye_style = "random",
	hair_style = "random",
	inventory_items = {"dress"},
	preset = "Male 3"}

Species{
	name = "Mark",
	categories = {"scapegoat"},
	base = "aer",
	dialog = "scapegoat",
	marker = "mark",
	eye_style = "random",
	hair_style = "random",
	inventory_items = {"dress"},
	preset = "Male 1"}

Species{
	name = "Jerkins",
	categories = {"scapegoat"},
	base = "aer",
	dialog = "scapegoat",
	marker = "jerkins",
	eye_style = "random",
	hair_style = "random",
	inventory_items = {"dress"},
	preset = "Male 2"}

Species{
	name = "Rubert",
	categories = {"scapegoat"},
	base = "aer",
	dialog = "scapegoat",
	marker = "rubert",
	eye_style = "random",
	hair_style = "random",
	inventory_items = {"dress"},
	preset = "Male 3"}

Species{
	name = "Rocky",
	categories = {"scapegoat"},
	base = "aer",
	dialog = "scapegoat",
	marker = "rocky",
	eye_style = "random",
	hair_style = "random",
	inventory_items = {"dress"},
	preset = "Male 2"}
