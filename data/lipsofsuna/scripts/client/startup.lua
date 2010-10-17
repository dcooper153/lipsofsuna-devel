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
	clss.group = Group{cols = 3, rows = 3, behind = true, fullscreen = true, style = "mainmenu"}
	clss.group:set_expand{col = 1, row = 1}
	clss.group:set_expand{col = 3}
	clss.text = Button{style = "label"}
	clss.button_retry = Button{style = "mainmenu-label", text = "Retry", pressed = function() clss:execute() end, visible = false}
	clss.button_quit = Button{style = "mainmenu-label", text = "Quit", pressed = function() Program:shutdown() end}
	clss.group2 = Group{rows = 1, margins = {bottom=30}, spacings = {horz=40}}
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
		clss.host_wait_timer = Timer{delay = 1, func = function(self)
			if Network:join{host = "localhost", clss.port} then
				clss.joined = true
				clss.group.floating = false
				Gui:init()
				Chargen:execute()
				self:disable()
			end
		end}
	elseif Startup.mode == "--import" then
		-- Import data files.
		if not clss.reload_started then
			clss.text.text = "Importing data files..."
			Reload:reload()
			clss.reload_wait_timer = Timer{delay = 1, func = function(self)
				if Reload.done then
					clss.text.text = "Data files imported."
					self:disable()
				end
			end}
		end
	elseif Startup.mode == "--join" then
		-- Join a game.
		Program:unload_world()
		if not Network:join{host = clss.host, port = clss.port} then
			clss.text.text = "Connecting to " .. clss.host .. ":" .. clss.port .. " failed!"
			clss.button_retry.visible = true
		end
	else
		-- Display help.
		clss.text.text = "Usage:\n" ..
			"  lipsofsuna --join <server> <port>\n" ..
			"  lipsofsuna --host localhost <port>\n" ..
			"  lipsofsuna --import\n"
	end
end

Startup:init()
Startup:execute()

Eventhandler{type = "tick", func = function(self, args)
	if Startup.joined and not Network.connected then
		Gui:free()
		Startup.group.floating = true
		Startup.button_retry.text = "Reconnect"
		Startup.button_retry.visible = true
		Startup.text.text = "Lost connection to the server!"
	end
end}
