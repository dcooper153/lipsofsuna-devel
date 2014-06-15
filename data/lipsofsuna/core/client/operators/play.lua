local Class = require("system/class")
local Time = require("system/time")

Operators.play = Class("PlayOperator")
Operators.play.data = {}

--- Resets the play operator.</br>
--
-- Context: Any.
--
-- @param self Operator.
Operators.play.reset = function(self)
	self.data.join_time = Time:get_secs()
	Client.play_hooks:call()
end

--- Checks if the game was just started.</br>
--
-- Context: Any.
--
-- @param self Operator.
-- @return True if jus started, false if not.
Operators.play.is_startup_period = function(self)
	if not self.data.join_time then return true end
	return (Time:get_secs() - self.data.join_time) < 2
end
