require "system/class"
require "system/math"

if not Los.program_load_extension("graphics") then
	error("loading extension `graphics' failed")
end

------------------------------------------------------------------------------

--- Takes a screenshot.
-- @param clss Program class.
-- @return Screenshot filename.
Program.capture_screen = function(clss)
	return Los.program_screenshot()
end

--- Measures a string.
-- @param clss Program class.
-- @param ... Arguments.<ul>
--   <li>1,font: Font name.</li>
--   <li>2,text: Text to measure.</li></ul>
--   <li>3,width: Width limit, or nil.</li></ul>
-- @return Width, height.
Program.measure_text = function(clss, ...)
	local a,b,c = ...
	if type(a) == "table" then
		return Los.program_measure_text(a[1] or a.font, a[2] or a.text, a[3] or a.width)
	else
		return Los.program_measure_text(a, b, c)
	end
end

--- Sets the current video mode.
-- @param clss Program class.
-- @param ... Arguments.<ul>
--   <li>1,width: Window width in pixels.</li>
--   <li>2,height: Window height in pixels.</li>
--   <li>3,fullscreen: True for fullscreen.</li></ul>
-- @return True on success.
Program.set_video_mode = function(clss, ...)
	return Los.program_set_video_mode(...)
end

--- The far plane distance of the camera.
-- @name Program.camera_far
-- @class table

--- The near plane distance of the camera.
-- @name Program.camera_near
-- @class table

--- The position of the camera.
-- @name Program.camera_position
-- @class table

--- The rotation of the camera.
-- @name Program.camera_rotation
-- @class table

--- Short term average frames per second.
-- @name Program.fps
-- @class table

--- Gets the version of the OpenGL renderer used.
-- @name Program.opengl_version
-- @class table

--- Current video mode (read-only).
-- @name Program.video_mode
-- @class table

--- List of supported fullscreen modes (read-only).
-- @name Program.video_modes
-- @class table

--- Main window title.
-- @name Program.window_title
-- @class table

Program:add_class_getters{
	camera_far = function(s) return rawget(s, "__camera_far") or 50 end,
	camera_near = function(s) return rawget(s, "__camera_near") or 0.1 end,
	camera_position = function(s) return rawget(s, "__camera_position") or Vector() end,
	camera_rotation = function(s) return rawget(s, "__camera_rotation") or Quaternion() end,
	fps = function(s) return Los.program_get_fps() end,
	opengl_version = function(s) return Los.program_get_opengl_version() end,
	video_mode = function(s) return Los.program_get_video_mode() end,
	video_modes = function(s) return Los.program_get_video_modes() end,
	window_title = function(s) return rawget(s, "__window_title") or "" end}

Program:add_class_setters{
	camera_far = function(s, v)
		rawset(s, "__camera_far", v)
		Los.render_set_camera_far(v)
	end,
	camera_near = function(s, v)
		rawset(s, "__camera_near", v)
		Los.render_set_camera_near(v)
	end,
	camera_position = function(s, v)
		rawset(s, "__camera_position", v)
		Los.render_set_camera_transform(v.handle, s.camera_rotation.handle)
	end,
	camera_rotation = function(s, v)
		rawset(s, "__camera_rotation", v)
		Los.render_set_camera_transform(s.camera_position.handle, v.handle)
	end,
	window_title = function(s, v)
		rawset(s, "__window_title", v)
		Los.program_set_title(v)
	end}
