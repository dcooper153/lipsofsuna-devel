local UiBackground = require("ui/widgets/background")
local UiButton = require("ui/widgets/button")
local UiConfigOption = require("ui/widgets/config-option")
local UiEntry = require("ui/widgets/entry")
local UiToggle = require("ui/widgets/toggle")

Ui:add_state{
	state = "host/new",
	root = "mainmenu",
	label = "Host",
	background = function()
		return UiBackground("mainmenu1")
	end}

Ui:add_widget{
	state = "host/new",
	widget = function()
		local widget = UiEntry("Save name", function(w)
			Operators.single_player:set_save_name(w.value) end)
		widget.value = Operators.single_player:generate_new_save_name()
		return widget
	end}

Ui:add_widget{
	state = "host/new",
	widget = function() return UiConfigOption("host_account") end}

Ui:add_widget{
	state = "host/new",
	widget = function() return UiEntry("Password", Client.options.host_password,
		function(w) Client.options.host_password = w.value end)
	end}

Ui:add_widget{
	state = "host/new",
	widget = function() return UiConfigOption("host_admin") end}

Ui:add_widget{
	state = "host/new",
	widget = function() return UiToggle("Restart", function(w)
			Client.options.host_restart = w.value
		end)
	end}

Ui:add_widget{
	state = "host/new",
	widget = function()
		return UiButton("Start game", function()
			-- Prevent overwriting.
			local name = Operators.single_player:get_save_name()
			if #name == 0 then return end
			if Operators.single_player:does_save_name_exist(name) then return end
			-- Start the game.
			Main.settings.file = Operators.single_player:get_save_filename()--Client.options.host_savefile
			Main.settings.admin = Client.options.host_admin
			Main.settings.generate = Client.options.host_restart
			Main.settings.account = Client.options.host_account
			Main.settings.password = Client.options.host_password
			Main.host:host_game(Main.settings.file, Client.options.host_port)
		end)
	end}
