#!/usr/bin/env ruby

ENV["LD_LIBRARY_PATH"] = "${LD_LIBRARY_PATH}:/home/martinus/dev/abseil-cpp/bazel-bin/absl/container:/home/martinus/dev/abseil-cpp/bazel-bin/absl/hash"

Dir["bench*"].sort.each do |f|
    list = `./#{f} l`
    list.split("\n").each do |l|
        cmd = "timeout 10m ./#{f} f \"^#{l}$\""
        #puts cmd
        if !system(cmd)
            puts "TIMEOUT: #{f} #{l}"
        end
    end
end