if not Settings then return end

local File = require("system/file")
require(Mod.path .. "generator")
require(Mod.path .. "region")

for k,v in pairs(File:scan_directory(Mod.path .. "sectors")) do
	require(Mod.path .. "sectors/" .. string.gsub(v, "([^.]*).*", "%1"))
end
