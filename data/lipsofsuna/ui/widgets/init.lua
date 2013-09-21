if not Settings then return end
if Settings.server then return end

local File = require("system/file")

File:require_directory(Mod.path, "init")
