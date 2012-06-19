if not Los.program_load_extension("password") then
	error("loading extension `password' failed")
end

Password = Class()

--- Creates a password hash.
-- @param self Program class.
-- @param pass Password string.
-- @param salt Salt string.
-- @param rounds Number of PBKDF2 rounds.
-- @return Password hash as a hexadecimal string.
Password.hash = function(self, pass, salt, rounds)
	return Los.program_hash_password(pass, salt, rounds)
end

--- Generates a random password salt string.
-- @param self Program class.
-- @param length String length.
-- @return Password salt string.
Password.random_salt = function(self, length)
	return Los.program_random_salt(length)
end
