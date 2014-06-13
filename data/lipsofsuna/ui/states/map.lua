local UiMap = require("ui/widgets/map")

Ui:add_widget{
	state = "map",
	label = "Map",
	widget = function() return UiMap() end}
