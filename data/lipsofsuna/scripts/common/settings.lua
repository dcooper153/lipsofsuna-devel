Settings = Class()
Settings.arguments = string.split(Program.args)
Settings.addr = "localhost"
Settings.port = 10101

--- Parses command line arguments.
-- @param clss Settings class.
Settings.parse_command_line = function(clss)
	local i = 1
	local a = clss.arguments
	local parse_pattern = function()
		local num = 0
		while i <= #a and string.sub(a[i], 1, 1) ~= "-" do
			if clss.pattern then
				clss.pattern = clss.region .. " " .. a[i]
			else
				clss.pattern = a[i]
			end
			i = i + 1
		end
		if not clss.pattern then clss.pattern = "spawnpoint1" end
		return num
	end
	local parse_addr_port = function()
		if i > #a or string.sub(a[i], 1, 1) == "-" then return 0 end
		Settings.address = a[i]
		if i > #a or string.sub(a[i + 1], 1, 1) == "-" then return 1 end
		Settings.port = tostring(a[i + 1])
		return 2
	end
	-- Parse arguments.
	while i <= #a do
		if a[i] == "--editor" then
			clss.editor = true
			i = i + 1
			i = i + parse_pattern()
		elseif a[i] == "--generate" then
			clss.generate = true
			i = i + 1
		elseif a[i] == "--help" or a[i] == "-h" then
			clss.help = true
			i = i + 1
		elseif a[i] == "--host" then
			clss.host = true
			clss.client = true
			i = i + 1
			i = i + parse_addr_port()
		elseif a[i] == "--join" then
			clss.join = true
			clss.client = true
			i = i + 1
			i = i + parse_addr_port()
		elseif a[i] == "--quit" then
			clss.quit = true
			i = i + 1
		elseif a[i] == "--server" then
			clss.server = true
			i = i + 1
			i = i + parse_addr_port()
		else
			clss.help = true
			break
		end
	end
	-- Host by default.
	if not clss.client and not clss.server and not clss.editor then
		clss.host = true
		clss.client = true
	end
	-- Check for validity.
	if clss.help then return end
	if clss.host and clss.editor then return end
	if clss.join and clss.editor then return end
	if clss.client and clss.server then return end
	if clss.client and clss.editor then return end
	if clss.editor and clss.server then return end
	if not clss.host and not clss.server and clss.generate then return end
	return true
end

--- Returns the usage string.
-- @param clss Settings class.
-- @return Usage string.
Settings.usage = function(clss)
	return [[Usage: lipsofsuna lipsofsuna [options]

Options:
  --editor <region>        Edit a map region.
  --generate               Generate a new map.
  --help                   Show this help message and exit.
  --host localhost <port>  Start a server and join it.
  --join <server> <port>   Join a remove server.
  --quit                   Quit immediately after startup.
  --server                 Run as a dedicated server.]]
end
