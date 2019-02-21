#!/usr/bin/env ruby

require "pp"

def median(values)
    v = values.sort
    idx1 = values.length / 2
    idx2 = (values.length - 1) / 2
    (v[idx1] + v[idx2]) / 2
end

replace_dot_with_comma = true

# benchmark => measurement => hashmap => hash => [[time...], [memory...]]
h = Hash.new do |h,k|
    h[k] = Hash.new do |h,k|
        h[k] = Hash.new do |h,k| 
            h[k] = Hash.new do |h,k|
                h[k] = [[], []]
            end
        end
    end
end

all_hashes = {}
all_hashmaps = {}
File.open(ARGV[0]).each_line do |l|
    l = l.gsub("\"", "").split(";")#.map { |w| w.strip }    
    next unless l.size == 7
    
    # "absl::flat_hash_map"; "FNV1a"; "InsertHugeInt"; "insert 100M int"; 98841586; 11.8671; 1730.17
    hashmap_name, hash_name, benchmark_name, measurement_name, validator, runtime, memory = l;    
    entry = h[benchmark_name][measurement_name][hashmap_name][hash_name]
    entry[0].push runtime.to_f
    entry[1].push memory.to_f

    all_hashmaps[hashmap_name] = true
    all_hashes[hash_name] = true
end

all_hashmaps = all_hashmaps.keys.sort
all_hashes = all_hashes.keys.sort

# print each benchmark as a nice table
def print_result_2d(benchmark_name, measurement_name, hashmap, type, all_hashmaps, all_hashes)
    types = ["runtime [s]", "memory [MiB]"]
    puts "#{benchmark_name} #{types[type]}"
    puts " hashmap | #{all_hashes.join(" | ")}"
    puts "---#{(["|---:"]*all_hashes.size).join}"
    all_hashmaps.each do |hashmap_name|
        printf hashmap_name
        all_hashes.each do |hash_name|
            entry = hashmap[hashmap_name][hash_name]
            if entry[type].empty?
                printf " | timeout"
            else
                printf " | %.3f", median(entry[type])
            end
        end
        puts
    end
    puts
end

h.sort.each do |benchmark_name, measurement|
    measurement.sort.each do |measurement_name, hashmap|
        print_result_2d(benchmark_name, measurement_name, hashmap, 0, all_hashmaps, all_hashes)
        print_result_2d(benchmark_name, measurement_name, hashmap, 1, all_hashmaps, all_hashes)
    end
end


# show pareto front
# benchmark => measurement => hashmap => hash => [[time...], [memory...]]
def print_pareto_front(benchmark_name, measurement, type, all_hashmaps, all_hashes)
    types = ["runtime [s]", "memory [MiB]"]
    puts "#{benchmark_name} #{types[type]}"
    all_measurements = measurement.keys.sort
    
    puts " \"hashmap\"; #{all_measurements.map { |m| "\"#{m}\"" }.join("; ")}"

    all_hashmaps.each do |hashmap_name|
        all_hashes.each do |hash_name|
            printf "\"#{hashmap_name} #{hash_name}\""
            all_measurements.each do |measurement_name|
                entry = measurement[measurement_name][hashmap_name][hash_name]
                if entry[type].empty?
                    printf "; timeout"
                else
                    printf "; %f", median(entry[type])
                end
            end
            puts
        end
    end
    puts
end

h.sort.each do |benchmark_name, measurement|
    print_pareto_front(benchmark_name, measurement, 0, all_hashmaps, all_hashes)
end
