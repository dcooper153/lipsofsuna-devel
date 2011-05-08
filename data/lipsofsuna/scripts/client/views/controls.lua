require "client/widgets/binding"

Views.Controls = Class(Widget)
Views.Controls.mode = "controls"

Views.Controls.new = function(clss)
	local self = Widget.new(clss, {cols = 1, rows = 3, spacings = {0, 0}})
	self.list = Widgets.List{page_size = 15}
	--self.list.pressed = function(view, row) self:show(row) end
	self.list:set_request{width = 100, height = 300}
	self.frame = Widgets.Frame{style = "default", cols = 1, rows = 1}
	self.frame:set_expand{col = 1, row = 1}
	self.frame:set_child(1, 1, self.list)
	self.title = Widgets.Frame{style = "title", text = "Controls"}
	self:set_expand{col = 1, row = 2}
	self:set_child(1, 1, self.title)
	self:set_child(1, 2, self.frame)
	-- Load the bindings.
	self.config = ConfigFile{name = "controls.cfg"}
	self:load()
	self:save()
	return self
end

Views.Controls.back = function(self)
	Gui:set_mode("menu")
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
	local widgets = {}
	for k,v in pairs(Action.dict_name) do
		table.insert(widgets, Widgets.Binding{action = v})
	end
	table.sort(widgets, function(a,b) return a.action.name < b.action.name end)
	for k,v in pairs(widgets) do
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

------------------------------------------------------------------------------

Views.Controls.inst = Views.Controls()
