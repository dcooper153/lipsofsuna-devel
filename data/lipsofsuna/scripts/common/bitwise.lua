Bitwise = Class()

Bitwise.bchk = function(x, y)
	return x % (y + y) >= y
end

Bitwise.band = function(x, y)
	local v = 0
	for b = 1,31 do
		if Bitwise.bchk(x,2^b) and Bitwise.bchk(y,2^b) then v = v + 2^b end
	end
	return v
end

Bitwise.bor = function(x, y)
	local v = 0
	for b = 1,31 do
		if Bitwise.bchk(x,2^b) or Bitwise.bchk(y,2^b) then v = v + 2^b end
	end
	return v
end
