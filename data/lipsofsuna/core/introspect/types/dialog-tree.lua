return {
	name = "dialog tree",
	equals = function(self, val1, val2)
		-- TODO
		for k,v in pairs(val1) do return end
		for k,v in pairs(val2) do return end
		return true
	end,
	write_str = function(self, val)
		local write_table = function(t)
			return self.types_dict["table"].write_str(self, t)
		end
		local write_value = function(v)
			return self.types_dict["generic"].write_str(self, v)
		end
		local write_branch
		write_branch = function(branch, indent)
			-- Get the positional and named arguments.
			local lst1 = {}
			local lst2 = {}
			for k,v in ipairs(branch) do
				table.insert(lst1, v)
			end
			for k,v in pairs(branch) do
				if type(k) ~= "number" then
					table.insert(lst2, {k, v})
				end
			end
			table.sort(lst2, function(a,b) return a[1] < b[1] end)
			-- Write indentation.
			local str = ""
			for i = 1,indent do
				str = str .. "\t"
			end
			str = str .. "{"
			local buf = {str}
			-- Write the positional command arguments.
			local pos = 1
			while true do
				local v = lst1[pos]
				local t = type(v)
				if t ~= "string" and t ~= "number" and t ~= "function" then break end
				if pos > 1 then
					table.insert(buf, ", ")
				end
				table.insert(buf, write_value(v))
				pos = pos + 1
			end
			assert(indent == 0 or pos > 1)
			-- Write named arguments.
			for k,v in ipairs(lst2) do
				local s = write_value(v[2])
				if string.match(v[1], "^[a-z_]*$") then
					table.insert(buf, string.format(", %s = %s", v[1], s))
				else
					table.insert(buf, string.format(", [%q] = %s", v[1], s))
				end
			end
			-- Write the positional branch arguments.
			local branched = false
			while lst1[pos] do
				assert(type(lst1[pos]) == "table", "expected table, got " .. type(lst1[pos]))
				branched = true
				local b = write_branch(lst1[pos], indent > 0 and indent + 1 or 2)
				if pos == 1 then
					table.insert(buf, string.format("\n%s", b))
				else
					table.insert(buf, string.format(",\n%s", b))
				end
				pos = pos + 1
			end
			-- Write indentation.
			str = ""
			if branched then
				str = str .. "\n"
				for i = 1,math.max(indent,1) do
					str = str .. "\t"
				end
			end
			str = str .. "}"
			table.insert(buf, str)
			return table.concat(buf)
		end
		return write_branch(val, 0)
	end}
