#! /usr/bin/lua

require "lfs"

------------------------------------------------------------------------------

local class_call = function(self, ...)
	return self:new(...)
end

local class_tostring = function(self)
	return "class:" .. self.class_name
end

local data_tostring = function(self)
	return "data:" .. self.class_name
end

local Class = setmetatable({class_name = "Class"}, {
	__call = class_call,
	__tostring = class_tostring})

Class.new = function(clss, name, base)
	if clss == Class then
		return setmetatable({super = base or Class, class_name = name}, {
			__index = base or Class,
			__call = class_call,
			__tostring = class_tostring})
	else
		local self = {class = clss, __index = clss, __tostring = data_tostring}
		return setmetatable(self, self)
	end
end

------------------------------------------------------------------------------

local Utils = Class("Utils")

Utils.get_file_type = function(self, name)
	if string.match(name, ".*[.]c$") then return "C" end
	if string.match(name, ".*[.]cpp$") then return "C++" end
	if string.match(name, ".*[.]cs$") then return "C#" end
	if string.match(name, ".*[.]h$") then return "C" end
	if string.match(name, ".*[.]hpp$") then return "C++" end
	if string.match(name, ".*[.]js$") then return "JavaScript" end
	if string.match(name, ".*[.]json$") then return "JSON" end
	if string.match(name, ".*[.]lua$") then return "Lua" end
	if string.match(name, ".*[.]php$") then return "PHP" end
	if string.match(name, ".*[.]py$") then return "Python" end
	if string.match(name, ".*[.]compositor$") then return "OGRE" end
	if string.match(name, ".*[.]material$") then return "OGRE" end
	if string.match(name, ".*[.]particle$") then return "OGRE" end
	if string.match(name, ".*[.]program$") then return "OGRE" end
	if string.match(name, ".*[.]template$") then return "OGRE" end
	if string.match(name, ".*[.]frag$") then return "GLSL" end
	if string.match(name, ".*[.]vert$") then return "GLSL" end
end

Utils.find_files = function(self, path)
	return coroutine.wrap(function()
		local recurse
		recurse = function(path)
			for name in lfs.dir(path) do
				if not string.match(name, "^[.]") then
					local file = path .. name
					local attr = lfs.attributes(file)
					if attr and attr.mode == "directory" then
						recurse(file .. "/")
					else
						coroutine.yield(path, name)
					end
				end
			end
		end
		recurse(path .. "/")
	end)
end

Utils.usage = function(self)
	print([[

code_search.lua [OPTIONS] [PATH]

Options:
  -h  --help                      Show this help message and exit.
  -l  --lines                     Count lines of code.
  -r  --replace STR STR           Replace a string.
  -rp --replace-pattern STR STR   Replace a pattern.
  -rw --replace-whole STR STR     Replace a pattern in the whole document.
  -s  --search STR                Search for a string.
  -sg --search-global STR         Search for a global Lua variable.
  -sp --search-pattern STR        Search for a pattern.
  -sv --search-variable STR       Search for a full variable name.
]])
end

------------------------------------------------------------------------------

local Parse = Class("Parse")

Parse.new = function(clss, path, name)
	local self = Class.new(clss)
	self.path = path
	self.name = name
	self.number = 0
	self.stack = {}
	return self
end

Parse.parse = function(self)
	local file = self:start()
	if not file then return end
	for l in file:lines() do
		self.number = self.number + 1
		self:line(l)
	end
	self:done()
	file:close()
end

Parse.start = function(self)
	if not Utils:get_file_type(self.name) then return end
	return io.open(self.path .. self.name, "r")
end

Parse.line = function(self, line)
end

Parse.done = function(self)
end

------------------------------------------------------------------------------

local Match = Class("Match", Parse)

Match.new = function(clss, path, name)
	local self = Parse.new(clss, path, name)
	self.found = false
	return self
end

