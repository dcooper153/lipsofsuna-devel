Ui:add_state{
	state = "host",
	root = "mainmenu",
	label = "Host",
	grab = function()
		return false
	end,
	background = function()
		return Widgets.Background{fullscreen = true, image = "mainmenu1"}
	end}

Ui:add_widget{
	state = "host",
	widget = function() return Widgets.Uiconfigoption("host_savefile") end}

Ui:add_widget{
	state = "host",
	widget = function() return Widgets.Uiconfigoption("host_account") end}

Ui:add_widget{
	state = "host",
	widget = function() return Widgets.Uientry("Password", Client.options.host_password,
		function(w) Client.options.host_password = w.value end)
	end}

Ui:add_widget{
	state = "host",
	widget = function() return Widgets.Uiconfigoption("host_admin") end}

Ui:add_widget{
	state = "host",
	widget = function() return Widgets.Uitoggle("Restart", function(w)
			Client.options.host_restart = w.value
		end)
	end}

Ui:add_widget{
	state = "host",
	widget = function()
		return Widgets.Uibutton("Start game", function()
			Settings.address = "localhost"
			Settings.file = Client.options.host_savefile
			Settings.admin = Client.options.host_admin
			Settings.generate = Client.options.host_restart
			Settings.host = true
			Settings.port = 10101
			Settings.account = Client.options.host_account
			Settings.password = Client.options.host_password
			Client:host_game()
		end)
	end}
