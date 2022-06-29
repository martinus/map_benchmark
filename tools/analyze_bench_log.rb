#!/usr/bin/env ruby

require "pp"
require "erb"

TEST_CONFIG = {
    "RandomFind_200" => {
        "factor" => 1.0/(200 * 5_000_000),
        "type" => "avg"
    },
    "RandomFind_2000" => {
        "factor" => 1.0/(2000 * 500_000),
        "type" => "avg"
    },
    "RandomFind_500000" => {
        "factor" => 1.0/(500_000 * 1000),
        "type" => "avg"
    },
    "RandomFindString" => {
        "factor" => 1.0/(100_000 * 1000),
        "type" => "avg",
        "autozoom" => true
    },
    "RandomInsertErase" => {
        "autozoom" => true
    },
    "RandomFindString_1000000" => {
        "factor" => 1.0/(1_000_000 * 200),
        "type" => "avg"
    },
    "CtorDtorEmptyMap" => {
        "factor" => 1.0/100_000_000
    },
    "CtorDtorSingleEntryMap" =>  {
        "factor" => 1.0/50_000_000
    },
}

NAME_REPLACEMENTS = {
    "boost::multi_index::hashed_unique" => "boost::multi_index::hashed_unique",
    "robin_hood::unordered_node_map" => "robin_hood::unordered_node_map",
    "robin_hood::unordered_flat_map" => "robin_hood::unordered_flat_map",
    "boost::unordered_map PoolAllocator 1_80" => "boost::unordered_map & PoolAllocator",
    "boost::unordered_map 1_80" => "boost::unordered_map",
    "boost::unordered_map unsynchronized_pool_resource 1_80" => "boost::unordered_map & unsynchronized_pool_resource",
    "std::unordered_map unsynchronized_pool_resource" => "std::unordered_map unsynchronized_pool_resource",
    "phmap::parallel_node_hash_map" => "gtl::parallel_node_hash_map",
    "phmap::parallel_flat_hash_map" => "gtl::parallel_flat_hash_map",
    "phmap::node_hash_map" => "gtl::node_hash_map",
    "phmap::flat_hash_map" => "gtl::flat_hash_map",

    "Identity" => "libstdc++-v3",
}

def median(values, invalid_value = 900)
    if values.nil? || values.empty?
        return invalid_value
    end
    v = values.sort
    idx1 = values.length / 2
    idx2 = (values.length - 1) / 2
    (v[idx1] + v[idx2]) / 2
end

def si_format(n)
    fmt = ["", "m", "µ", "n"]
    factor = 1.0
    idx = 0
    while n*factor < 1 && idx < fmt.size-1
        factor *= 1000;
        idx += 1
    end
    val = factor*n
    digits = 0;
    digits = 1 if val < 100
    digits = 2 if val < 10
    sprintf("%.#{digits}f%s", factor*n, fmt[idx])
end

replace_dot_with_comma = true

# benchmark => hash => hashmap => measurement => [[time...], [memory...]]
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

# map from benchmark to measurement_names, but sorted.
all_measurements = Hash.new { |h,k| h[k] = [] }

all_data = []
STDIN.each_line do |l|
    l = l.gsub("\"", "").split(";").map { |w| w.strip }
    next if (l.size < 5)
    all_data.push(l)
end

all_data.sort.each do |l|
    # "absl::flat_hash_map"; "FNV1a"; "InsertHugeInt"; sort_order, "insert 100M int"; 98841586; 11.8671; 1730.17
    hashmap_name, hash_name, benchmark_name, sort_order, measurement_name, validator, runtime, memory = l

    # skip stuff
    if hash_name == "Identity" || hash_name == "boost::hash" || hash_name == "std::hash"
        hash_name = "std::hash / boost::hash / Identity"
    end
    if hash_name == "mumxmumxx1"
        next
    end

    all_measurements[benchmark_name].push(measurement_name) unless all_measurements[benchmark_name].include?(measurement_name)

    TEST_CONFIG[benchmark_name] = {} unless TEST_CONFIG[benchmark_name]

    # don't split up like that, try something else
    #entry = h[benchmark_name][hash_name][hashmap_name][measurement_name]
    # TODO try separate chaining and all the rest
    type = "Open Addressing"
    if hashmap_name =~ /std::unordered_map/ || hashmap_name =~ /boost::unordered_map/ || hashmap_name =~ /boost::multi_index/
        type = "Separate Chaining"
    end
    hashmap_name = NAME_REPLACEMENTS[hashmap_name] || hashmap_name
    hashmap_name = "#{hashmap_name}<br>#{hash_name}"

    entry = h[benchmark_name][type][hashmap_name][measurement_name]

    if l.size == 8
        entry[0].push runtime.to_f * (TEST_CONFIG[benchmark_name]["factor"] || 1.0)
        entry[1].push memory.to_f
    else
        # timeout
        entry[0].push 1e10
        entry[1].push 1e10
    end

    all_hashmaps[hashmap_name] = true
    all_hashes[hash_name] = true