Match.line = function(self, line)
	if self:match(line) then
		if not self.found then
			self.found = true
			print("")
			print("In file " .. self.path .. self.name .. " :")
		end
		print(string.format("%d: %s", self.number, line))
	end
end

Match.match = function(self, line)
end

------------------------------------------------------------------------------

local MatchVariable = Class("MatchVariable", Match)

MatchVariable.new = function(clss, path, name, expr)
	local self = Match.new(clss, path, name)
	self.expression = expr
	self.language = Utils:get_file_type(name)
	return self
end

MatchVariable.match = function(self, line)
	return self:match_full_name(line, self.expression)
end

MatchVariable.match_full_name = function(self, line, expr)
	-- Check that the line contains the variable name.
	local s1,e1 = string.find(line, self.expression, 1, true)
	if not s1 then
		return false
	end
	-- Check that the full variable name matches.
	local n = s1 > 1 and string.sub(line, s1-1, s1-1)
	if n and string.find("_abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890", n, 1, true) then
		return false
	end
	local n = string.sub(line, e1+1, e1+1)
	if n and string.find("_abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890", n, 1, true) then
		return false
	end
	-- Check that the line is not in a comment.
	if self.language == "Lua" then
		local s2,e2 = string.find(line, "--", 1, true)
		if s2 and s2 < s1 then
			return false
		end
	elseif self.language == "C" or self.language == "C++" then
		local s2,e2 = string.find(line, "//", 1, true)
		if s2 and s2 < s1 then
			return false
		end
	end
	return true
end

------------------------------------------------------------------------------

local MatchGlobal = Class("MatchGlobal", MatchVariable)

MatchGlobal.new = function(clss, path, name, expr)
	local self = MatchVariable.new(clss, path, name, expr)
	self.expression_local = "local " .. expr .. " = "
	self.required = (Utils:get_file_type(name) ~= "Lua")
	return self
end

MatchGlobal.match = function(self, line)
	-- Check if the variable is local.
	if self.required then
		return false
	end
	if string.find(line, self.expression_local, 1, true) then
		self.required = true
		return false
	end
	-- Check that the line contains the full variable name.
	return self:match_full_name(line, self.expression)
end

------------------------------------------------------------------------------

local MatchPattern = Class("MatchPattern", Match)

MatchPattern.new = function(clss, path, name, expr)
	local self = Match.new(clss, path, name)
	self.expression = expr
	return self
end

MatchPattern.match = function(self, line)
	return string.find(line, self.expression)
end

------------------------------------------------------------------------------

local MatchString = Class("MatchString", Match)

MatchString.new = function(clss, path, name, expr)
	local self = Match.new(clss, path, name)
	self.expression = expr
	return self
end

MatchString.match = function(self, line)
	return string.find(line, self.expression, 1, true)
end

------------------------------------------------------------------------------

local MatchWhole = Class("MatchWhole", Match)

MatchWhole.new = function(clss, path, name, expr)
	local self = Match.new(clss, path, name)
	self.expression = expr
	return self
end

MatchWhole.parse = function(self)
	local file = self:start()
	if not file then return end
	local text = file:read("*a")
	self:match(text)
	self:done()
	file:close()
end

MatchWhole.match = function(self, line)
	for m in string.gmatch(line, self.expression) do
		if not self.found then
			self.found = true
			print("")
			print("In file " .. self.path .. self.name .. " :")
		end
		print(m)
	end
end

------------------------------------------------------------------------------

local Replace = Class("Replace", Match)

Replace.new = function(clss, path, name, expr, repl)
	local self = Match.new(clss, path, name)
	self.count = 0
	self.output = {}
	return self
end

Replace.line = function(self, line)
	local res = self:replace(line)
	if res then
		table.insert(self.output, res)
		self.count = self.count + 1
	else
		table.insert(self.output, line)
	end
end

Replace.replace = function(self, line)
end

Replace.done = function(self)
	if self.count == 0 then return end
	print("In file " .. self.path .. self.name .. " : " .. self.count)
	table.insert(self.output, "")
	local res = table.concat(self.output, "\n")
	local file = io.open(self.path .. self.name, "w")
	file:write(res)
	file:close()
