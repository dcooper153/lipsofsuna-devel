
Ui:add_state{
	state = "join",
	root = "mainmenu",
	background = function()
		return Widgets.Background{fullscreen = true, image = "mainmenu1"}
	end}

Ui:add_widget{
	state = "join",
	widget = function() return Widgets.Uiconfigoption("join_account") end}

Ui:add_widget{
	state = "join",
	widget = function() return Widgets.Uientry("Password", Client.options.join_password,
		function(w) Client.options.join_password = w.value end)
	end}

Ui:add_widget{
	state = "join",
	widget = function() return Widgets.Uiconfigoption("join_address") end}

Ui:add_widget{
	state = "join",
	widget = function() return Widgets.Uiconfigoption("join_port") end}

Ui:add_widget{
	state = "join",
	widget = function() return Widgets.Uibutton("Connect", function()
			Settings.address = Client.options.join_address
			Settings.host = false
			Settings.port = Client.options.join_port
			Settings.account = Client.options.join_account
			Settings.password = Client.options.join_password
			Client:join_game()
		end)
	end}
