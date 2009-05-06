#!/usr/bin/ruby

$outdir = ARGV[ARGV.length - 1] + "/"

def find(path)
	dir = Dir.open(path)
	dir.each do |name|
		if name[0] == '.'[0] || name == "CVS"
			next
		end
		full = path + "/" + name
		begin
			if File.stat(full).directory?
				find(full) do
					|p, n| yield(p + "/", n)
				end
			else
				if name[/.*\.c$/]
					yield(path + "/", name)
				end
			end
		rescue StandardError => e
		end
	end
	dir.close
end

def parse(path, name)
	outdir = $outdir
	block = false
	out = File.new(outdir + "default.lua", "w+")
	file = File.new(path + name, "r")
	file.each_line do |l|
		if l["/* @luadoc"]
			block = true
		elsif l["*/"]
			if block
				out.print("\n")
			end
			block = false
		elsif block and l["module \""]
			l[" * "] = ""
			m = String.new(l)
			l["module \""] = ""
			l["\"\n"] = ""
			l.gsub!(".", "-")
			l.downcase!
			out = File.new(outdir + l + ".lua", "w+")
			out.print(m)
		elsif block and
			l[" * "] = ""
			out.print(l)
		end
	end
	file.close
end

for i in 0..(ARGV.length-2)
	find(ARGV[i]) { |path, name| parse(path, name) }
end

