
Ui:add_state{
	state = "mainmenu",
	root = "mainmenu",
	background = function()
		return Widgets.Background{fullscreen = true, image = "mainmenu1"}
	end,
	init = function()
		Sound:switch_music_track("menu")
	end}

Ui:add_widget{
	state = "mainmenu",
	widget = function() return Widgets.Uibutton("Play now", function()
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

Ui:add_widget{
	state = "mainmenu",
	widget = function() return Widgets.Uitransition("Host", "host") end}

Ui:add_widget{
	state = "mainmenu",
	widget = function() return Widgets.Uitransition("Join", "join") end}

Ui:add_widget{
	state = "mainmenu",
	widget = function() return Widgets.Uitransition("Controls", "controls") end}

Ui:add_widget{
	state = "mainmenu",
	widget = function() return Widgets.Uitransition("Options", "options") end}

Ui:add_widget{
	state = "mainmenu",
	widget = function() return Widgets.Uitransition("Editor", "editor") end}

Ui:add_widget{
	state = "mainmenu",
	widget = function() return Widgets.Uibutton("Quit", function() Program.quit = true end) end}
