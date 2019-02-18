#!/usr/bin/env ruby

require "pp"

ENV["LD_LIBRARY_PATH"] = "${LD_LIBRARY_PATH}:/home/martinus/dev/abseil-cpp/bazel-bin/absl/container:/home/martinus/dev/abseil-cpp/bazel-bin/absl/hash"

benchs = `./#{Dir["bench*"].first} l`.split("\n")

apps = Dir["bench*"].sort.uniq

STDERR.puts "running these apps:\n\t#{apps.join("\t\n")}"

10.times do |i|
    benchs.each do |l|
        # next unless l =~ /RandomFind/
        STDERR.puts "iteration #{i}"
        apps.each do |f|
            cmd = "timeout 10m ./#{f} f \"^#{l}$\""
            if !system(cmd)
                puts "TIMEOUT: #{f} #{l}"
            end
            sleep 1
        end
        puts
    end
end
