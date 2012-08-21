require "system/core"

if Program:get_args() == "-U" or Program:get_args() == "--unittest" then
	Program:launch_mod{name = "unittest", args = Program:get_args()}
else
	Program:launch_mod{name = "lipsofsuna", args = Program:get_args()}
end
Program:set_quit(true)
