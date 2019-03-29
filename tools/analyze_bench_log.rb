#!/usr/bin/env ruby

require "pp"
require "erb"

TEST_CONFIG = Hash.new { |h,k| h[k] = {} }

TEST_CONFIG["RandomFind_200"] =    { "factor" => 1.0/(200 * 5000000), "type" => "avg" }
TEST_CONFIG["RandomFind_2000"] =   { "factor" => 1.0/(2000 * 500000), "type" => "avg" }
TEST_CONFIG["RandomFind_500000"] = { "factor" => 1.0/(500000 * 1000), "type" => "avg" }
MAP_NAMES = {
    "boost::multi_index::hashed_unique" => "boost::multi_index::<br>hashed_unique",
    "robin_hood::unordered_node_map" => "robin_hood::<br>unordered_node_map",
    "robin_hood::unordered_flat_map" => "robin_hood::<br>unordered_flat_map",
    "boost::unordered_map 1_65_1" => "boost::unordered_map",
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
    fmt = ["", "m", "µ", "n", "p", "f", "a", "z", "y"]
    factor = 1.0
    idx = 0
    while n*factor < 1
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

STDIN.each_line do |l|
    l = l.gsub("\"", "").split(";").map { |w| w.strip }
    next if (l.size < 4)

    # "absl::flat_hash_map"; "FNV1a"; "InsertHugeInt"; "insert 100M int"; 98841586; 11.8671; 1730.17
    hashmap_name, hash_name, benchmark_name, measurement_name, validator, runtime, memory = l
    all_measurements[benchmark_name].push(measurement_name) unless all_measurements[benchmark_name].include?(measurement_name)

    entry = h[benchmark_name][hash_name][hashmap_name][measurement_name]
    
    if l.size == 7
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
            elsif TEST_CONFIG[benchmark_name]["type"] == "avg"
                runtime_sum /= all_measurements_sorted.size
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
        xaxis: { automargin: true, },
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

    # generate hashmap names for each hash, in order
    names = []
    data.each_with_index do |item, idx|
        names.push(item[1].map { |x| "\"#{MAP_NAMES[x.last] || x.last}\"" }.join(", "))
    end

    # generate data for each hashname for all measurements
    hash_names = []
    hash = []
    data.each do |hashname, d|
        hash_names.push(hashname)
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

    text = []
    data.each do |hashname, d|
        t = []
        d.each do |runtime_sum, memory_max, runtimes_median, hashmap_name|
            if runtime_sum < 1e10
                time = sprintf("%ss%s", si_format(runtime_sum), (TEST_CONFIG[benchmark_name]["type"] == "avg" ? " avg" : ""))
                mem = sprintf(memory_max >= 10 ? "%.fMB" : "%.1fMB", memory_max)
                t.push "\"#{time} #{mem}\""
            else
                t.push sprintf("\"timeout\"")
            end
        end
        text.push(t)
    end

    height_em = [20, (((all_hashmaps.size + 5) * all_hashes.size) * 1.5).to_i].max
    uid = "id_#{rand(2**32).to_s(16)}"
    measurement_names_str = measurement_names.map { |n| "\"#{n}\"" }.join(", ")

    fn = benchmark_name + ".html"
    File.open(fn, "wt") do |f|
        f.write ERB.new(tpl, 0, "%<>").result(binding)
        puts "wrote #{fn}"
    end
end
