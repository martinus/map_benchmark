#!/usr/bin/env ruby

require "pp"
require "erb"

def median(values, invalid_value = 900)
    if values.nil? || values.empty?
        return invalid_value
    end
    v = values.sort
    idx1 = values.length / 2
    idx2 = (values.length - 1) / 2
    (v[idx1] + v[idx2]) / 2
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
        entry[0].push runtime.to_f
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
                med = median(runtimes)
                runtime_sum += med
                runtimes_median.push(med)
                memory_max = [memory_max, median(memory)].max
            end
            data.push [runtime_sum, memory_max, runtimes_median, hashmap_name]
        end
        hash_to_data[hash_name] = data.sort
    end

    data = hash_to_data.to_a.sort do |a, b|
        # [hashname, [[runtime_sum, memory_max, [runtimes_median], hashmap_name1], [runtime_sum, ...]]
        a[1][0][0] <=> b[1][0][0]
    end

    # finally we have the data exactly as we want, we can generate 
    data
end

h.sort.each do |benchmark_name, hash|
    
    tpl = <<END_PLOTLY_TEMPLATE
<html>

<head>
    <!-- Plotly.js -->
    <script src="https://cdn.plot.ly/plotly-latest.min.js"></script>
</head>

<body>
    <div id="<%= uid %>" style="height:<%= height_em %>em">
        <!-- Plotly chart will be drawn inside this DIV -->
    </div>
    <script>
        var colors = Plotly.d3.scale.category10().range();
% names.each_with_index do |n, idx|
        var m<%= idx %>y = [ <%= n %>];
% end
        var measurement_names = [ <%= measurement_names_str %> ];

        var data = [
% hash.each_with_index do |h, hash_idx|
%   h.each_with_index do do |measurement, measurement_idx|
            {
                x: [ measurement.join(", ") ],
                y: m<%= hash_idx %>y,
                name: 'hash <%= hash_idx %> measurement <%= measurement_idx %>',
                type: 'bar',
                orientation: 'h',
                yaxis: 'y<%= hash_idx == 0 ? '' : hash_idx+1 %>',
                marker: { color: colors[<%= measurement_idx %>], }
            },
%   end
% end
        ];

        var layout = {
            grid: { subplots: [
% hash.each_with_index do |h, hash_idx|
                ['xy<%= hash_idx == 0 ? '' : hash_idx+1 %>'],
% end
            ] },

            barmode: 'stack',
% hash.each_with_index do |h, hash_idx|
            yaxis<%= hash_idx == 0 ? '' : hash_idx+1 %>: { title: 'TODO hash title' },
% end
            legend: { traceorder: 'reversed' },
            margin: { l: 350 },
        };

        Plotly.newPlot('<%= uid %>', data, layout);
    </script>
    </div>

</body>

</html>
END_PLOTLY_TEMPLATE


    # [hashname, [[runtime_sum, memory_max, [runtimes_median], hashmap_name], [runtime_sum, ...]]
    measurement_names = all_measurements[benchmark_name]
    data = convert_benchmark(benchmark_name, hash, all_hashmaps, all_hashes, measurement_names)

    # generate hashmap names for each hash, in order
    names = []
    data.each_with_index do |item, idx|
        names.push(item[1].map { |x| "\"#{x.last}\"" }.join(", "))
    end

    # generate data for each hashname for all measurements
    data.each_with_index do |hashname, d|
        measurement_names.times do |i|
            m = []
            d.each do |runtime_sum, memory_max, runtimes_median, hashmap_name|
                m.push(runtimes_median[measurement_idx])
            end
            # now m contains everything for that measurement
        end
    end
    data.map do |hash_name, hashmap_data

    height_em = [20, (all_hashmaps.size*1.5).to_i+7].max
    uid = "id_#{rand(2**32).to_s(16)}"
    measurement_names_str = measurement_names.map { |n| "\"#{n}\"" }.join(", ")

    puts ERB.new(tpl, 0, "%<>").result(binding)


    pp data
end


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
=begin
h.sort.each do |benchmark_name, measurement|
    measurement.sort.each do |measurement_name, hashmap|
        print_result_2d(benchmark_name, measurement_name, hashmap, 1, all_hashmaps, all_hashes)
        print_result_2d(benchmark_name, measurement_name, hashmap, 0, all_hashmaps, all_hashes)
    end
end
=end

