if not Settings then return end
if Settings.server then return end

local File = require("system/file")

require "editor/editor"
File:require_directory("client/widgets")
require "client/quickslots"
