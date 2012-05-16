require(Mod.path .. "widget")

Widgets.Uibinding = Class(Widgets.Uiwidget)
Widgets.Uibinding.class_name = "Widgets.Uibinding"

Widgets.Uibinding.new = function(clss, action)
	local self = Widgets.Uiwidget.new(clss)
	self.action = action
	self.hint = "$A: Change the control\n$$B\n$$U\n$$D"
	self.name = string.gsub(action.name, "_", " ")
	self.name = string.gsub(self.name, "(.)(.*)", function(a,b) return string.upper(a) .. b end)
	return self
end

Widgets.Uibinding.apply = function(self)
	Effect:play_global("uitoggle1")
	self.input_mode = true
	self.input_key = "key1"
	if self.action.mode == "analog" then
		self.hint = "Input the first control"
	else
		self.hint = "Input the control"
	end
end

Widgets.Uibinding.handle_event = function(self, args)
	if not self.input_mode then
		return Widgets.Uiwidget.handle_event(self, args)
	end
	local accept
	local a = self.action
	local k = self.input_key
	-- Handle the event.
	if args.type == "keypress" then
		a[k] = args.code
		accept = true
	elseif args.type == "mousepress" then
		a[k] = string.format("mouse%d", args.button)
		accept = true
	elseif args.type == "mousescroll" then
		a.key1 = "mousez"
		a.key2 = nil
		accept = true
		self.editing_binding_key = "key2"
	elseif args.type == "mousemotion" and a.mode == "analog" then
		local v = Vector(args.dx, args.dy)
		local n = self.editing_binding_motion + v
		if math.abs(n.x) > 10 then
			a.key1 = "mousex"
			a.key2 = nil
			accept = true
			self.editing_binding_key = "key2"
		elseif math.abs(n.y) > 10 then
			a.key1 = "mousey"
			a.key2 = nil
			accept = true
			self.editing_binding_key = "key2"
		else
			self.editing_binding_motion = n
		end
	end
	-- Finish the input grab.
	if accept then
		Effect:play_global("uitoggle1")
		if k == "key1" and a.mode == "analog" then
			self.input_key = "key2"
			self.hint = "Input the second control"
		else
			self.input_mode = nil
			self.input_key = nil
			self.hint = "$A: Change the control\n$$B\n$$U\n$$D"
		end
		Ui:repaint_state()
	end
end

Widgets.Uibinding.rebuild_canvas = function(self)
	local w = self.size.x
	local h = self.size.y
	local keyname = function(k) return Keycode[k] or (k and tostring(k) or "----") end
	-- Add the base.
	Widgets.Uiwidget.rebuild_canvas(self)
	-- Add the action name.
	self:canvas_text{
		dest_position = {10,0},
		dest_size = {w-10,h},
		text = self.name,
		text_alignment = {0,0.5},
		text_color = Theme.text_color_1,
		text_font = Theme.text_font_1}
	-- Add the first key.
	local x1 = Theme.width_label_1
	local key1 = self.action.key1
	local name1 = self.input_key ~= "key1" and keyname(key1) or "????"
	self:canvas_text{
		dest_position = {Theme.width_label_1,0},
		dest_size = {w-Theme.width_label_1,h},
		text = name1,
		text_alignment = {0,0.5},
		text_color = Theme.text_color_1,
		text_font = Theme.text_font_1}
	-- Add the second key.
	if self.action.mode == "analog" then
		local x2 = x1 + (self.size.x - x1) / 2
		local key2 = self.action.key2
		local name2 = self.input_key ~= "key2" and keyname(key2) or "????"
		self:canvas_text{
			dest_position = {x2,0},
			dest_size = {w-x2,h},
			text = name2,
			text_alignment = {0,0.5},
			text_color = Theme.text_color_1,
			text_font = Theme.text_font_1}
	end
end
