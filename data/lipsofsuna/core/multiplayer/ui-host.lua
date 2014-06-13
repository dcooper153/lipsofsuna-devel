local UiBackground = require("ui/widgets/background")
local UiButton = require("ui/widgets/button")
local UiEntry = require("ui/widgets/entry")

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
	widget = function() return Widgets.Uiconfigoption("host_account") end}

Ui:add_widget{
	state = "host/new",
	widget = function() return UiEntry("Password", Client.options.host_password,
		function(w) Client.options.host_password = w.value end)
	end}

Ui:add_widget{
	state = "host/new",
	widget = function() return Widgets.Uiconfigoption("host_admin") end}

Ui:add_widget{
	state = "host/new",
	widget = function() return Widgets.Uitoggle("Restart", function(w)
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
			Settings.file = Operators.single_player:get_save_filename()--Client.options.host_savefile
			Settings.admin = Client.options.host_admin
			Settings.generate = Client.options.host_restart
			Settings.account = Client.options.host_account
			Settings.password = Client.options.host_password
			Main.host:host_game(Settings.file, Client.options.host_port)
		end)
	end}
