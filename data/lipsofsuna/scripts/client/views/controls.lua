Views.Controls = Class(Widget)

Views.Controls.new = function(clss)
	local self = Widget.new(clss, {cols = 1, rows = 3, spacings = {0, 0}})
	self.list = Widgets.List()
	self.list.pressed = function(view, row) self:show(row) end
	self.list:set_request{width = 100, height = 100}
	self.title = Widgets.Frame{style = "title", text = "Controls"}
	self:set_expand{col = 1, row = 2}
	self:set_child{col = 1, row = 1, widget = self.title}
	self:set_child{col = 1, row = 2, widget = self.list}
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
