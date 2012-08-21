local Class = require("system/class")

local Debug = Class("Debug")

--- Prints the paths to class instances of the given type.
-- @param clss Debug class.
-- @param find Class name to find.
Debug.dump_paths_by_class_instance = function(clss, find)
	collectgarbage()
	local visited = {}
	local format_var
	local traverse_function
	local traverse_table
	local traverse_var
	format_var = function(var)
		if type(var) == "string" then
			return string.format("%s", var)
		elseif type(var) == "table" then
			local e,r = pcall(tostring, var)
			if r then return r end
		end
		return string.format("<%s>", type(var))
	end
	traverse_function = function(var, path, depth)
		visited[var] = true
		local i = 1
		while true do
			local k,v = debug.getupvalue(var, i)
			if not k then return end
			traverse_var("<upkey>", k, path, depth + 1)
			traverse_var("<upval>", v, path, depth + 1)
			i = i + 1
		end
	end
	traverse_table = function(var, path, depth)
		visited[var] = true
		for k,v in pairs(var) do
			traverse_var(format_var(k) .. "=", k, path, depth + 1)
			if not var.class or (k ~= "class" and k ~= "super") then
				traverse_var(format_var(k) .. "=" .. format_var(v), v, path, depth + 1)
			end
		end
	end
	traverse_var = function(name, var, path, depth)
		path[depth] = name
		if type(var) == "table" then
			if var.class_name == find then
				print(table.concat(path, " / "))
			end
			if not visited[var] then
				traverse_table(var, path, depth)
			end
		elseif type(var) == "function" then
			if not visited[var] then
				traverse_function(var, path, depth)
			end
		end
		path[depth] = nil
	end
	traverse_var("_G", _G, {}, 1)
end

return Debug
