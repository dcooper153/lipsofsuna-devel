
Ui:add_state{
	state = "skills",
	init = function()
		-- Get the skill specs.
		local skills = {}
		for k,v in pairs(Skillspec.dict_name) do
			table.insert(skills, v)
		end
		table.sort(skills, function(a,b) return a.name < b.name end)
		-- Create the skill toggles.
		local widgets = {}
		for k,v in pairs(skills) do
			local skill = Client.data.skills[v.name]
			local active = skill and skill.active
			local value = skill and skill.value
			table.insert(widgets, Widgets.Uiskill(v, active, value))
		end
		return widgets
	end}
