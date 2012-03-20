require(Mod.path .. "widget")

Widgets.Uiserverinfo = Class(Widgets.Uiwidget)
Widgets.Uiserverinfo.class_name = "Widgets.Uiserverinfo"

Widgets.Uiserverinfo.new = function(clss, args)
	local self = Widgets.Uiwidget.new(clss)
	for k,v in pairs(args) do self[k] = v end
	self.hint = "$A: Select\n$$B\n$$U\n$$D"
	return self
end

Widgets.Uiserverinfo.apply = function(self)
	Client.options.join_address = self.ip
	Client.options.join_port = self.port
	Ui:pop_state()
end

Widgets.Uiserverinfo.rebuild_size = function(self)
	-- Get the base size.
	local size = Widgets.Uiwidget.rebuild_size(self)
	-- Resize to fit the label.
	if self.desc then
		local w,h = Program:measure_text("default", self.desc, 290)
		if h then size.y = math.max(size.y, h + 30) end
	end
	return size
end

Widgets.Uiserverinfo.rebuild_canvas = function(self)
	local a = 1
	local w = self.size.x
	local h = self.size.y
	-- Add the base.
	Widgets.Uiwidget.rebuild_canvas(self)
	-- Add the background.
	self:canvas_image{
		dest_position = {5,3},
		dest_size = {self.size.x-10,self.size.y-6},
		source_image = "widgets1",
		source_position = self.focused and {350,415} or {350,375},
		source_tiling = {12,64,12,11,14,13}}
	-- Add the name.
	if self.name then
		self:canvas_text{
			dest_position = {10,5},
			dest_size = {w-10,h},
			text = self.name,
			text_alignment = {0,0},
			text_color = self.focused and {1,1,0,1} or {a,a,a,1},
			text_font = "bigger"}
	end
	-- Add the description.
	if self.desc then
		self:canvas_text{
			dest_position = {10,23},
			dest_size = {w-10,h},
			text = self.desc,
			text_alignment = {0,0},
			text_color = self.focused and {1,1,0,1} or {a,a,a,1},
			text_font = "default"}
	end
	-- Add the player count.
	if self.players then
		self:canvas_text{
			dest_position = {0,8},
			dest_size = {w-10,h},
			text = tostring(self.players),
			text_alignment = {1,0},
			text_color = self.focused and {1,1,0,1} or {a,a,a,1},
			text_font = "default"}
	end
end