end

------------------------------------------------------------------------------

local ReplaceString = Class("ReplaceString", Replace)

ReplaceString.new = function(clss, path, name, expr, repl)
	local self = Replace.new(clss, path, name)
	self.expression = expr
	self.replacement = repl
	return self
end

ReplaceString.replace = function(self, line)
	local s,e = string.find(line, self.expression, 1, true)
	if not s then return end
	return string.sub(line, 1, s - 1) .. self.replacement .. string.sub(line, e + 1)
end

------------------------------------------------------------------------------

local ReplacePattern = Class("ReplacePattern", Replace)

ReplacePattern.new = function(clss, path, name, expr, repl)
	local self = Replace.new(clss, path, name)
	self.expression = expr
	self.replacement = repl
	return self
end

ReplacePattern.replace = function(self, line)
	local res,num = string.gsub(line, self.expression, self.replacement)
	if num == 0 then return end
	return res
end

------------------------------------------------------------------------------

local ReplaceWhole = Class("ReplaceWhole", ReplacePattern)

ReplaceWhole.new = function(clss, path, name, expr, repl)
	local self = ReplacePattern.new(clss, path, name, expr, repl)
	return self
end

ReplaceWhole.parse = function(self)
	local file = self:start()
	if not file then return end
	local text = file:read("*a")
	self:line(text)
	self:done()
	file:close()
end

------------------------------------------------------------------------------

local CountLines = Class("CountLines")

CountLines.new = function(clss)
	local self = Class.new(clss)
	self.langs = {}
	return self
end

CountLines.parse = function(self, path, name)
	local lang = Utils:get_file_type(name)
	if not lang then return end
	local num = self.langs[lang] or 0
	local file = io.open(path .. name, "r")
	for l in file:lines() do
		num = num + 1
	end
	file:close()
	self.langs[lang] = num
end

CountLines.get_total = function(self)
	local total = 0
	for k,v in pairs(self.langs) do
		total = total + v
	end
	return total
end

CountLines.get_list = function(self)
	local list = {}
	for k,v in pairs(self.langs) do
		table.insert(list, {k, v})
	end
	table.sort(list, function(a, b) return a[2] > b[2] end)
	return list
end

------------------------------------------------------------------------------

local GraphDeps = Class("GraphDeps")

GraphDeps.new = function(clss)
	local self = Class.new(clss)
	self.files = {}
	self.modules = {}

	self.nodes = {}
	self.edges = {}
	return self
end

GraphDeps.parse = function(self, path, name)
	local lang = Utils:get_file_type(name)
	if string.match(name, "__mod__.json") then
		local clean = self:clean_path(path)
		self.modules[clean] = true
	elseif lang == "Lua" then
		local clean = self:clean_path(path)
		local file = io.open(path .. name, "r")
		local deps = {}
		for l in file:lines() do
			local req = string.match(l, "require[(]\"(.*)\"[)]")
			if req then
				deps[req] = true
			end
		end
		self.files[clean .. name] = {clean, name, deps}
	end
end

GraphDeps.clean_path = function(self, path)
	local m = string.match(path, "[.]/data/lipsofsuna/(.*)$")
	if m then return m end
	local m = string.match(path, "[.]/data/system/(.*)$")
	if m then return "system/" .. m end
	local m = string.match(path, "[.]/data/(.*)$")
	if m then return m end
	return path
end

GraphDeps.solve_module = function(self, path, name)
	local c = self:clean_path(path)
	local p = c
	while p do
		if self.modules[p] then return p end
		if self.modules[p .. "/"] then return p .. "/" end
		p = string.match(p, "(.*)/[^/]*$")
	end
	return "system/"
end

