local Lobby = require("system/lobby")
local UiBackground = require("ui/widgets/background")
local UiButton = require("ui/widgets/button")
local UiConfigOption = require("ui/widgets/config-option")
local UiEntry = require("ui/widgets/entry")
local UiTransition = require("ui/widgets/transition")

Ui:add_state{
	state = "join",
	root = "mainmenu",
	label = "Join",
	background = function()
		return UiBackground("mainmenu1")
	end}

Ui:add_widget{
	state = "join",
	widget = function() return UiTransition("Server list", "join/servers") end}

Ui:add_widget{
	state = "join",
	widget = function() return UiConfigOption("join_address") end}

Ui:add_widget{
	state = "join",
	widget = function() return UiConfigOption("join_port") end}

Ui:add_widget{
	state = "join",
	widget = function() return UiConfigOption("join_account") end}

Ui:add_widget{
	state = "join",
	widget = function()
		return UiEntry("Password", Client.options.join_password, function(w)
			Client.options.join_password = w.value
		end)
	end}

Ui:add_widget{
	state = "join",
	widget = function() return UiButton("Connect", function()
			Settings.account = Client.options.join_account
			Settings.password = Client.options.join_password
			Main.join:join_game(Client.options.join_address, Client.options.join_port)
		end)
	end}

------------------------------------------------------------------------------

Ui:add_state{
	state = "join/servers",
	root = "mainmenu",
	label = "Select server",
	background = function()
		return UiBackground("mainmenu1")
	end,
	init = function()
		-- Download the server list.
		Lobby:set_master(Client.options.master_server)
		local servers = Lobby:download_server_list() or {}
		-- Add the local server.
		table.insert(servers, 1, {ip = "localhost", port = 10101,
			name = "Localhost", desc = "Connect to a locally hosted server"})
		-- Create the widgets.
		local widgets = {}
		for k,v in pairs(servers) do
			table.insert(widgets, Widgets.Uiserverinfo(v))
		end
		return widgets
	end}
