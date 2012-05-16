require(Mod.path .. "toggle")

Widgets.Uiradio = Class(Widgets.Uitoggle)
Widgets.Uiradio.class_name = "Widgets.Uiradio"

Widgets.Uiradio.new = function(clss, label, group, changed)
	local self = Widgets.Uitoggle.new(clss, label, changed)
	self.radio_group = group
	self.hint = "$A: Select\n$$B\n$$U\n$$D"
	return self
end

Widgets.Uiradio.apply = function(self)
	-- Reactivation does nothing.
	if self.value then return end
	-- Deactivate other toggles in the group.
	for k,v in pairs(Ui.widgets) do
		if v.radio_group == self.radio_group then
			if v ~= self and v.value then
				v.value = false
				v.need_repaint = true
			end
		end
	end
	-- Activate this radio toggle.
	self.value = true
	self.need_repaint = true
	self:changed()
	Effect:play_global("uitoggle1")
end

Widgets.Uiradio.changed = function(self)
end
