if not Settings then return end
if Settings.server then return end

require(Mod.path .. "options")

-- Initialize options.
Options.inst = Options()
__initial_videomode = {Options.inst.window_width, Options.inst.window_height, Options.inst.fullscreen, Options.inst.vsync, Options.inst.multisamples}