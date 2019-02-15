#!/usr/bin/env ruby

require "pp"

ENV["LD_LIBRARY_PATH"] = "${LD_LIBRARY_PATH}:/home/martinus/dev/abseil-cpp/bazel-bin/absl/container:/home/martinus/dev/abseil-cpp/bazel-bin/absl/hash"

benchs = `./#{Dir["bench*"].first} l`.split("\n")

apps = Dir["bench*cityhash"]
apps += Dir["bench*node*"]
apps += Dir["bench*absl*nullhash"]

apps = apps.sort.uniq

10.times do |i|
    benchs.each do |l|
        # next unless l =~ /RandomFind/
        apps.each do |f|
            cmd = "timeout 10m ./#{f} f \"^#{l}$\""
            STDERR.puts "iteration #{i}, running '#{cmd}''"
            if !system(cmd)
                puts "TIMEOUT: #{f} #{l}"
            end
            sleep 1
        end
        puts
    end
end