GraphDeps.print = function(self)
	local mods = {}
	local modd = {}
	for full,args in pairs(self.files) do
		local m = self:solve_module(args[1], args[2])
		local deps = modd[m]
		if not deps then
			deps = {}
			modd[m] = deps
		end
		for dep in pairs(args[3]) do
			local m1 = self:solve_module(dep)
			if m ~= m1 and m1 ~= "system/" then
				mods[m] = true
				mods[m1] = true
				deps[m1] = true
			end
		end
	end
	print("digraph deps {")
	print("  rankdir=LR;")
	print("  size=50;")
	for m in pairs(mods) do
		print("  node [shape = circle] \"" .. m .. "\";")
	end
	for src,deps in pairs(modd) do
		for dst in pairs(deps) do
			print("  \"" .. src .. "\" -> \"" .. dst .. "\";")
		end
	end
	print("}")
end

------------------------------------------------------------------------------

if #arg == 0 or arg[1] == "-h" or arg[1] == "--help" then
	return Utils:usage()
elseif arg[1] == "-d" or arg[1] == "--deps" then
	if #arg ~= 1 and #arg ~= 2 then return Utils:usage() end
	local deps = GraphDeps()
	for path,name in Utils:find_files(arg[2] or ".") do
		deps:parse(path, name)
	end
	deps:print()
elseif arg[1] == "-l" or arg[1] == "--lines" then
	if #arg ~= 1 and #arg ~= 2 then return Utils:usage() end
	local count = CountLines()
	for path,name in Utils:find_files(arg[2] or ".") do
		count:parse(path, name)
	end
	for k,v in pairs(count:get_list()) do
		print(string.format("%-10s: %d", v[1], v[2]))
	end
	print(string.format("\n%-10s: %d", "Total", count:get_total()))
elseif arg[1] == "-r" or arg[1] == "--replace" then
	if #arg ~= 3 and #arg ~= 4 then return Utils:usage() end
	for path,name in Utils:find_files(arg[4] or ".") do
		ReplaceString(path, name, arg[2], arg[3]):parse()
	end
elseif arg[1] == "-rp" or arg[1] == "--replace-pattern" then
	if #arg ~= 3 and #arg ~= 4 then return Utils:usage() end
	for path,name in Utils:find_files(arg[4] or ".") do
		ReplacePattern(path, name, arg[2], arg[3]):parse()
	end
elseif arg[1] == "-rw" or arg[1] == "--replace-whole" then
	if #arg ~= 3 and #arg ~= 4 then return Utils:usage() end
	for path,name in Utils:find_files(arg[4] or ".") do
		ReplaceWhole(path, name, arg[2], arg[3]):parse()
	end
elseif arg[1] == "-s" or arg[1] == "--search" then
	if #arg ~= 2 and #arg ~= 3 and #arg ~= 3 then return Utils:usage() end
	for path,name in Utils:find_files(arg[3] or ".") do
		MatchString(path, name, arg[2]):parse()
	end
elseif arg[1] == "-sg" or arg[1] == "--search-global" then
	if #arg ~= 2 and #arg ~= 3 then return Utils:usage() end
	for path,name in Utils:find_files(arg[3] or ".") do
		MatchGlobal(path, name, arg[2]):parse()
	end
elseif arg[1] == "-sp" or arg[1] == "--search-pattern" then
	if #arg ~= 2 and #arg ~= 3 then return Utils:usage() end
	for path,name in Utils:find_files(arg[3] or ".") do
		MatchPattern(path, name, arg[2]):parse()
	end
elseif arg[1] == "-sv" or arg[1] == "--search-variable" then
	if #arg ~= 2 and #arg ~= 3 then return Utils:usage() end
	for path,name in Utils:find_files(arg[3] or ".") do
		MatchVariable(path, name, arg[2]):parse()
	end
elseif arg[1] == "-sw" or arg[1] == "--search-whole" then
	if #arg ~= 2 and #arg ~= 3 then return Utils:usage() end
	for path,name in Utils:find_files(arg[3] or ".") do
		MatchWhole(path, name, arg[2]):parse()
	end
else
	return Utils:usage()
end
