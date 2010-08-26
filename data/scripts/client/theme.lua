Widgets:add_font_style("default", [[
	file: serif.ttf;
	size: 14;
]])

Widgets:add_font_style("mainmenu", [[
	file: serif.ttf;
	size: 64;
]])

Widgets:add_widget_style("button", [[
	file: widgets1.dds;
	source: 0 0;
	width: 6 3 6;
	height: 6 3 6;
	padding: 2 2 2 2;
	text-color: 1 1 1 1;
]])

Widgets:add_widget_style("button:focus", [[
	file: widgets1.dds;
	source: 0 17;
	width: 6 3 6;
	height: 6 3 6;
	padding: 2 2 2 2;
	text-color: 1 1 0 1;
]])

Widgets:add_widget_style("entry", [[
	file: widgets1.dds;
	source: 55 41;
	width: 3 26 3;
	height: 3 26 3;
	padding: 2 2 2 2;
	text-color: 0 0 0 1;
	selection-color: 0 1 0 1;
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

Widgets:add_widget_style("progress", [[
	file: widgets1.dds;
	source: 55 12;
	width: 6 13 6;
	height: 4 2 4;
	padding: 4 4 4 4;
	text-color: 1 1 1 1;
	selection-color: 0.5 0.5 0.5 1;
]])

Widgets:add_widget_style("tree", [[
	file: widgets1.dds;
	source: 55 41;
	width: 3 26 3;
	height: 3 26 3;
	padding: 2 2 2 2;
	hover-color: 1 0 0 1;
	text-color: 0 0 0 1;
	selection-color: 0 1 0 1;
]])
