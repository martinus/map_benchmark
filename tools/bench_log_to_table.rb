#!/usr/bin/env ruby

require "pp"

IGNORE_BENCHMARKS= ["CtorDtorEmptyMap", "CtorDtorSingleEntryMap"]

def parse_csv(filename)
    csv = []
    File.open(filename).each_line do |l|
        l = l.gsub("\"", "").split(";").map { |w| w.strip }
        next if (l.size < 5)
        next if IGNORE_BENCHMARKS.include?(l[2])
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
        # "absl::flat_hash_map"; "FNV1a"; "InsertHugeInt"; sort_order, "insert 100M int"; 98841586; 11.8671; 1730.17
        hashmap_name, hash_name, benchmark_name, sort_order, measurement_name, validator, runtime, memory = l

        entry = h[[hashmap_name, hash_name]][benchmark_name][[sort_order, measurement_name]]
        entry[0].push(runtime.to_f)
        entry[1].push(memory.to_f)
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
#   [hashmap_name, hash_name] => benchmark_name => [sum_median(time), max(memory)]
def summarize(full_hash)
    summary_hash = Hash.new do |h,k|
        h[k] = Hash.new
    end
        

    full_hash.each do |hashmap_hash, benchmark_hash|
        benchmark_hash.each do |benchmark_name, sort_measurement_hash|
            sum_median_times = 0.0
            max_memory = 0.0
            sort_measurement_hash.each do |order_measurement, times_memory|
                sum_median_times += median(times_memory[0])
                max_memory = [max_memory, times_memory[1].max].max
            end
            #pp ["sum of median / max:", [sum_median_times, max_memory]]

            summary_hash[hashmap_hash][benchmark_name] = [sum_median_times, max_memory]
        end
    end

    summary_hash
end

# normalizes time to lowest time=100
def normalize_time(summary_hash)
    # first find out the minimum per benchmark
    min_time_per_benchmark = Hash.new do |h,k|
        h[k] = Float::INFINITY
    end

    summary_hash.each do |hashmap_hash, benchmark_hash|
        benchmark_hash.each do |benchmark_name, times_memory|
            min_time_per_benchmark[benchmark_name] = [min_time_per_benchmark[benchmark_name], times_memory[0]].min
        end
    end

    # iterate again and normalize to 100
    normalized_hash = Hash.new do |h,k|
        h[k] = Hash.new
    end

    summary_hash.each do |hashmap_hash, benchmark_hash|
        benchmark_hash.each do |benchmark_name, times_memory|
            normalized_hash[hashmap_hash][benchmark_name] = [
                100.0*times_memory[0] / min_time_per_benchmark[benchmark_name],
                times_memory[1]
            ]
        end
    end

    normalized_hash 
end

# input:
#   [hashmap_name, hash_name] => benchmark_name => [sum_median(time), max(memory)]
# output:
#   [avg(score), max(memory), hashmap_name, hash_name]
def sorted_score(normalized_hash)
    scores = []
    normalized_hash.each do |hashmap_hash, benchmark_hash|
        score = 0.0
        max_memory = 0.0
        benchmark_hash.each do |benchmark_name, times_memory|
            score += times_memory[0]
            max_memory = [max_memory, times_memory[1]].max
        end
        score /= benchmark_hash.size
        scores.push [score, max_memory, hashmap_hash]
    end

    scores.sort
end

csv = parse_csv(ARGV[0])
full_hash = convert_csv_to_hash(csv)
summary_hash = summarize(full_hash)
normalized_hash = normalize_time(summary_hash)
scores = sorted_score(normalized_hash)

scores.each do |t, mem, names|
    printf("%5.2f %6.1f %s\n", t, mem, names)
end