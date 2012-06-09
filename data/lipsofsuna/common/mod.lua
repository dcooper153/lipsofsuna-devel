Mod = Class()
Mod.class_name = "Mod"

Mod.load = function(self, name)
	local prev_name = self.name
	local prev_path = self.path
	self.name = name
	self.path = name .. "/"
	require(name .. "/init")
	self.name = prev_name
	self.path = prev_path
end

Mod.load_optional = function(self, name)
	local prev_name = self.name
	local prev_path = self.path
	self.name = name
	self.path = name .. "/"
	pcall(require, name .. "/init")
	self.name = prev_name
	self.path = prev_path
end
