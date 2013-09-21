local File = require("system/file")

Program:add_path(Mod.path)
File:require_directory(Mod.path, "init")
