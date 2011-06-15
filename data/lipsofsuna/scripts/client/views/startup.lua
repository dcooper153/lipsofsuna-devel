Startup = Class()

--- Initializes the startup screen.
-- @param clss Startup class.
Startup.init = function(clss)
	clss.group = Widgets.Background{cols = 3, rows = 3, behind = true, fullscreen = true, image = "mainmenu1"}
	clss.group:set_expand{col = 1, row = 1}
	clss.group:set_expand{col = 3}
	clss.text = Widgets.Label()
	clss.button_back = Widgets.Label{font = "mainmenu", text = "Back", pressed = function() clss:back() end}
	clss.button_quit = Widgets.Label{font = "mainmenu", text = "Quit", pressed = function() Program:shutdown() end}
	clss.group2 = Widget{cols = 4, rows = 1, margins = {bottom=30}, spacings = {horz=40}}
	clss.group2:set_child{col = 2, row = 1, widget = clss.button_back}
	clss.group2:set_child{col = 3, row = 1, widget = clss.button_quit}
	clss.group2:set_expand{col = 1}
	clss.group2:set_expand{col = 4}
	clss.group:set_child{col = 2, row = 2, widget = clss.text}
	clss.group:set_child{col = 2, row = 3, widget = clss.group2}
end

--- Returns to the launcher.
-- @param clss Startup class.
Startup.back = function(clss)
	Program:launch_mod{name = "default"}
	Program.quit = true
end

--- Finishes the startup when connection has been established.
-- @param clss Startup class.
Startup.close = function(clss)
	clss.group.floating = false
	clss.connecting = nil
	clss.joined = true
end

--- Executes the startup command.
-- @param clss Startup class.
Startup.enter = function(clss)
	clss.group.floating = true
	clss:retry()
end

--- Retries hosting or joining.
-- @param clss Startup class.
Startup.retry = function(clss)
	if clss.host_wait_timer then
		clss.host_wait_timer:disable()
		clss.host_wait_timer = nil
	end
	if Settings.host then
		-- Host a game.
		local opts = string.format("--file %s --server %s %d", Settings.file, Settings.address, Settings.port)
		if Settings.generate then opts = opts .. " -g" end
		Program:unload_world()
		Client:host(opts)
		clss:set_state("Starting the server on port " .. Settings.port .. "...")
		clss.host_wait_timer = Timer{delay = 2, func = function(timer)
			if Network:join{host = "localhost", Settings.port} then
				clss:set_state("Connecting to the server...")
				clss.connecting = true
			else
				clss:set_state("Failed to start the server!")
				clss.connecting = nil
			end
			timer:disable()
		end}
	else
		-- Join a game.
		Program:unload_world()
		if Network:join{host = Settings.address, port = Settings.port} then
			clss:set_state("Connecting to " .. Settings.address .. ":" .. Settings.port .. "...")
			clss.connecting = true
		else
			clss:set_state("Failed to connect to " .. Settings.address .. ":" .. Settings.port .. "!")
			clss.connecting = nil
		end
	end
end

--- Shows or updates the waiting screen.
-- @param clss Startup class.
-- @param text Waiting text.
Startup.set_state = function(clss, text)
	clss.text.text = text
end

Startup:init()

Eventhandler{type = "tick", func = function(self, args)
	if Startup.joined and not Network.connected then
		Gui:set_mode("startup")
		Startup:set_state("Lost connection to the server!")
	end
end}

Protocol:add_handler{type = "AUTHENTICATE_REJECT", func = function(event)
	local ok,s = event.packet:read("string")
	if not ok then return end
	Gui:set_mode("startup")
	Startup:set_state("Authentication failed: " .. s)
end}

Protocol:add_handler{type = "CLIENT_AUTHENTICATE", func = function(event)
	Network:send{packet = Packet(packets.CLIENT_AUTHENTICATE, "string", Settings.account, "string", Settings.password)}
end}

Protocol:add_handler{type = "GENERATOR_STATUS", func = function(event)
	local ok,s,f = event.packet:read("string", "float")
	if ok then
		Gui:set_mode("startup")
		Startup:set_state("Map generator: " .. s .. " (" .. math.ceil(f * 100) .. "%)")
	end
end}
