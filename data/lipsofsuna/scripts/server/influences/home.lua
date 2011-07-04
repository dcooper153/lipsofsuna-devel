-- Increase light duration.
Influencespec{name = "follow", func = function(feat, info, args, value)
	if Marker:find{name="home"} then
		local a = Marker.find{name="home"}
		a.position = args.target.position
	else
		local a = Marker{name="home",position=args.target.position} 
	end
end}