# show pareto front
# benchmark => measurement => hashmap => hash => [[time...], [memory...]]
def print_pareto_front(benchmark_name, measurement, type, all_hashmaps, all_hashes, all_measurements)
    types = ["runtime [s]", "memory [MiB]"]
    puts "#{benchmark_name} #{types[type]}"
    
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

=begin
h.sort.each do |benchmark_name, measurement|
    print_pareto_front(benchmark_name, measurement, 1, all_hashmaps, all_hashes, all_measurements[benchmark_name])
    print_pareto_front(benchmark_name, measurement, 0, all_hashmaps, all_hashes, all_measurements[benchmark_name])
end
=end


=begin

var colors = Plotly.d3.scale.category10().range()

var trace1a = {
  x: [1e-8, 2e-8, 3e-8],
  y: ["robin_hood::unordered_flat_map", "boost::unordered_map", "absl::flat_hash_map"],
  type: 'bar',
  orientation: 'h',
  name: '4 bits, 50M inserts & erase robin_hood::hash', 
  xaxis: 'x',
  yaxis: 'y',
  marker: {  color: colors[0], }
};

var trace1b = {
  x: [3e-8, 2e-8, 4e-8],
  y: ["robin_hood::unordered_flat_map", "boost::unordered_map", "absl::flat_hash_map"],
  type: 'bar',
  orientation: 'h',
  xaxis: 'x',
  yaxis: 'y2',
  marker: {  color: colors[0], }
};


var trace2 = {
  x: [2e-8, 3e-8, 4e-8],
  y: ["boost::unordered_map", "robin_hood::unordered_flat_map", "absl::flat_hash_map"],
  type: 'bar',
  yaxis: 'y2',
  orientation: 'h',
  marker: {  color: colors[1], }

};

var data = [trace1a, trace1b, trace2];

var layout = {
  grid: {subplots:[['xy'], ['xy2']]},
  
  barmode: 'stack',
  yaxis: { title: 'absl::Hash'},
  legend: {traceorder: 'reversed'},
  yaxis2: {title: 'robin_hood::hash',},
                      margin: {
                        l: 350
                    },
};

Plotly.newPlot('myDiv', data, layout);
=end

# we want
# benchmark => hash => [runtime_sum, [runtimes_median], [memory_median], hashmap_name]


# benchmark => measurement => hashmap => hash => [[time...], [memory...]]
def print_plotly(benchmark_name, measurement, all_hashmaps, all_hashes, all_measurements_sorted)
    # [sum, [runtimes], [memory], hashmap_name, hash_name]
    values = []
    all_hashmaps.each do |hashmap_name|
        all_hashes.each do |hash_name|
            dataset = [0.0, [], [], hashmap_name, hash_name]
            all_measurements_sorted.each do |measurement_name|
                entry = measurement[measurement_name][hashmap_name][hash_name]
                r = median(entry[0], 1e10)

                dataset[0] += r
                dataset[1].push (r >= 1e10 ? 0 : r)
                dataset[2].push median(entry[1])
            end
            values.push dataset
        end
    end

    # sort by sum
    values.sort! { |a,b| b <=> a }

    # now we have all data, print it.
    tpl = <<END_PLOTLY_TEMPLATE
        <div id="{{UID}}" style="height:{{HEIGHT}}"></div>
            <script>
                var hashmap_names = [{{HASHMAP_NAMES}}];
                var measurement_names = [{{MEASUREMENT_NAMES}}];
                var measurements = [{{MEASUREMENTS}}];
                var size_MiB = [{{SIZE_MIB}}];

                var data = [];
                for (var i = 0; i < measurement_names.length; ++i) {
                    var trace_x = [];
                    for (var j = 0; j < measurements.length; ++j) {
                        trace_x.push(measurements[j][i]);
                    }                    
                    var trace = {
                        y: hashmap_names,
                        x: trace_x,
                        name: measurement_names[i],
                        type: 'bar',
                        orientation: 'h',
                    };
                    data.push(trace);
                }


                var layout = {
                    barmode: 'stack',
                    title: '{{BENCHMARK_NAME}}',
                    margin: {
                        l: 350
                    },
                    annotations: [],
                    legend: {
                        traceorder: "normal",
                    }
                };
        
                for (var i = 0; i < hashmap_names.length; ++i) {
                    var sum = 0.0;
                    for (var j = 0; j < measurements.length; ++j) {
                        sum += measurements[j][i];
                    }
                    var title = sum.toPrecision(4).toString() + " sec, " + size_MiB[i].toPrecision(4).toString() + " MiB";
                    if (sum == 0) {
                        title = "timeout";
                    }
                    var a = {
                        x: sum,
                        y: hashmap_names[i],
                        text: title,
                        showarrow: false,
                        xanchor: 'left',
                        xshift: 10
                    };
                    layout.annotations.push(a);
                }

                Plotly.newPlot('{{UID}}', data, layout, { showSendToCloud: true });
            </script>
        </div>
