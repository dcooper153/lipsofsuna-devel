local class_call = function(self, ...)
	return self:new(...)
end

local class_tostring = function(self)
	return "class:" .. self.class_name
end

local data_tostring = function(self)
	return "data:" .. self.class_name
end

local Class = setmetatable({class_name = "Class"}, {
	__call = class_call,
	__tostring = class_tostring})

Class.new = function(clss, name, base)
	if clss == Class then
		return setmetatable({super = base or Class, class_name = name}, {
			__index = base or Class,
			__call = class_call,
			__tostring = class_tostring})
	else
		local self = {class = clss, __index = clss, __tostring = data_tostring}
		return setmetatable(self, self)
	end
end

return Class
