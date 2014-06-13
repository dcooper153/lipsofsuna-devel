local UiLabel = require("ui/widgets/label")

Ui:add_state{
	state = "admin/client-stats",
	label = "Client stats"}

Ui:add_widget{
	state = "admin/client-stats",
	widget = function(secs) return UiLabel(Operators.stats:get_client_stats_text()) end}

------------------------------------------------------------------------------

Ui:add_state{
	state = "admin/server-stats",
	label = "Server stats"}

Ui:add_widget{
	state = "admin/server-stats",
	label = "Server stats",
	widget = function(secs) return UiLabel(Client.data.admin.server_stats) end}

------------------------------------------------------------------------------

Ui:add_state{
	state = "admin/profiling",
	label = "Server stats"}

Ui:add_widget{
	state = "admin/profiling",
	label = "Profiling stats",
	widget = function(secs) return UiLabel(Main.timing:get_profiling_string()) end}
