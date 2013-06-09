require "system/bitwise"
require "system/config-file"
require "system/core"
require "system/coroutine"
require "system/database"
require "system/eventhandler"
require "system/image"
require "system/file"
require "system/network"
require "system/math"
require "system/noise"
require "system/object"
require "system/physics"
require "system/thread"
require "system/time"
require "system/timer"

-- Load main.
Main = require("core/main/init")
Main:load()
Main:main()
