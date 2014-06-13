local File = require("system/file")
local UiBackground = require("ui/widgets/background")
local UiButton = require("ui/widgets/button")
local UiEntry = require("ui/widgets/entry")

Ui:add_state{
	state = "host/load",
	root = "mainmenu",
	label = "Host",
	background = function()
		return UiBackground("mainmenu1")
	end}

Ui:add_widget{
	state = "host/load",
	widget = function() return Widgets.Uiconfigoption("host_account") end}

Ui:add_widget{
	state = "host/load",
	widget = function() return UiEntry("Password", Client.options.host_password,
		function(w) Client.options.host_password = w.value end)
	end}

Ui:add_widget{
	state = "host/load",
	widget = function() return Widgets.Uiconfigoption("host_admin") end}

Ui:add_widget{
	state = "host/load",
	widget = function() return Widgets.Uitoggle("Restart", function(w)
			Client.options.host_restart = w.value
		end)
	end}

Ui:add_widget{
	state = "host/load",
	widget = function() return Widgets.Uilabel("Files:") end}

Ui:add_widget{
	state = "host/load",
	widget = function()
		local widgets = {}
		for k,v in ipairs(File:scan_save_directory("")) do
			local file = string.match(v, "save([a-z0-9_]*).sqlite")
			if file then
				local name = Operators.single_player:decode_save_filename(file)
				table.insert(widgets, UiButton(name, function()
					Settings.file = file
					Settings.admin = Client.options.host_admin
					Settings.generate = Client.options.host_restart
					Settings.host = "localhost:10101"
					Settings.join = nil
					Settings.account = Client.options.host_account
					Settings.password = Client.options.host_password
					Main.host:host_game()
				end))
			end
		end
		return widgets
	end}
