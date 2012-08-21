local File = require("system/file")

for k,v in pairs(File:scan_directory(Mod.path)) do
	if v ~= "init.lua" then
		require(Mod.path .. string.gsub(v, "([^.]*).*", "%1"))
	end
end
