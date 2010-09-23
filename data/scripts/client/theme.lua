Widgets:add_font_style("default", [[
	file: serif.ttf;
	size: 14;
]])

Widgets:add_font_style("mainmenu", [[
	file: serif.ttf;
	size: 64;
]])

Widgets:add_widget_style("label", [[
	text-color: 1 1 1 1;
]])

Widgets:add_widget_style("mainmenu", [[
	file: mainmenu1.dds;
	width: 0 1024 0;
	height: 0 1024 0;
	fill-mode: resize;
]])

Widgets:add_widget_style("mainmenu-label", [[
	font: mainmenu;
	source: 0 0;
	width: 6 3 6;
	height: 6 3 6;
	padding: 2 2 2 2;
	text-color: 1 1 1 0.5;
]])

Widgets:add_widget_style("mainmenu-label:focus", [[
	font: mainmenu;
	source: 0 17;
	width: 6 3 6;
	height: 6 3 6;
	padding: 2 2 2 2;
	text-color: 1 1 0.5 0.5;
]])
