Options = Class()

Options.init = function(clss)
	-- Luminance adjustment.
	local scroll_luminance =  Scroll{style = "health"}
	scroll_luminance:set_range{min = 0, max = 1}
	scroll_luminance:set_request{width = 100}
	scroll_luminance.value = 0
	scroll_luminance.pressed = function()
		Bloom.luminance = 1 - scroll_luminance.value
		Bloom:compile()
	end
	-- Radius adjustment.
	local scroll_radius =  Scroll{style = "health"}
	scroll_radius:set_range{min = 1, max = 64}
	scroll_radius:set_request{width = 100}
	scroll_radius.value = 40
	scroll_radius.pressed = function()
		Bloom.radius = math.floor(scroll_radius.value)
		Bloom:compile()
	end
	-- Exposure adjustment.
	local scroll_exposure =  Scroll{style = "health"}
	scroll_exposure:set_range{min = 1, max = 10}
	scroll_exposure:set_request{width = 100}
	scroll_exposure.value = 1.5
	scroll_exposure.pressed = function()
		Bloom.exposure = scroll_exposure.value
		Bloom:compile()
	end
	-- Packing.
	local bloom_group = Group{cols = 2}
	bloom_group:append_row(Button{text = "Radius", style = "label"}, scroll_radius)
	bloom_group:append_row(Button{text = "Exposure", style = "label"}, scroll_exposure)
	bloom_group:append_row(Button{text = "Influence", style = "label"}, scroll_luminance)
	clss.group = Group{cols = 2, style = "window"}
	clss.group:append_row(Button{text = "Bloom", style = "inventory-label"})
	clss.group:append_row(bloom_group)
end

Options.toggle = function(clss)
	Gui.menus:open{widget = clss.group}
end

Options:init()
