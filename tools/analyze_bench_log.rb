#!/usr/bin/env ruby

require "pp"

replace_dot_with_comma = true

# benchmark => hashmap => [result]
h = Hash.new { |h,k| h[k] = Hash.new { |h,k| h[k] = [] } }

File.open(ARGV[0]).each_line do |l|
    l = l.split(";").map { |w| w.strip }
    next unless l.size == 4

    time, _, benchmark, map = l

    h[benchmark][map].push time.to_f
end

def median(values)
    v = values.sort
    idx1 = values.length / 2
    idx2 = (values.length - 1) / 2
    (v[idx1] + v[idx2]) / 2
end

h.sort.each do |benchmark, map_results|
    map_results.sort.each do |map, results|
        str = "#{benchmark}; #{map}; #{median(results)}"
        if replace_dot_with_comma
            str = str.gsub(".", ",")
        end
        puts str
    end
end
