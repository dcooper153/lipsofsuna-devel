Door = Class(Object)

--- Creates a door.
-- @param clss Door class.
-- @param args Arguments.
-- @return New door.
Door.new = function(clss, args)
	local self = Object.new(clss, args)
	self.realized = true
	self:insert_hinge_constraint{position = Vector(-1,0,0), axis = Vector(0,1,0)}
	return self
end
