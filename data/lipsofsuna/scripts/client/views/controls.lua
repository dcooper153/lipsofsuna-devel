require "client/widgets/binding"

Views.Controls = Class(Widget)
Views.Controls.mode = "controls"

Views.Controls.new = function(clss)
	local self = Widget.new(clss, {cols = 1, rows = 3, spacings = {0, 0}})
	self.list = Widgets.List{page_size = 15}
	self.list:set_request{width = 100, height = 300}
	self.button = Widgets.Button{text = "Save bindings", pressed = function() self:save() end}
	self.frame = Widgets.Frame{style = "default", cols = 1, rows = 2}
	self.frame:set_expand{col = 1, row = 1}
	self.frame:set_child(1, 1, self.list)
	self.frame:set_child(1, 2, self.button)
	self.title = Widgets.Frame{style = "title", text = "Controls"}
	self:set_expand{col = 1, row = 2}
	self:set_child(1, 1, self.title)
	self:set_child(1, 2, self.frame)
	-- Create the popup.
	self.popup_label = Widgets.Label()
	self.popup = Widgets.Frame{style = "tooltip", cols = 1, rows = 1}
	self.popup:set_child(1, 1, self.popup_label)
	-- Load the bindings.
	self.config = ConfigFile{name = "controls.cfg"}
	self:load()
	self:save()
	return self
end

Views.Controls.back = function(self)
	Gui:set_mode("menu")
end

--- Closes the contorls view.
-- @param self Controls view.
Views.Controls.close = function(self)
	-- Cancel the control grab.
	self.editing_binding = nil
	self.popup.visible = false
end

Views.Controls.edit = function(self, widget)
	self.popup_label.text = "Input the binding for `" .. widget.name .. "'."
	self.popup:popup()
	self.editing_binding = widget
	self.editing_binding_key = "key1"
	self.editing_binding_motion = Vector()
end

--- Hijacks all input events when editing a binding.<br/>
-- Hijacking is implemented in event.lua by hardcoding the event handlers
-- to call this function when Views.Controls.inst.editing_binding is true.
-- @param self Controls view.
-- @param args Event arguments.
Views.Controls.input = function(self, args)
	local accept
	local a = self.editing_binding.action
	local k = self.editing_binding_key
	-- Handle the event.
	if args.type == "keypress" then
		a[k] = args.code
		accept = true
	elseif args.type == "mousepress" then
		a[k] = string.format("mouse%d", args.button)
		accept = true
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
	-- Finish the grab.
	if accept then
		self:update()
		if k == "key1" and a.mode == "analog" then
			self.editing_binding_key = "key2"
		else
			self.editing_binding_key = nil
			self.editing_binding = nil
			self.popup.visible = false
		end
	end
end

Views.Controls.load = function(self)
	local translate = function(k)
		if not k or k == "none" then return end
		return tonumber(k) or Keysym[k] or k
	end
	-- Load a binding for each action.
	for k,v in pairs(Action.dict_name) do
		local keys = self.config:get(k)
		if keys then
			local key1,key2 = string.match(keys, "([a-zA-Z0-9]*)[ \t]*([a-zA-Z0-9]*)")
			key1 = translate(key1)
			key2 = translate(key2)
			if key1 then v.key1 = key1 end
			if key2 then v.key2 = key2 end
		end
	end
	-- Add a widget for each action.
	self.widgets = {}
	for k,v in pairs(Action.dict_name) do
		table.insert(self.widgets, Widgets.Binding{action = v, pressed = function(w) self:edit(w) end})
	end
	table.sort(self.widgets, function(a,b) return a.action.name < b.action.name end)
	for k,v in pairs(self.widgets) do
		self.list:append{widget = v}
	end
end

Views.Controls.save = function(self)
	local translate = function(k)
		if not k then return "none" end
		return Keycode[k] or tostring(k)
	end
	-- Save each action.
	for k,v in pairs(Action.dict_name) do
		local key1 = translate(v.key1)
		local key2 = translate(v.key2)
		self.config:set(k, key1 .. " " .. key2)
	end
	self.config:save()
end

Views.Controls.update = function(self)
	for k,v in pairs(self.widgets) do
		v:reshaped()
	end
end

------------------------------------------------------------------------------

Views.Controls.inst = Views.Controls()
