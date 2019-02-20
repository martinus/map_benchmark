#!/usr/bin/env ruby

benchs = ARGV
if benchs.empty?
    benchs = `./#{Dir["bench*"].first} l`.split("\n")
end
apps = Dir["bench*"].sort.uniq 

STDERR.puts "apps:\n\t#{apps.join("\n\t")}"
STDERR.puts "benchmarks:\n\t#{benchs.join("\n\t")}"

10.times do |iter|
    benchs.each do |bench|
        STDERR.puts "iteration #{iter}"
        apps.each do |app|
            cmd = "timeout 10m ./#{app} #{bench}"
            if !system(cmd)
                puts "TIMEOUT: #{app} #{bench}"
            end
        end
        puts
    end
end