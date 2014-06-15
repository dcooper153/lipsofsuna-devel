--- TODO:doc
--
-- Lips of Suna is free software: you can redistribute it and/or modify
-- it under the terms of the GNU Lesser General Public License as
-- published by the Free Software Foundation, either version 3 of the
-- License, or (at your option) any later version.
--
-- @module ui.theme
-- @alias Theme

local Class = require("system/class")
local Graphics = require("system/graphics")
local Widgets = require("system/widgets")

--- TODO:doc
-- @type Theme
Theme = Class("Theme")
Theme.themes = {
	{
		scale = 1,
		spacing = 1,
		help_text_width = 175,
		help_title_height = 20,
		text_color_1 = {0,0,0,1},
		text_color_2 = {0.5,0.5,0.5,1},
		text_color_3 = {1,1,1,1},
		text_font_1 = "serif1",
		text_font_2 = "serif2",
		text_pad_1 = 3,
		text_height_1 = 14,
		text_height_2 = 20,
		height_widget_1 = 24,
		width_icon_1 = 30,
		width_label_1 = 150,
		width_slider_button_1 = 14,
		width_widget_1 = 300
	},
	{
		scale = 1.5,
		spacing = 2,
		help_text_width = 235,
		help_title_height = 26,
		text_color_1 = {0,0,0,1},
		text_color_2 = {0.5,0.5,0.5,1},
		text_color_3 = {1,1,1,1},
		text_font_1 = "serif2",
		text_font_2 = "serif3",
		text_pad_1 = 3,
		text_height_1 = 20,
		text_height_2 = 26,
		height_widget_1 = 32,
		width_icon_1 = 40,
		width_label_1 = 225,
		width_slider_button_1 = 20,
		width_widget_1 = 450
	},
	{
		scale = 2,
		spacing = 3,
		help_text_width = 295,
		help_title_height = 32,
		text_color_1 = {0,0,0,1},
		text_color_2 = {0.5,0.5,0.5,1},
		text_color_3 = {1,1,1,1},
		text_font_1 = "serif3",
		text_font_2 = "serif4",
		text_pad_1 = 3,
		text_height_1 = 26,
		text_height_2 = 32,
		height_widget_1 = 38,
		width_icon_1 = 50,
		width_label_1 = 300,
		width_slider_button_1 = 26,
		width_widget_1 = 600
	}}

Theme.init = function(self)
	Widgets:add_font_style("serif1", "serif", 14)
	Widgets:add_font_style("serif2", "serif", 20)
	Widgets:add_font_style("serif3", "serif", 26)
	Widgets:add_font_style("serif4", "serif", 32)

	Widgets:add_font_style("default", "serif", 14)
	Widgets:add_font_style("bigger", "serif", 18)
	Widgets:add_font_style("medium", "serif", 24)
	Widgets:add_font_style("mainmenu", "serif", 48)
	Widgets:add_font_style("tiny", "serif", 9)
end

Theme.draw_base = function(self, widget, x, y, w, h, focused)
	widget:canvas_image{
		dest_position = {x,y},
		dest_size = {w,h},
		source_image = "widgets3",
		source_position = focused and {0,40} or {0,0},
		source_tiling = {-1,600,-1,-1,40,-1}}
end

Theme.draw_button = function(self, widget, text, x, y, w, h, focused, pressed)
	local position
	if pressed then
		position = focused and {0,160} or {0,200}
	else
		position = focused and {0,200} or {0,160}
	end
	-- Add the background.
	widget:canvas_image{
		dest_position = {x,y},
		dest_size = {w,h},
		source_image = "widgets3",
		source_position = position,
		source_tiling = {-1,600,-1,-1,40,-1}}
	-- Add the text.
	if text then
		widget:canvas_text{
			dest_position = {x+5,y+5+(focus and 5 or 3)},
			dest_size = {w-10,h-10},
			text = text,
			text_alignment = {0.5,0.5},
			text_color = self.text_color_1,
			text_font = self.text_font_1}
	end
end

