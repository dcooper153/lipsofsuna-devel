require(Mod.path .. "entry")

Widgets.Uiintrospectfield = Class(Widgets.Uientry)
Widgets.Uiintrospectfield.class_name = "Widgets.Uiintrospectfield"
Widgets.Uiintrospectfield.brief_fields = {["boolean"] = true, ["color"] = true, ["number"] = true, ["quaternion"] = true, ["string"] = true, ["vector"] = true}

Widgets.Uiintrospectfield.new = function(clss, spec, field)
	-- Format the label string.
	local label = field.label or field.name
	if spec:is_field_set(field.name) then
		label = label .. " (*)"
	end
	-- Format the value string.
	local str = field.type
	local value = spec[field.name]
	if clss.brief_fields[field.type] then
		local t = Introspect.types_dict[field.type]
		if value ~= nil then
			if field.type == "string" then
				str = string.gsub(value, "\n", "\\n")
			else
				str = t.write_str(value)
			end
		else
			str = ""
		end
	end
	str = string.sub(str, 1, 20)
	-- Create the entry custom widget.
	local self = Widgets.Uiwidget.new(clss, nil, label)
	self.help = field.description
	self.spec = spec
	self.field = field
	self.value = str
	self.hint = "$$B\n$$U\n$$D"
	return self
end

Widgets.Uiintrospectfield.apply = function(self)
	Operators.introspect:set_field_name(self.field.name)
	Ui:push_state("introspect/field")
end
