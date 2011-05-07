--- Light modifier.
Modifier{name = "light", func = function(self, object, timer, secs)
	return timer - secs
end}
