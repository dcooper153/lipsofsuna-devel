Unittest:add(1, "lipsofsuna", "serializer: encoding", function()
	local Serializer = require("system/serializer")
	local Quaternion = require("system/math/quaternion")
	local Vector = require("system/math/vector")
	local s = Serializer{}
	-- Boolean.
	assert(s:encode_value("boolean", true) == "true")
	assert(s:decode_value("boolean", "true") == true)
	assert(s:encode_value("boolean", false) == "false")
	assert(s:decode_value("boolean", "false") == false)
	-- Number.
	assert(s:encode_value("number", 111.111) == "111.111")
	assert(s:decode_value("number", "111.111") == 111.111)
	-- String.
	assert(s:encode_value("string", "test") == "test")
	assert(s:decode_value("string", "test") == "test")
	-- Number list.
	assert(s:encode_value("number list", {1,2.2,3}) == "1,2.2,3")
	local res1 = s:decode_value("number list", "1,2.2,3")
	assert(res1)
	assert(res1[1] == 1)
	assert(res1[2] == 2.2)
	assert(res1[3] == 3)
	local res2 = s:decode_value("number list", "1.11")
	assert(res2)
	assert(res2[1] == 1.11)
	assert(s:decode_value("number list", "1..1") == nil)
	assert(s:decode_value("number list", "") == nil)
	assert(s:decode_value("number list", ",") == nil)
	-- Vector.
	assert(s:encode_value("vector", Vector(1,2.2,3)) == "1,2.2,3")
	local res3 = s:decode_value("vector", "1,2.2,3")
	assert(tostring(res3) == "Vector(1,2.2,3)")
	assert(s:decode_value("vector", "1,2.2") == nil)
	assert(s:decode_value("vector", "1,2.2,3,4") == nil)
	-- Quaternion.
	assert(s:encode_value("quaternion", Quaternion(1,2.2,3,4.44)) == "1,2.2,3,4.44")
	local res4 = s:decode_value("quaternion", "1,2.2,3,4.44")
	assert(tostring(res4) == "Quaternion(1,2.2,3,4.44)")
	assert(s:decode_value("quaternion", "1,2.2,3") == nil)
	assert(s:decode_value("quaternion", "1,2.2,3,4.44,5") == nil)
	-- Arbitrary.
	local tmp5 = s:encode_value("lua", {"a", test = 2, [true] = false})
	assert(type(tmp5) == "string")
	local res5 = s:decode_value("lua", tmp5)
	assert(res5[1] == "a")
	assert(res5["test"] == 2)
	assert(res5[true] == false)
end)

Unittest:add(1, "lipsofsuna", "serializer: read", function()
	local Serializer = require("system/serializer")
	local s = Serializer{
		{name = "foo", type = "string"},
		{name = "test", type = "vector", set = function(o, v) o.test,o.extra = v,"ok" end}}
	local obj = {}
	s:read(obj, {{"foo", "bar"}, {"test", "1,2.2,3.33"}})
	assert(obj.foo == "bar")
	assert(tostring(obj.test) == "Vector(1,2.2,3.33)")
	assert(obj.extra == "ok")
end)

Unittest:add(1, "lipsofsuna", "serializer: write", function()
	local Serializer = require("system/serializer")
	local Vector = require("system/math/vector")
	local s = Serializer{
		{name = "test", type = "vector"}}
	local hits = 0
	s:write({test = Vector(1,2.2,3.33)}, function(name, value)
		assert(name == "test")
		assert(value == "1,2.2,3.33")
		hits = hits + 1
	end)
	assert(hits == 1)
end)
