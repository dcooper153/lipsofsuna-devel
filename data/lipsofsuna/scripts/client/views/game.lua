Views.Game = Class(Widget)
Views.Game.mode = "game"

--- Creates a new game view.
-- @param clss Game view class.
-- @return Game view.
Views.Game.new = function(clss)
	local self = Widget.new(clss)
	return self
end

Views.Game.back = function(self)
	Gui:set_mode("menu")
end

--- Enters the game view.
-- @param self Game view.
Views.Game.enter = function(self)
	Program.cursor_grabbed = true
end

--- Closes the game view.
-- @param self Game view.
Views.Game.close = function(self)
	Program.cursor_grabbed = false
end

Views.Game.inst = Views.Game()
