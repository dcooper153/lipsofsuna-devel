if not Settings then return end
if Settings.server then return end

require "common/skills"
require "common/unlocks"

require "editor/editor"
File:require_directory("client/widgets")
require "client/quickslots"
