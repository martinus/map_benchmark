#!/usr/bin/env ruby

require "pp"

IGNORE_BENCHMARKS= ["CtorDtorEmptyMap", "CtorDtorSingleEntryMap"]

RENAMES = {
    "ankerl::unordered_dense_map" => "ankerl::unordered_dense::map",
    "ankerl::hash" => "ankerl::unordered_dense::hash",
}

# renames only when benchmark name contains "String"
RENAMES_STRING = {
    #"mumx" => "std::hash",
}

RENAMES_NONSTRING = {
    #"boost::hash" => "boost::hash / std::hash / identity",
    #"std::hash"  =>  "boost::hash / std::hash / identity",
}

def parse_csv(filename)
    csv = []
    File.open(filename).each_line do |l|
        l = l.gsub("\"", "").split(";").map { |w| w.strip }
        next if (l.size < 5)
        next if IGNORE_BENCHMARKS.include?(l[2])

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

# input:
#   [hashmap_name, hash_name] => benchmark_name => [sum_median(time), max(memory)]
# output:
#   bechmark is extended with "Memory"
def add_memory_benchmark(summary_hash)
    cpy = summary_hash.clone
    cpy.each do |_, benchmark|
        r1 = benchmark["InsertHugeInt"]
        r2 = benchmark["RandomDistinct2"]
        if r1.nil? || r2.nil?
            benchmark["Memory"] = [1e99, 1e99]
        else
            mem = geomean([r1[1], r2[1]])
            benchmark["Memory"] = [mem, mem]
        end
    end
    cpy
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

# input:
#   [hashmap_name, hash_name] => benchmark_name => [sum_median(time), max(memory)]
# output:
#   bench1; bench2; bench3; max(memory)
#   100.0; 123.4; 132.5; hashmap_name; hash_name
#   100.0; 123.4; 132.5; hashmap_name; hash_name
#   ...
def normalized_to_csv(normalized_hash)
    # collect all benchmark names
    benchmark_names = {}
    normalized_hash.each do |hashmap_hash, benchmark_hash|
        benchmark_hash.each do |benchmark_name, _|
            benchmark_names[benchmark_name] = true
        end
    end
    benchmark_names = benchmark_names.keys.sort

    # print header
    header = benchmark_names + ["hashmap", "hash"]
    header = header.map {|name| "\"#{name}\"" }.join("; ")
    print("#{header}\n")

    normalized_hash.each do |hashmap_hash, benchmark_hash|
        benchmark_names.each do |benchmark_name|
            result = benchmark_hash[benchmark_name]
            if result.nil? || result[0] >= 1e10
                print("-; ")
            else
                print("#{result[0]}; ")
            end
        end
        print("\"#{hashmap_hash[0]}\"; \"#{hashmap_hash[1]}\"\n")
    end
end

def geomean(data)
    begin
        r = 0.0
        data.each do |x|
            r += Math::log(x)
        end
        Math::exp(r / data.size)
    rescue TypeError
        "\"timeout\""
    end
end

def val_or_timeout(benchmark, name)
    r = benchmark[name]
    if r.nil? || r[0] > 1e6
        return "\"timeout\""
    end
    r[0]
end

def round_or_timeout(num)
    begin
        num.round
    rescue NoMethodError
        "\"timeout\""
    end
end

# input:
#   [hashmap_name, hash_name] => benchmark_name => [sum_median(time), max(memory)]
# output:
#   {id:1, hm:"fph::DynamicFphMap", h:"std::hash", cpy:3016, ihi:4774, it:3882, rd2:5560, rie:1227, rf200:103, rf2k:100, rf500k:119, ries:622, rfs:227, rfs1m:231, avgn:107, avgs:229, avg:1805},
def normalized_to_tabular(normalized_hash)
    # use the geomean of "InsertHugeInt" and "RandomDistinct2" as memory
    min_mem = 1e99
    normalized_hash.each do |_, benchmark|
        r1 = benchmark["InsertHugeInt"]
        r2 = benchmark["RandomDistinct2"]
        next if r1.nil? || r2.nil?
        mem = geomean([r1[1], r2[1]])
        if (mem < min_mem)
            min_mem = mem
        end
    end

    id = 1
    normalized_hash.each do |hashmap_hash, benchmark|
        hashmap_name, hash_name = hashmap_hash
        # find max
        mem = val_or_timeout(benchmark, "Memory")
        cpy = val_or_timeout(benchmark, "Copy")
        ihi = val_or_timeout(benchmark, "InsertHugeInt")
        it = val_or_timeout(benchmark, "IterateIntegers")
        rd2 = val_or_timeout(benchmark, "RandomDistinct2")
        rie = val_or_timeout(benchmark, "RandomInsertErase")
        rf200 = val_or_timeout(benchmark, "RandomFind_200")
        rf2k = val_or_timeout(benchmark, "RandomFind_2000")
        rf500k = val_or_timeout(benchmark, "RandomFind_500000")
        ries = val_or_timeout(benchmark, "RandomInsertEraseStrings")
        rfs = val_or_timeout(benchmark, "RandomFindString")
        rfs1m = val_or_timeout(benchmark, "RandomFindString_1000000")

        avgn = geomean([rf200, rf2k, rf500k])
        avgs = geomean([rfs, rfs1m])
        avg = geomean([mem, cpy, ihi, it, rd2, rie, rf200, rf2k, rf500k, ries, rfs, rfs1m])
        
        print("{id:#{id}")
        print(", hm:\"#{hashmap_name}\"")
        print(", h:\"#{hash_name}\"")
        print(", mem:#{round_or_timeout(mem)}")
        print(", cpy:#{round_or_timeout(cpy)}")
        print(", ihi:#{round_or_timeout(ihi)}")
        print(", it:#{round_or_timeout(it)}")
        print(", rd2:#{round_or_timeout(rd2)}")
        print(", rie:#{round_or_timeout(rie)}")
        print(", rf200:#{round_or_timeout(rf200)}")
        print(", rf2k:#{round_or_timeout(rf2k)}")
        print(", rf500k:#{round_or_timeout(rf500k)}")
        print(", ries:#{round_or_timeout(ries)}")
        print(", rfs:#{round_or_timeout(rfs)}")
        print(", rfs1m:#{round_or_timeout(rfs1m)}")
        print(", avgn:#{round_or_timeout(avgn)}")
        print(", avgs:#{round_or_timeout(avgs)}")
        print(", avg:#{round_or_timeout(avg)}")
        print("},\n")
        id += 1
    end
end

csv = parse_csv(ARGV[0])
full_hash = convert_csv_to_hash(csv)
summary_hash = summarize(full_hash)
with_mem = add_memory_benchmark(summary_hash)
normalized_hash = normalize_time(with_mem)
#scores = sorted_score(normalized_hash)
#scores.each do |t, mem, names|
#    printf("%5.2f\t%6.1f\t\"%s\"\n", t, mem, names.join(" "))
#end
normalized_to_tabular(normalized_hash)