Theme.draw_entry = function(self, widget, x, y, w, h, focused, editing)
	local position
	if editing then
		position = {0,120}
	else
		position = {0,80}
	end
	-- Add the background.
	widget:canvas_image{
		dest_position = {x,y},
		dest_size = {w,h},
		source_image = "widgets3",
		source_position = position,
		source_tiling = {-1,600,-1,-1,40,-1}}
end

Theme.draw_icon_scaled = function(self, widget, name, x, y, w, h, color)
	local icon = Main.specs:find_by_name("IconSpec", name)
	if icon then
		widget:canvas_image{
			color = color,
			dest_position = {x,y},
			dest_size = {w,h},
			source_image = icon.image,
			source_position = icon.offset,
			source_tiling = {-1,icon.size[1],-1,-1,icon.size[2],-1}}
	end
end

local __text_displ = {Vector(-1.5,-1.5), Vector(1.5,-1.5), Vector(-1.5,1.5), Vector(1.5,1.5)}

Theme.draw_scene_text = function(self, widget, text, x, y, w, h, alpha, align)
	local d = 1.5
	local a1 = 0.4 * (alpha or 1)
	local a2 = (alpha or 1)
	for k,v in pairs(__text_displ) do
		widget:canvas_text{
			dest_position = {x+v.x,y+v.y},
			dest_size = {w,h},
			text = text,
			text_alignment = align or {0.5,1},
			text_color = {self.text_color_1[1], self.text_color_1[2], self.text_color_1[3], a1 * self.text_color_1[4]},
			text_font = self.text_font_1}
	end
	widget:canvas_text{
		dest_position = {x,y},
		dest_size = {w,h},
		text = text,
		text_alignment = align or {0.5,1},
		text_color =  {self.text_color_3[1], self.text_color_3[2], self.text_color_3[3], a2 * self.text_color_3[4]},
		text_font = self.text_font_1}
end

Theme.draw_slider = function(self, widget, value, x, y, w, h, focused, editing)
	local bw = self.width_slider_button_1
	local position1 = {0,240}
	local position2 = {0,280}
	-- Add the left button.
	widget:canvas_image{
		dest_position = {x,y},
		dest_size = {bw,h},
		source_image = "widgets3",
		source_position = {640,0},
		source_tiling = {-1,20,-1,-1,40,-1}}
	widget:canvas_text{
		dest_position = {x,y+self.text_pad_1},
		dest_size = {bw,h},
		text = "<",
		text_alignment = {0.5,0.5},
		text_color = self.text_color_1,
		text_font = self.text_font_1}
	-- Add the right button.
	widget:canvas_image{
		dest_position = {x+w-bw,y},
		dest_size = {bw,h},
		source_image = "widgets3",
		source_position = {680,0},
		source_tiling = {-1,20,-1,-1,40,-1}}
	widget:canvas_text{
		dest_position = {x+w-bw,y+self.text_pad_1},
		dest_size = {bw,h},
		text = ">",
		text_alignment = {0.5,0.5},
		text_color = self.text_color_1,
		text_font = self.text_font_1}
	-- Add the background.
	widget:canvas_image{
		dest_position = {x+bw,y},
		dest_size = {w-bw*2,h},
		source_image = "widgets3",
		source_position = position1,
		source_tiling = {-1,600,-1,-1,40,-1}}
	-- Add the highlighted area.
	if value > 0 then
		widget:canvas_image{
			dest_clip = {0,0,value*(w-bw*2),h},
			dest_position = {x+bw,y},
			dest_size = {value*(w-bw*2),h},
			source_image = "widgets3",
			source_position = position2,
			source_tiling = {-1,600,-1,-1,40,-1}}
	end
end

Theme.measure_button = function(self, w, text)
	local w1,h1 = Graphics:measure_text(self.text_font_1, text, w)
	return w1 or 0,h1 or 0
end

Theme.set_theme = function(self, index)
	if self.current_theme == index then return end
	self.current_theme = index
	for k,v in pairs(self.themes[index]) do
		Theme[k] = v
	end
	return true
end

return Theme


