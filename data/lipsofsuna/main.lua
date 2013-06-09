require "system/math" -- FIXME: Global Vector and Quaternion still used in many places.

-- Main is global on purpose.
Main = require("main/main")
Main:load()
Main:main()
