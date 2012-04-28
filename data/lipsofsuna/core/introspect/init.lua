require(Mod.path .. "introspect")

for k,v in pairs(File:scan_directory(Mod.path .. "types")) do
	if v ~= "init.lua" then
		require(Mod.path .. "types/" .. string.gsub(v, "([^.]*).*", "%1"))
	end
end
