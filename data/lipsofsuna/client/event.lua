Eventhandler{type = "keypress", func = function(self, args)
	Binding:event(args, Ui:handle_event(args))
end}

Eventhandler{type = "keyrelease", func = function(self, args)
	Binding:event(args, Ui:handle_event(args))
end}

Eventhandler{type = "mousepress", func = function(self, args)
	Binding:event(args, Ui:handle_event(args))
end}

Eventhandler{type = "mouserelease", func = function(self, args)
	Binding:event(args, Ui:handle_event(args))
end}

Eventhandler{type = "mousescroll", func = function(self, args)
	Binding:event(args, Ui:handle_event(args))
end}

Eventhandler{type = "mousemotion", func = function(self, args)
	Binding:event(args, Ui:handle_event(args))
end}

Eventhandler{type = "music-ended", func = function(self, args)
	Sound:cycle_music_track()
end}

Eventhandler{type = "quit", func = function(self, args)
	Program.quit = true
end}
