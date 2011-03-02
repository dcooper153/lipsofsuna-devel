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
	if data and data[2] ~= password then return end
	local self = Class.new(clss, {
		login = login,
		password = password,
		permissions = data and data[3] or 0,
		character = data and data[4]})
	clss.dict_name[login] = self
	return self
end
