--- Burning modifier.
Modifier{name = "burning", func = function(self, object, timer, secs)
	-- Update the burning timer.
	if not object.burning_timer then
		object.burning_timer = secs
	else
		object.burning_timer = object.burning_timer + secs
	end
	-- Damage every second.
	if object.burning_timer > 1 then
		object:damaged(math.random(4,7))
		object.burning_timer = object.burning_timer - 1
	end
	-- End after a while.
	return timer - secs
end}
