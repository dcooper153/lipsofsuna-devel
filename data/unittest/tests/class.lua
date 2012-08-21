Unittest:add(1, "system", "class", function()
	local Class = require("system/class")
	-- To string.
	assert(tostring(Class) == "class:Class")
	local C0 = Class:new("C0")
	assert(type(C0) == "table")
	assert(tostring(C0) == "class:C0")
	-- Inheritance.
	local C1 = Class("C1")
	C1.C = "C1"
	C1.C1 = "C1"
	local C2 = Class("C2", C1)
	C2.C = "C2"
	C2.C2 = "C2"
	local C3 = Class("C3", C2)
	C3.C = "C3"
	C3.C3 = "C3"
	local I = C3()
	I.C = "I"
	I.I = "I"
	assert(tostring(C1) == "class:C1")
	assert(C1.C == "C1")
	assert(C1.C1 == "C1")
	assert(C1.C2 == nil)
	assert(C1.C3 == nil)
	assert(C1.I == nil)
	assert(tostring(C2) == "class:C2")
	assert(C2.C, "C2")
	assert(C2.C1 == "C1")
	assert(C2.C2 == "C2")
	assert(C2.C3 == nil)
	assert(C2.I == nil)
	assert(tostring(C3) == "class:C3")
	assert(C3.C == "C3")
	assert(C3.C1 == "C1")
	assert(C3.C2 == "C2")
	assert(C3.C3 == "C3")
	assert(C3.I == nil)
	assert(tostring(I) == "data:C3")
	assert(I.C == "I")
	assert(I.C1 == "C1")
	assert(I.C2 == "C2")
	assert(I.C3 == "C3")
	assert(I.I == "I")
	-- Collecting circular references.
	local Object = Class("Object")
	Object.hello = function(self) return "Hello" end
	local test = setmetatable({}, {__mode = "kv"})
	test.a = Object("Object")
	test.b = Object()
	test.b.a = test.a
	test.a.b = test.b
	collectgarbage()
	for k,v in pairs(test) do assert(false) end
	-- Inheritance and polymorphism.
	Object.hello = function(self) return "Hello" end
	local Creature = Class("Creature", Object)
	Creature.new = function(clss) return Object.new(clss) end
	Creature.hello = function(self) return Object.hello(self) .. " World" end
	local cre = Creature()
	assert(cre:hello() == "Hello World")
end)
