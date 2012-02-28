Ui:add_widget{
	state = "admin/client-stats",
	widget = function(secs) return Widgets.Uilabel(Client.data.admin.client_stats) end}

Ui:add_widget{
	state = "admin/server-stats",
	widget = function(secs) return Widgets.Uilabel(Client.data.admin.server_stats) end}
