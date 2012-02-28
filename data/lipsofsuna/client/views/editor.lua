require "editor/editor"

Views.Editor = Class(Widget)
Views.Editor.class_name = "Views.Editor"

--- Creates a new editor view.
-- @param clss Editor view class.
-- @return Editor view.
Views.Editor.new = function(clss)
	local self = Widget.new(clss, {cols = 1, rows = 1, fullscreen = true})
	self.timer = Timer{enabled = false, func = function(timer, secs) self:update(secs) end}
	self.editor = Editor()
	Editor.inst = self.editor
	return self
end

--- Returns back to the login screen.
-- @param self Editor view.
Views.Editor.back = function(self)
end

Views.Editor.close = function(self)
	for k,v in pairs(Object.objects) do v:detach() end
	Client.sectors:unload_world()
	self.timer.enabled = false
	self.editor.light.enabled = false
	Client.sectors.unload_time = 10
end

--- Executes the editor view.
-- @param self Editor view.
-- @param from Name of the previous mode.
Views.Editor.enter = function(self, from)
	self.editor:load(Settings.pattern or "lips1")
	self.timer.enabled = true
	self.editor.light.enabled = true
	Client.sectors.unload_time = nil
	Sound:switch_music_track("game")
end

Views.Editor.update = function(self, secs)
end
