Launcher = Class()

Launcher.new = function(clss, args)
	local self = Class.new(clss)
	self.mode = args.mode or "host"
	self.mod = args.mod or "lipsofsuna"
	self.server = args.server or "localhost"
	self.port = args.port or 10101
	self.file = args.file or 1
	self.account = args.account or "guest"
	return self
end

Launcher.execute = function(self, account, password)
	local args
	if self.mode == "host" then
		args = string.format("--host localhost %d --file %d", self.port, self.file)
	else
		args = string.format("--join %s %d", self.server, self.port)
	end
	if account and #account > 0 then
		args = string.format("%s --account %s", args, account)
	end
	if password and #password > 0 then
		args = string.format("%s --password %s", args, password)
	end
	Program:launch_mod{name = self.mod, args = args}
	Program.quit = true
end

Launcher.get_description = function(self)
	if self.mode == "host" then
		if self.mod ~= "lipsofsuna" then
			return string.format("Host #%d (%s)", self.file, self.mod)
		else
			return string.format("Host #%d", self.file)
		end
	else
		if self.mod ~= "lipsofsuna" then
			return string.format("Join %s (%s)", self.server, self.mod)
		else
			return string.format("Join %s", self.server)
		end
	end
end
