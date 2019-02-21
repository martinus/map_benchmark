#!/usr/bin/env ruby

require 'timeout'

timeout_sec = 15*60

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
            cmd = "./#{app} #{bench}"
            pid = Process.spawn(cmd)
            begin
                Timeout.timeout(timeout_sec) do
                    Process.wait(pid)
                end
            rescue Timeout::Error
                Process.kill('TERM', pid)
                puts "TIMEOUT: #{app} #{bench}"
            end
        end
        puts
    end
end
