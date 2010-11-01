Options = Class()

Options.init = function(clss)
	-- Bloom toggle.
	clss.check_postproc = Widgets.Check{text = "Enable bloom"}
	-- Luminance adjustment.
	local scroll_luminance = Widgets.Progress{min = 0, max = 1, value = 0}
	scroll_luminance:set_request{width = 100}
	scroll_luminance.value = 0
	scroll_luminance.pressed = function(self)
		local v = self:get_value_at(Client.cursor_pos)
		self.value = v
		Bloom.luminance = 1 - v
		Bloom:compile()
	end
	-- Radius adjustment.
	local scroll_radius = Widgets.Progress{value = 40}
	scroll_radius:set_request{width = 100}
	scroll_radius.pressed = function(self)
		local v = self:get_value_at(Client.cursor_pos)
		self.value = v
		Bloom.radius = math.floor(v)
		Bloom:compile()
	end
	-- Exposure adjustment.
	local scroll_exposure = Widgets.Progress{min = 1, max = 10, value = 1.5}
	scroll_exposure:set_request{width = 100}
	scroll_exposure.pressed = function(self)
		local v = self:get_value_at(Client.cursor_pos)
		self.value = v
		Bloom.exposure = v
		Bloom:compile()
	end
	-- Packing.
	local bloom_group = Widget{cols = 2}
	bloom_group:append_row(clss.check_postproc)
	bloom_group:append_row(Widgets.Label{text = "Radius"}, scroll_radius)
	bloom_group:append_row(Widgets.Label{text = "Exposure"}, scroll_exposure)
	bloom_group:append_row(Widgets.Label{text = "Influence"}, scroll_luminance)
	clss.group = Widget{cols = 2}
	clss.group:append_row(Widgets.Label{text = "Bloom"})
	clss.group:append_row(bloom_group)
end

Options.toggle = function(clss)
	Gui.menus:open{widget = clss.group}
end

Options:init()
