Account = Class()
Account.dict_client = {}
Account.dict_name = {}
setmetatable(Account.dict_name, {__mode = "v"})

--- Loads or creates an account.
-- @param clss Account class.
-- @param login Login name.
-- @param password Password.
-- @return Account, or nil if authentication failed.
Account.new = function(clss, login, password)
	local data = Serialize:load_account(login)
	local hash = Password:hash(password, Serialize.accounts.password_salt)
	if data and data[2] ~= hash then return end
	local self = Class.new(clss, {
		login = login,
		password = hash,
		permissions = data and data[3] or 0,
		character = data and data[4]})
	clss.dict_name[login] = self
	return self
end