END_PLOTLY_TEMPLATE
    
    tpl_prefix = <<END_PLOTLY_TEMPLATE
<html>
    <head><script src="https://cdn.plot.ly/plotly-latest.min.js"></script></head>
    <body>
END_PLOTLY_TEMPLATE

    tpl_postfix = <<END_PLOTLY_TEMPLATE
    </body>
</html>
END_PLOTLY_TEMPLATE

    filename = "#{benchmark_name}.html"
    File.open(filename, "wt") do |f|
        f.write(tpl_prefix)

        all_hashes.each do |hash_name|
            t = tpl.gsub("{{BENCHMARK_NAME}}", "#{benchmark_name} #{hash_name}")
            t.gsub!("{{HEIGHT}}", "#{[20, (all_hashmaps.size*1.5).to_i+7].max}em")
            t.gsub!("{{MEASUREMENT_NAMES}}", all_measurements_sorted.map{ |m| "'#{m}'" }.join(", "))

            text_hashmap_names = ""
            text_measurements = ""
            text_size = ""
            values.each do |sum, runtime_measures, memory_measures, hashmap_name, current_hash_name|
                next unless hash_name == current_hash_name
                text_hashmap_names << "'" << hashmap_name << "',"
                text_measurements << "[" << runtime_measures.join(",") << "],"
                text_size << memory_measures.last.to_s << ","
            end

            t.gsub!("{{HASHMAP_NAMES}}", text_hashmap_names)    
            t.gsub!("{{MEASUREMENTS}}", text_measurements)
            t.gsub!("{{SIZE_MIB}}", text_size)
            t.gsub!("{{UID}}", "id_#{rand(2**32).to_s(16)}")

            f.write(t)
        end

        f.write(tpl_postfix)
    end
    puts "wrote #{filename}"
end

=begin
    # create indices, and sort by sum of all measurements
    time_size_idx = (0...(all_hashmaps.size*all_hashes.size)).to_a.map { |idx| [0.0, 0.0, idx] }
    measurement_values.each do |_, measurement|
        measurement.each_with_index do |m, idx|
            time_size_idx[idx][0] += m[0] # sort by runtime
            time_size_idx[idx][1] = [time_size_idx[idx][1], m[1]].max
        end
    end
    time_size_idx.sort! { |a,b| b <=> a }



    filename = "#{benchmark_name}.html"
    File.open(filename, "wt") do |f|
        f.write tpl_prefix
        
        all_hashes.each do |hash_name|
            t = tpl.gsub("{{BENCHMARK_NAME}}", "#{benchmark_name} #{hash_name}")
            t.gsub!("{{HEIGHT}}", "#{[20, (all_hashmaps.size*1.5).to_i+7].max}em")
            t.gsub!("{{MEASUREMENT_NAMES}}", all_measurements.map{ |m| "'#{m}'" }.join(", "))

            text = ""
            time_size_idx.each do |time, size, idx|
                next unless all_hashes[]
                text << "'" << full_hashmap_names[idx] << "', "
            end

        tpl.gsub!("{{HASHMAP_NAMES}}", text)    

        text_measurements = ""
        all_measurements.each do |measurement_name|
            data = measurement_values[measurement_name]
            text_measurements << "["
            time_size_idx.each do |time, size, idx|
                if (time < 1e5)
                    text_measurements << data[idx][0].to_s
                else
                    text_measurements << "0"
                end
                text_measurements << ","
            end
            text_measurements << "],"
        end
        tpl.gsub!("{{MEASUREMENTS}}", text_measurements)

        text_size_MiB = time_size_idx.map{ |time, size, idx| size }.join(",")
        tpl.gsub!("{{SIZE_MIB}}", text_size_MiB)

    File.write(filename, tpl)
    puts "wrote #{filename}"
end
=end
