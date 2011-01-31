--- Bleeding modifier.
Modifier{name = "bleeding", func = function(self, object, timer, secs)
	-- Initialize the bleeding timer.
	if not object.bleeding_timer or object.bleeding_timer > timer then
		object.bleeding_timer = 0
	end
	-- Damage every five second.
	if object.plague_timer and timer - object.plague_timer > 1 then
		object:damaged(5)
		object.plague_timer = timer
	end
	return timer - secs
end}
