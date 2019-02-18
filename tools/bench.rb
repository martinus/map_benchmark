#!/usr/bin/env ruby

benchs = `./#{Dir["bench*"].first} l`.split("\n")
apps = Dir["bench*"].sort.uniq

STDERR.puts "running these apps:\n\t#{apps.join("\t\n")}"

10.times do |iter|
    benchs.each do |bench|
        STDERR.puts "iteration #{iter}"
        apps.each do |app|
            cmd = "timeout 10m ./#{app} #{bench}"
            if !system(cmd)
                puts "TIMEOUT: #{app} #{bench}"
            end
            sleep 1
        end
        puts
    end
end
