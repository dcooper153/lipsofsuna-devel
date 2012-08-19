Ui:add_state{
	state = "host/new",
	root = "mainmenu",
	label = "Host",
	grab = function()
		return false
	end,
	background = function()
		return Widgets.Uibackground("mainmenu1")
	end}

Ui:add_widget{
	state = "host/new",
	widget = function()
		local widget = Widgets.Uientry("Save name", function(w)
			Operators.single_player:set_save_name(w.value) end)
		widget.value = Operators.single_player:generate_new_save_name()
		return widget
	end}

Ui:add_widget{
	state = "host/new",
	widget = function() return Widgets.Uiconfigoption("host_account") end}

Ui:add_widget{
	state = "host/new",
	widget = function() return Widgets.Uientry("Password", Client.options.host_password,
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
		return Widgets.Uibutton("Start game", function()
			-- Prevent overwriting.
			local name = Operators.single_player:get_save_name()
			if #name == 0 then return end
			if Operators.single_player:does_save_name_exist(name) then return end
			-- Start the game.
			Settings.address = "localhost"
			Settings.file = Operators.single_player:get_save_filename()--Client.options.host_savefile
			Settings.admin = Client.options.host_admin
			Settings.generate = Client.options.host_restart
			Settings.host = true
			Settings.port = 10101
			Settings.account = Client.options.host_account
			Settings.password = Client.options.host_password
			Client:host_game()
		end)
	end}
