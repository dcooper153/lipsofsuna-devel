Unittest:add(1, "password", function()
	require "system/password"
	local rfc3962_test_vectors = {
		{{"password", "ATHENA.MIT.EDUraeburn", 1},
			"cdedb5281bb2f801565a1122b25635150ad1f7a04bb9f3a333ecc0e2e1f70837"},
		{{"password", "ATHENA.MIT.EDUraeburn", 2},
			"01dbee7f4a9e243e988b62c73cda935da05378b93244ec8f48a99e61ad799d86"},
		{{"password", "ATHENA.MIT.EDUraeburn", 1200},
			"5c08eb61fdf71e4e4ec3cf6ba1f5512ba7e52ddbc5e5142f708a31e2e62b1e13"},
		{{"password", "\18\52\86\120\120\86\52\18", 5},
			"d1daa78615f287e6a1c8b120d7062a493f98d203e6be49a6adf4fa574b6e64ee"},
		{{"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX", "pass phrase equals block size", 1200},
			"139c30c0966bc32ba55fdbf212530ac9c5ec59f1a452f5cc9ad940fea0598ed1"},
		{{"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX", "pass phrase exceeds block size", 1200},
			"9ccad6d468770cd51b10e6a68721be611a8b4d282601db3b36be9246915ec82a"},
		{{"\240\157\132\158", "EXAMPLE.COMpianist", 50},
			"6b9cf26d45455a43a5b8bb276a403b39e7fe37a0c41e02c281ff3069e1e94f52"}}
	for k,v in ipairs(rfc3962_test_vectors) do
		local h = Password:hash(v[1])
		assert(h == string.sub(v[2], 0, 32))
	end
	local salt = Password:random_salt(12)
	assert(#salt == 12)
end)
