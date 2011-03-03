Startup = Class()

Startup.addr = "localhost"
Startup.port = 10101
Startup.mod = "lipsofsuna"

--- Initializes the startup screen.
-- @param clss Startup class.
Startup.init = function(clss)
	if not Startup.group then
		clss.text = Widgets.Label()
		clss.button_host = Widgets.Label{font = "mainmenu", text = "Host", pressed = function() clss:host() end}
		clss.button_join = Widgets.Label{font = "mainmenu", text = "Join", pressed = function() clss:join() end}
		clss.button_quit = Widgets.Label{font = "mainmenu", text = "Quit", pressed = function() Program:shutdown() end}
		clss.entry_server = Widgets.Entry{text = clss.addr}
		clss.entry_server:set_request{width = 150}
		clss.entry_mod = Widgets.Entry{text = clss.mod}
		clss.entry_mod:set_request{width = 150}
		clss.group1 = Widget{cols = 3, rows = 2, margins = {5,5,5,5}}
		clss.group1:set_child{col = 2, row = 1, widget = Widgets.Label{text = "Server"}}
		clss.group1:set_child{col = 3, row = 1, widget = clss.entry_server}
		clss.group1:set_child{col = 2, row = 2, widget = Widgets.Label{text = "Game"}}
		clss.group1:set_child{col = 3, row = 2, widget = clss.entry_mod}
		clss.group1:set_expand{col = 1}
		clss.group2 = Widget{rows = 1, margins = {bottom = 30}, spacings = {horz = 40}}
		clss.group2:append_col(clss.button_host)
		clss.group2:append_col(clss.button_join)
		clss.group2:append_col(clss.button_quit)
		clss.group3 = Widget{cols = 3, rows = 2}
		clss.group3:set_expand{col = 1, row = 1}
		clss.group3:set_expand{col = 3}
		clss.group3:set_child{col = 2, row = 1, widget = clss.text}
		clss.group3:set_child{col = 2, row = 2, widget = clss.group2}
		clss.group = Widgets.Background{cols = 1, rows = 2, behind = true, fullscreen = true, image = "mainmenu1"}
		clss.group:set_expand{col = 1, row = 1}
		clss.group:set_child{col = 1, row = 1, widget = clss.group1}
		clss.group:set_child{col = 1, row = 2, widget = clss.group3}
		clss.group.floating = true
	else
		clss.group.floating = true
	end
	Sound.music_fading = 5.0
	Sound.music_volume = 0.2
	Sound.music = "fairytale8"
end

--- Hides the startup screen.
-- @param clss Startup class.
Startup.free = function(clss)
	clss.group.floating = false
	if clss.host_wait_timer then
		clss.host_wait_timer:disable()
		clss.host_wait_timer = nil
	end
end

--- Joins a server.
-- @param clss Startup class.
Startup.join = function(clss)
	clss.mod = clss.entry_mod.text
	clss.addr = clss.entry_server.text
	Program:launch_mod{name = clss.mod, args = "--join " .. clss.addr .. " " .. clss.port}
	Program.quit = true
end

--- Hosts a server.
-- @param clss Startup class.
Startup.host = function(clss)
	clss.mod = clss.entry_mod.text
	clss.addr = clss.entry_server.text
	Program:launch_mod{name = clss.mod, args = "--host localhost " .. clss.port}
	Program.quit = true
end

Startup:init()
