Mod = Class()
Mod.class_name = "Mod"

Mod.load = function(self, name)
	self.name = name
	self.path = name .. "/"
	require(name .. "/init")
	self.name = nil
	self.path = nil
end
