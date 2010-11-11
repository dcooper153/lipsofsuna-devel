Startup = Class()

string.split = function(self, sep)
	local sep,fields = sep or " ", {}
	local pattern = string.format("([^%s]+)", sep)
	self:gsub(pattern, function(c) fields[#fields+1] = c end)
	return fields
end

--- Initializes the startup screen.
-- @param clss Startup class.
Startup.init = function(clss)
	local args = string.split(Program.args)
	Startup.mode = args[1] or "--host"
	Startup.host = args[2] or "localhost"
	Startup.port = tonumber(args[3] or "10101")
	clss.group = Widgets.Background{cols = 3, rows = 3, behind = true, fullscreen = true, image = "mainmenu1"}
	clss.group:set_expand{col = 1, row = 1}
	clss.group:set_expand{col = 3}
	clss.text = Widgets.Label()
	clss.button_retry = Widgets.Label{font = "mainmenu", text = "Retry", pressed = function() clss:execute() end}
	clss.button_quit = Widgets.Label{font = "mainmenu", text = "Quit", pressed = function() Program:shutdown() end}
	clss.group2 = Widget{rows = 1, margins = {bottom=30}, spacings = {horz=40}}
	clss.group2:append_col(clss.button_retry)
	clss.group2:append_col(clss.button_quit)
	clss.group:set_child{col = 2, row = 2, widget = clss.text}
	clss.group:set_child{col = 2, row = 3, widget = clss.group2}
	clss.group.floating = true
end

--- Executes the startup command.
-- @param clss Startup class.
Startup.execute = function(clss)
	if Startup.mode == "--host" then
		-- Host a game.
		Program:unload_world()
		Client:host()
		clss.text.text = "Starting the server..."
		clss.host_wait_timer = Timer{delay = 2, func = function(timer)
			if Network:join{host = "localhost", clss.port} then
				clss.text.text = "Connecting to the server..."
				clss.connecting = true
			else
				clss.text.text = "Failed to start the server!"
				clss.connecting = nil
			end
			timer:disable()
		end}
	elseif Startup.mode == "--join" then
		-- Join a game.
		Program:unload_world()
		if Network:join{host = clss.host, port = clss.port} then
			clss.text.text = "Connecting to " .. clss.host .. ":" .. clss.port .. "..."
			clss.connecting = true
		else
			clss.text.text = "Failed to connect to " .. clss.host .. ":" .. clss.port .. "!"
			clss.connecting = nil
		end
	else
		-- Display help.
		clss.text.text = "Usage:\n" ..
			"  lipsofsuna --join <server> <port>\n" ..
			"  lipsofsuna --host localhost <port>\n"
	end
end

--- Finishes the startup when connection has been established.
-- @param clss Startup class.
Startup.finish = function(clss)
	clss.connecting = nil
	clss.joined = true
	clss.group.floating = false
	Gui:init()
	Chargen:execute()
end

Startup:init()

Eventhandler{type = "tick", func = function(self, args)
	if Startup.connecting and Network.connected then
		Startup:finish()
	elseif Startup.joined and not Network.connected then
		Gui:free()
		Chargen:free()
		Startup.group.floating = true
		Startup.button_retry.text = "Reconnect"
		Startup.button_retry.visible = true
		Startup.text.text = "Lost connection to the server!"
	end
end}
