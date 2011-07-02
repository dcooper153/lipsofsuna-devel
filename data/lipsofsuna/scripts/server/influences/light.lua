-- Increase light duration.
Influencespec{name = "light", func = function(feat, info, args, value)
	if not args.target then return end
	args.target:inflict_modifier("burning", value)
	args.target:inflict_modifier("bleeding", value)
	args.target:inflict_modifier("light", value)
end}
