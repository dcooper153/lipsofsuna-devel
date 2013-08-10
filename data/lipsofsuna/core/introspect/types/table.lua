return {
	name = "table",
	equals = function(self, val1, val2)
		-- TODO
	end,
	validate = function(self, val)
		return type(val) == "table" and not val.class
	end,
	write_str = function(self, val)
		local write_table
		local write_value = function(v)
			return self.types_dict["generic"].write_str(self, v)
		end
		local write_named = function(k, v)
			local s = write_value(v)
			if type(k) ~= "string" then
				return string.format("[%s] = %s", tostring(k), s)
			elseif string.match(k, "^[a-z_]*$") then
				return string.format("%s = %s", k, s)
			else
				return string.format("[%q] = %s", k, s)
			end
		end
		write_table = function(t)
			-- Get the positional and named fields.
			local lst1 = {}
			local lst2 = {}
			for k,v in ipairs(t) do
				table.insert(lst1, v)
			end
			for k,v in pairs(t) do
				if type(k) ~= "number" or k > #lst1 then
					table.insert(lst2, {k, v})
				end
			end
			table.sort(lst2, function(a,b) return a[1] < b[1] end)
			-- Write the positional fields.
			local str = "{"
			local first = true
			for k,v in ipairs(lst1) do
				local s = write_value(v)
				if first then
					str = string.format("%s%s", str, s)
					first = false
				else
					str = string.format("%s, %s", str, s)
				end
			end
			-- Write the named fields.
			for k,v in ipairs(lst2) do
				local s = write_named(v[1], v[2])
				if first then
					str = string.format("%s%s", str, s)
					first = false
				else
					str = string.format("%s, %s", str, s)
				end
			end
			return str .. "}"
		end
		return write_table(val)
	end}
