local Class = require("system/class")
local Quaternion = require("system/math/quaternion")
local Vector = require("system/math/vector")

--- Graphics functions.
-- @type Graphics
local Graphics = Class("Graphics")

--- Takes a screenshot.
-- @param clss Graphics class.
-- @return Screenshot filename.
Graphics.capture_screen = function(clss)
	return Los.program_screenshot()
end

--- Creates the main window.
-- @param clss Graphics class.
-- @param width Window width.
-- @param height Window height.
-- @param full True for fullscreen.
-- @param vsync True for vertical sync.
-- @param msaa Multisampling level.
-- @param grab True for pointer grab.
Graphics.create_window = function(clss, width, height, full, vsync, msaa, grab)
	__initial_videomode = {width, height, full, vsync, msaa}
	__initial_pointer_grab = grab and true or false
	if not Los.program_load_extension("graphics") then
		error("loading extension `graphics' failed")
	end
	__initial_pointer_grab = nil
	__initial_videomode = nil
end

--- Layouts a string.
-- @param clss Graphics class.
-- @param font Font name.
-- @param text Text to layout.
-- @param width Width limit.
-- @return Number list containing (x,y,width) for each glyph.
Graphics.layout_text = function(clss, font, text, width)
	return Los.program_layout_text(font, text, width)
end

--- Loads the graphics resources.
-- @param clss Graphics class.
Graphics.load_graphics = function(clss)
	return Los.program_load_graphics()
end

--- Measures a string.
-- @param clss Graphics class.
-- @param font Font name.
-- @param text Text to measure.
-- @param width Width limit, or nil.
-- @return Width, height.
Graphics.measure_text = function(clss, font, text, width)
	return Los.program_measure_text(font, text, width)
end

--- Renders the scene.
-- @param clss Graphics class.
Graphics.render_scene = function(clss)
	return Los.program_render()
end

--- Updates the scene.
-- @param clss Graphics class.
-- @param secs Seconds since the last update.
Graphics.update_scene = function(clss, secs)
	return Los.program_render_update(secs)
end

--- Updates the animations of the scene.
-- @param clss Graphics class.
-- @param secs Seconds since the last update.
Graphics.update_scene_animations = function(clss, secs)
	return Los.program_render_update_animations(secs)
end

--- Gets the far plane distance of the camera.
-- @param self Graphics class.
-- @return Far plane distance.
Graphics.get_camera_far = function(self)
	return self.__camera_far or 50
end

--- Sets the far plane distance of the camera.
-- @param self Graphics class.
-- @param v Far plane distance.
Graphics.set_camera_far = function(self, v)
	self.__camera_far = v
	Los.render_set_camera_far(v)
end

--- Gets the near plane distance of the camera.
-- @param self Graphics class.
-- @return Near plane distance.
Graphics.get_camera_near = function(self)
	return self.__camera_near or 0.1
end

--- Sets the near plane distance of the camera.
-- @param self Graphics class.
-- @param v Near plane distance.
Graphics.set_camera_near = function(self, v)
	self.__camera_near = v
	Los.render_set_camera_near(v)
end

--- Gets the position of the camera.
-- @param self Graphics class.
-- @return Vector.
Graphics.get_camera_position = function(self)
	return self.__camera_position or Vector()
end

--- Sets the position of the camera.
-- @param self Graphics class.
-- @param v Vector.
Graphics.set_camera_position = function(self, v)
	local rot = self:get_camera_rotation()
	self.__camera_position = v
	Los.render_set_camera_transform(v.handle, rot.handle)
end

--- Gets the rotation of the camera.
-- @param self Graphics class.
-- @return Quaternion.
Graphics.get_camera_rotation = function(self)
	return self.__camera_rotation or Quaternion()
end

--- Sets the rotation of the camera.
-- @param self Graphics class.
-- @param v Quaternion.
Graphics.set_camera_rotation = function(self, v)
	local pos = self:get_camera_position()
	self.__camera_rotation = v
	Los.render_set_camera_transform(pos.handle, v.handle)
end

--- Gets the current video mode.
-- @param self Graphics class.
-- @return Video mode table.
Graphics.get_video_mode = function(self)
	return Los.program_get_video_mode()
end

--- Sets the current video mode.
-- @param self Graphics class.
-- @param width Window width in pixels.
-- @param height Window height in pixels.
-- @param fullscreen True for fullscreen.
-- @return True on success.
Graphics.set_video_mode = function(self, width, height, fullscreen)
	return Los.program_set_video_mode(width, height, fullscreen)
end

--- Gets the list of supported fullscreen video modes.
-- @param self Graphics class.
-- @return List of video mode tables.
Graphics.get_video_modes = function(self)
	return Los.program_get_video_modes()
end

--- Gets the title of the main window.
-- @param self Graphics class.
-- @return String.
Graphics.get_window_title = function(self)
	return self.__window_title or ""
end

--- Sets the title of the main window.
-- @param self Graphics class.
-- @param v String.
Graphics.set_window_title = function(self, v)
	self.__window_title = v
	Los.program_set_title(v)
end

return Graphics
