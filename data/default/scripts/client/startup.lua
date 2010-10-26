Startup = Class()

-- Modify these to join a server of your choice.
Startup.addr = "h.lipsofsuna.org"
Startup.port = 10101
Startup.mod = "lipsofsuna"

--- Initializes the startup screen.
-- @param clss Startup class.
Startup.init = function(clss)
	if not Startup.group then
		clss.group = Widgets.Background{cols = 3, rows = 3, behind = true, fullscreen = true, image = "mainmenu1"}
		clss.group:set_expand{col = 1, row = 1}
		clss.group:set_expand{col = 3}
		clss.text = Widgets.Label()
		clss.button_host = Widgets.Label{font = "mainmenu", text = "Host", pressed = function() clss:host() end}
		clss.button_join = Widgets.Label{font = "mainmenu", text = "Join", pressed = function() clss:join() end}
		clss.button_quit = Widgets.Label{font = "mainmenu", text = "Quit", pressed = function() Program:shutdown() end}
		clss.group2 = Widget{rows = 1, margins = {bottom = 30}, spacings = {horz = 40}}
		clss.group2:append_col(clss.button_host)
		clss.group2:append_col(clss.button_join)
		clss.group2:append_col(clss.button_quit)
		clss.group:set_child{col = 2, row = 2, widget = clss.text}
		clss.group:set_child{col = 2, row = 3, widget = clss.group2}
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
	Program:launch_mod{name = clss.mod, args = "--join " .. clss.addr .. " " .. clss.port}
	Program.quit = true
end

--- Hosts a server.
-- @param clss Startup class.
Startup.host = function(clss)
	Program:launch_mod{name = clss.mod, args = "--host localhost " .. clss.port}
	Program.quit = true
end

Startup:init()
