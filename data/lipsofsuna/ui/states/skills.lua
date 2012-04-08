Ui:add_state{
	state = "skills",
	label = "Skills",
	init = function()
		-- Create a sorted list of skills and their toggle states.
		local status = Client.data.skills:get_statuses()
		local skills = {}
		for name,status in pairs(status) do
			local unlocked = Client.data.unlocks:get("skill", name)
			if unlocked and status ~= "incompatible" then
				local spec = Skillspec:find{name = name}
				local active = (status == "active")
				local activable = (status == "activable")
				table.insert(skills, {name, spec, active or activable, active})
			end
		end
		table.sort(skills, function(a,b) return a[1] < b[1] end)
		-- Create the skill toggles.
		local widgets = {}
		for k,v in pairs(skills) do
			table.insert(widgets, Widgets.Uiskill(v[2], v[3], v[4]))
		end
		return widgets
	end}
