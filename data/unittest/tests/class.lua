Unittest:add(1, "class", function()
	require "system/class"
	-- Creating classes.
	local Object = Class()
	Object.new = function(clss, args)
		local self = Class.new(clss)
		self.handle = "foo"
		if args then
			for k,v in pairs(args) do self[k] = v end
		end
		return self
	end
	Object.hello = function(self) return "Hello" end
	-- Class getters and setters.
	local c = Class()
	c.__test = "test"
	c.class_getters = {test = function(s) return s.__test end}
	c.class_setters = {test = function(s, v) s.__test = v end}
	assert(c.test == "test")
	c.test = "jest"
	assert(c.test == "jest")
	-- Instance getters and setters.
	Object:add_getters{value = function(s) return s.__value end}
	Object:add_setters{value = function(s, v) s.__value = v end}
	local o = Object()
	assert(o.value == nil)
	o.value = "some value"
	assert(o.value == "some value")
	-- Setting nil using a setter.
	Object:add_getters{nilval = function(s, v) s.__nilval = v end}
	assert(o.nilval == nil)
	o.nilval = "test"
	assert(o.nilval == "test")
	o.nilval = nil
	assert(o.nilval == nil)
	o.nilval = false
	assert(o.nilval == false)
	-- Name field.
	assert(o.name == nil)
	assert(o.class_name == "Class")
	o.name = "Yui"
	assert(o.name == "Yui")
	o.name = nil
	assert(o.name == nil)
	-- Collecting circular references.
	local test = setmetatable({}, {__mode = "kv"})
	test.a = Object()
	test.b = Object{a = a}
	test.a.b = test.b
	collectgarbage()
	for k,v in pairs(test) do assert(false) end
	-- Inheritance and polymorphism.
	local Creature = Class(Object)
	Creature.new = function(clss, args) return Object.new(clss, args) end
	Creature.hello = function(self) return Object.hello(self) .. " World" end
	local cre = Creature()
	assert(cre:hello() == "Hello World")
	-- Inheritance of accessors.
	cre.value = "x"
	assert(cre.value == "x")
	assert(not rawget(cre, "value"))
	-- Extending accessors.
	Creature:add_getters{value = function(s) return s.__value .. "X" end}
	Creature:add_setters{value = function(s, v) s.__value = "Creature" .. v end}
	cre.value = "Test"
	assert(cre.value == "CreatureTestX")
end)
