--- Plague modifier.
Modifier{name = "plague", func = function(self, object, timer, secs)
	-- Initialize the plague timer.
	if not object.plague_timer or object.plague_timer > timer then
		object.plague_timer = 0
	end
	-- Damage and infect every five second.
	if object.plague_timer and timer - object.plague_timer > 5 then
		-- Damage.
		object:damaged(5)
		object.plague_timer = timer
		-- Infect.
		local near = Object:find{point = object.position, radius = 5}
		for k,v in pairs(near) do
			if math.random() > 0.1 then
				v:inflict_modifier("plague", 10000)
			end
		end
	end
	-- Plague never ends on its own since the timer is incremented.
	return timer + secs
end}
