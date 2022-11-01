#!/usr/bin/env ruby

require "pp"

RENAMES = {
    #"ankerl::unordered_dense_map" => "ankerl::unordered_dense::map",
    #"ankerl::hash" => "ankerl::unordered_dense::hash",
}

# renames only when benchmark name contains "String"
RENAMES_STRING = {
    #"mumx" => "std::hash",
}

RENAMES_NONSTRING = {
    #"boost::hash" => "boost::hash / std::hash / identity",
    #"std::hash"  =>  "boost::hash / std::hash / identity",
}

def parse_csv(filename, benchmark_name)
    csv = []
    File.open(filename).each_line do |l|
        l = l.gsub("\"", "").split(";").map { |w| w.strip }
        next if (l.size < 5)
        next unless l[2] == benchmark_name

        # "absl::flat_hash_map"; "FNV1a"; "InsertHugeInt"; sort_order, "insert 100M int"; 98841586; 11.8671; 1730.17
        l = l.map { |entry| RENAMES[entry] || entry }
        if l[2] =~ /String/
            l = l.map { |entry| RENAMES_STRING[entry] || entry }
        else
            l = l.map { |entry| RENAMES_NONSTRING[entry] || entry }
        end
        csv.push(l)
    end
    csv
end

# [hashmap_name, hash_name] => benchmark_name => [sort_order, measurement] => [[time...], [memory...]]
def convert_csv_to_hash(csv)
    h = Hash.new do |h,k|
        h[k] = Hash.new do |h,k|
            h[k] = Hash.new do |h,k|
                h[k] = [[], []]
            end
        end
    end

    csv.sort.each do |l|
        next if l.size < 5

        # "absl::flat_hash_map"; "FNV1a"; "InsertHugeInt"; sort_order, "insert 100M int"; 98841586; 11.8671; 1730.17
        hashmap_name, hash_name, benchmark_name, sort_order, measurement_name, validator, runtime, memory = l

        entry = h[[hashmap_name, hash_name]][benchmark_name][[sort_order, measurement_name]]
        if l.size <= 6
            # timeout
            entry[0].push(1e10)
            entry[1].push(1e10)
        else
            entry[0].push(runtime.to_f)
            entry[1].push(memory.to_f)
        end
    end

    h
end

def median(values)
    if values.nil? || values.empty?
        return invalid_value
    end
    v = values.sort
    idx1 = values.length / 2
    idx2 = (values.length - 1) / 2
    (v[idx1] + v[idx2]) / 2
end

# input: 
#   [hashmap_name, hash_name] => benchmark_name => [sort_order, measurement] => [[time...], [memory...]]
# output:
#   hashmap_name; hash_name; max(memory), median(time measurement1), median(time measurement2),...
def print_header(full_hash)
    print("\"hashmap\"; \"hash\"; \"max RSS [MiB]\"")
    full_hash.each do |hashmap_hash, benchmark_hash|
        benchmark_hash.each do |benchmark_name, sort_measurement_hash|
            sort_measurement_hash.keys.sort.each do |nr, name|
                print("; \"#{name}\"")
            end
            print("\n")
            return
        end
    end
end

# input: 
#   [hashmap_name, hash_name] => benchmark_name => [sort_order, measurement] => [[time...], [memory...]]
# output:
#   hashmap_name; hash_name; max(memory), median(time measurement1), median(time measurement2),...
def print_medians(full_hash)
    full_hash.each do |hashmap_hash, benchmark_hash|
        benchmark_hash.each do |benchmark_name, sort_measurement_hash|
            median_times = []
            max_memory = 0.0
            sort_measurement_hash.each do |order_measurement, times_memory|
                median_times += [median(times_memory[0])]
                max_memory = [max_memory, times_memory[1].max].max
            end

            puts "\"#{hashmap_hash[0]}\"; \"#{hashmap_hash[1]}\"; #{max_memory}; #{median_times.join("; ")}"
        end
    end
end

if ARGV.size != 2
    puts "usage: #{__FILE__} <data.txt> <benchmarkname>"
    exit(1)
end

csv = parse_csv(ARGV[0], ARGV[1])
full_hash = convert_csv_to_hash(csv)
print_header(full_hash)
print_medians(full_hash)