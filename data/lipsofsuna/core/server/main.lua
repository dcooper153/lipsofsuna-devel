Main.main_start_hooks:register(5, function(secs)
	Main.server = Server --FIXME
end)

Main.update_hooks:register(5, function(secs)
	Main.timing:start_action("server")
	Main.server:update(secs)
end)