end

all_hashmaps = all_hashmaps.keys.sort
all_hashes = all_hashes.keys.sort

# what we get:
#   benchmark => hash => hashmap => measurement => [[time...], [memory...]]
# what we want:
#   benchmark => [hash, [runtime_sum, memory_max, [runtimes_median], hashmap_name]]
#   where [hash, ...] is sorted by fastest hash, and runtimes sorted by hashmap.
def convert_benchmark(benchmark_name, hash, all_hashmaps, all_hashes, all_measurements_sorted)
    hash_to_data = {}
    hash.each do |hash_name, hashmap|
        data = []
        hashmap.each do |hashmap_name, measurements|
            runtime_sum = 0.0
            memory_max = 0.0
            runtimes_median = []
            memory_median = []
            all_measurements_sorted.each do |m|
                runtimes, memory = measurements[m]
                med = median(runtimes, 1e10)
                runtime_sum += med
                runtimes_median.push(med)
                memory_max = [memory_max, median(memory)].max
            end

            # clean up data if we had a timeout 
            if (runtime_sum >= 1e10) 
                runtime_sum = 1e10
                runtimes_median = runtimes_median.map { |x| 0 }
            end

            data.push [runtime_sum, memory_max, runtimes_median, hashmap_name]
        end
        hash_to_data[hash_name] = data.sort do |a, b|
            b <=> a
        end
    end

    data = hash_to_data.to_a.sort do |a, b|
        # [hashname, [[runtime_sum, memory_max, [runtimes_median], hashmap_name1], [runtime_sum, ...]]
        sum_a = a[1].inject(0) { |sum, x| sum + x[0] }
        sum_b = b[1].inject(0) { |sum, x| sum + x[0] }
        #a[1].last[0] <=> b[1].last[0]
        sum_a <=> sum_b
    end

    # finally we have the data exactly as we want, we can generate 
    data
end

#   benchmark => [hash, [runtime_sum, memory_max, [runtimes_median], hashmap_name]]
def create_scatterplots(benchmark_name, hashname_hashmap_paretofront, data)
    File.open("#{benchmark_name}.dat", "wt") do |f|
        data.each do |hashname, d|
            d.each do |runtime_sum, memory_max, runtimes_median, hashmap_name|
                if (runtime_sum >= 1e10) 
                    next
                end
                color = "0xffa0a0"
                if hashname_hashmap_paretofront[hashname][hashmap_name]
                    color = "0xa0d700"
                    name = NAME_REPLACEMENTS[hashmap_name] || hashmap_name
                    f.puts("#{memory_max} #{runtime_sum} #{color} #{name}")
                end
            end
        end
    end

end

h.sort.each do |benchmark_name, hash|  
    tpl = <<END_PLOTLY_TEMPLATE
<html><head></head><body>

<script src="https://cdn.plot.ly/plotly-latest.min.js"></script>
<div id="<%= uid %>" style="height:<%= height_em %>em"></div>
<script>
    var colors = Plotly.d3.scale.category10().range();
% names.each_with_index do |n, idx|
    var m<%= idx %>y = [ <%= n %>];
% end
    var measurement_names = [ <%= measurement_names_str %> ];

    var data = [
% hash.each_with_index do |h, hash_idx|
%   h.each_with_index do |measurement, measurement_idx|
        { x: [ <%= measurement.join(", ") %> ],
          y: m<%= hash_idx %>y, name: measurement_names[<%= measurement_idx %>] + ' (<%= hash_names[hash_idx] %>)', type: 'bar', orientation: 'h', yaxis: 'y<%= hash_idx == 0 ? '' : hash_idx+1 %>', marker: { color: colors[<%= measurement_idx %>], },
%       if measurement_idx == h.size-1
            textposition: 'outside',
            text: [ <%= text[hash_idx].join(", ") %> ],
%       end
        },
%   end
% end
    ];

    var layout = {
        // title: { text: '<%= benchmark_name %>'},
        grid: {
            ygap: 0.1,
            subplots: [
% hash.each_with_index do |h, hash_idx|
            ['xy<%= hash_idx == 0 ? '' : hash_idx+1 %>'],
% end
        ] },

        barmode: 'stack',
% hash_names.each_with_index do |h, hash_idx|
        yaxis<%= hash_idx == 0 ? '' : hash_idx+1 %>: { title: '<%= h %>', automargin: true, },
% end
        xaxis: { automargin: true, <% if (!autozoom) %> range: [0, <%= xaxis_width %>] <% end %> },
        legend: { traceorder: 'normal' },
        margin: { pad: 0, l:0, r:0, t:0, b:0, },
        showlegend:false,
    };

    Plotly.newPlot('<%= uid %>', data, layout);
