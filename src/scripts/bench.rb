#!/usr/bin/env ruby

ENV["LD_LIBRARY_PATH"] = "${LD_LIBRARY_PATH}:/home/martinus/dev/abseil-cpp/bazel-bin/absl/container:/home/martinus/dev/abseil-cpp/bazel-bin/absl/hash"

list = `./#{Dir["bench*"].first} l`.split("\n")

10.times do 
    list.each do |l|
        # next unless l =~ /RandomFind/
        Dir["bench*"].sort.each do |f|
            sleep 1
            cmd = "timeout 10m ./#{f} f \"^#{l}$\""
            if !system(cmd)
                puts "TIMEOUT: #{f} #{l}"
            end
        end
        puts
    end
end
