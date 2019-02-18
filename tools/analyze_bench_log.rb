#!/usr/bin/env ruby

require "pp"

def median(values)
    v = values.sort
    idx1 = values.length / 2
    idx2 = (values.length - 1) / 2
    (v[idx1] + v[idx2]) / 2
end

replace_dot_with_comma = true

# benchmark => hashmap => hash => [[time...], [memory...]]
h = Hash.new do |h,k|
    h[k] = Hash.new do |h,k| 
        h[k] = Hash.new do |h,k|
            h[k] = [[], []]
        end
    end
end

all_hashes = {}
all_hashmaps = {}
File.open(ARGV[0]).each_line do |l|    
    l = l.split(";").map { |w| w.strip }    
    next unless l.size == 7
    
    # "absl::flat_hash_map"; "FNV1a"; "InsertHugeInt"; "insert 100M int"; 98841586; 11.8671; 1730.17
    hashmap, hash, benchmark, measurement, validator, runtime, memory = l;    
    benchmark_name = "#{benchmark}: #{measurement}".gsub("\"", "")
    entry = h[benchmark_name][hashmap][hash]
    entry[0].push runtime.to_f
    entry[1].push memory.to_f

    all_hashmaps[hashmap] = true
    all_hashes[hash] = true
end

all_hashmaps = all_hashmaps.keys.sort
all_hashes = all_hashes.keys.sort

# print each benchmark as a nice table
def print_result(benchmark_name, hashmap, type, all_hashmaps, all_hashes)
    types = ["runtime [s]", "memory [MiB]"]
    puts "#{benchmark_name} #{types[type]}"
    puts "; #{all_hashes.join("; ")}"
    all_hashmaps.each do |map|
        printf "#{map}"
        all_hashes.each do |hash|
            entry = hashmap[map][hash]
            if entry[type].empty?
                printf "; \"timeout\""
            else
                printf "; #{median(entry[type])}"
            end
        end
        puts
    end
    puts
end

h.sort.each do |benchmark_name, hashmap|
    print_result(benchmark_name, hashmap, 0, all_hashmaps, all_hashes)
    print_result(benchmark_name, hashmap, 1, all_hashmaps, all_hashes)
end