</script>

</body></html>
END_PLOTLY_TEMPLATE


    # [hashname, [[runtime_sum, memory_max, [runtimes_median], hashmap_name], [runtime_sum, ...]]
    measurement_names = all_measurements[benchmark_name]
    data = convert_benchmark(benchmark_name, hash, all_hashmaps, all_hashes, measurement_names)

    # find out pareto front
    hashname_hashmap_paretofront = Hash.new { |h,k| h[k] = {} }
    data.each do |hashname, d|
        best_memory_max = Float::MAX        
        d.reverse.each do |runtime_sum, memory_max, runtimes_median, hashmap_name|
            if memory_max < best_memory_max
                hashname_hashmap_paretofront[hashname][hashmap_name] = true
                best_memory_max = memory_max
            end
        end
    end

    # generate hashmap names for each hash, in order
    names = []
    data.each do |hashname, d|
        n = []
        d.each do |runtime_sum, memory_max, runtimes_median, hashmap_name|
            prefix = "\""
            postfix = "\""
            if hashname_hashmap_paretofront[hashname][hashmap_name]
                prefix = "\"<b>"
                postfix = "</b>\""
            end
            n.push(prefix + (NAME_REPLACEMENTS[hashmap_name] || hashmap_name) + postfix)
        end
        names.push(n.join(", "))
    end

    # generate data for each hashname for all measurements
    hash_names = []
    hash = []
    data.each do |hashname, d|
        hash_names.push(NAME_REPLACEMENTS[hashname] || hashname)
        measurements = []
        measurement_names.size.times do |measurement_idx|
            m = []
            d.each do |runtime_sum, memory_max, runtimes_median, hashmap_name|
                m.push(runtimes_median[measurement_idx])
            end
            # now m contains everything for that measurement
            measurements.push(m)
        end
        hash.push(measurements)
    end

    best_time = 0
    text = []
    data.each do |hashname, d|
        t = []
        d.each do |runtime_sum, memory_max, runtimes_median, hashmap_name|
            total = runtime_sum
            is_avg = TEST_CONFIG[benchmark_name]["type"] == "avg"
            if is_avg
                total /= measurement_names.size
            end
            if runtime_sum < 1e10
                time = sprintf("%ss%s", si_format(total), (is_avg ? " avg" : ""))
                mem_fmt = nil
                if (memory_max > 100 || memory_max == 0)
                    mem_fmt = "%.0f"
                elsif (memory_max > 10)
                    mem_fmt = "%.1f"
                elsif (memory_max > 1)
                    mem_fmt = "%.2f"
                else
                    mem_fmt = "%.3f"
                end
                mem = sprintf(mem_fmt + "MB", memory_max)

                prefix = "\""
                postfix = "\""
                if hashname_hashmap_paretofront[hashname][hashmap_name]
                    prefix = "\"<b>"
                    postfix = "</b>\""
                end
    
                t.push "#{prefix}#{time}<br>#{mem}#{postfix}"
            else
                t.push sprintf("\"timeout\"")
            end
        end
        text.push(t)
    end

    # first hash, last entry
    best_hash_xrange = data.first[1].first[0]
    found = data.last[1].find{ |x| x[0] < 1e10 }
    worst_hash_xrange = 0.0
    if found
        worst_hash_xrange = found[0]
    end
    #worst_hash_xrange = data.last[1].find{ |x| x[0] < 1e10 }[0] || 0.0
    autozoom = TEST_CONFIG[benchmark_name]["autozoom"]
    xaxis_width = [1.5 * best_hash_xrange, 1.07 * worst_hash_xrange].min

    height_em = [20, (((all_hashmaps.size + 5) * all_hashes.size) * 2.0).to_i].max
    uid = "id_#{rand(2**32).to_s(16)}"
    measurement_names_str = measurement_names.map { |n| "\"#{n}\"" }.join(", ")

    fn = benchmark_name + ".html"
    File.open(fn, "wt") do |f|
        f.write ERB.new(tpl, 0, "%<>").result(binding)
        puts "wrote #{fn}"
    end

    create_scatterplots(benchmark_name, hashname_hashmap_paretofront, data)
end
