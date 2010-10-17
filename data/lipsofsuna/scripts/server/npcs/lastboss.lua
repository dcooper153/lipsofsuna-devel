Lastboss = Class(Monster)

--- Creates the last boss.
-- @param clss Lastboss class.
-- @param args Arguments.
-- @param object Existing object or nil to create a new one.
-- @return Object.
Lastboss.new = function(clss, args, object)

	local self = object or Monster(args)
	self.class = Lastboss
	self.model = "aer-002"
	self.name = "last boss"

	return self
end

Lastboss.die = function()
	-- TODO: Send congratulation screen
	restart()
end
