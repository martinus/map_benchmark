#!/usr/bin/env ruby

require "pp"

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
    l = l.gsub("\"", "").split(";").map { |w| w.strip }
    next unless l.size == 7

    # "absl::flat_hash_map"; "FNV1a"; "InsertHugeInt"; "insert 100M int"; 98841586; 11.8671; 1730.17
    hashmap_name, hash_name, benchmark_name, measurement_name, validator, runtime, memory = l

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
        print_result_2d(benchmark_name, measurement_name, hashmap, 1, all_hashmaps, all_hashes)
        print_result_2d(benchmark_name, measurement_name, hashmap, 0, all_hashmaps, all_hashes)
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
    print_pareto_front(benchmark_name, measurement, 1, all_hashmaps, all_hashes)
    print_pareto_front(benchmark_name, measurement, 0, all_hashmaps, all_hashes)
end

# benchmark => measurement => hashmap => hash => [[time...], [memory...]]
def print_plotly(benchmark_name, measurement, type, all_hashmaps, all_hashes)
    all_measurement_names = measurement.keys.sort
    full_hashmap_names = []

    measurement_values = Hash.new { |h,k| h[k] = [] }

    all_hashmaps.each do |hashmap_name|
        all_hashes.each do |hash_name|
            full_hashmap_names.push "#{hashmap_name} #{hash_name}"
            all_measurement_names.each do |measurement_name|
                measurement_values[measurement_name].push median(measurement[measurement_name][hashmap_name][hash_name][type], 1e10)
            end
        end
    end

    # create indices, and sort by sum of all measurements
    sum_idx = (0...full_hashmap_names.size).to_a.map { |idx| [0.0, idx] }
    measurement_values.each do |_, measurement|
        measurement.each_with_index do |m, idx|
            sum_idx[idx][0] += m
        end
    end
    sum_idx.sort! { |a,b| b <=> a }

    # now we have all data, print it.
    tpl = <<END_PLOTLY_TEMPLATE
    <div id="map_benchmark_{{BENCHMARK_NAME}}" style="height:{{HEIGHT}}"><!-- Plotly chart will be drawn inside this DIV --></div>
        <script>
            var hash_names = [{{HASHMAP_NAMES}}];
            var measurement_names = [{{MEASUREMENT_NAMES}}];
            var values = [
                {{MEASUREMENTS}}
            ];

            var data = [];
            for (var i = 0; i < measurement_names.length; ++i) {
                var trace = {
                    y: hash_names,
                    x: values[i],
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
            };
    
            for (var i = 0; i < hash_names.length; ++i) {
                var sum = 0.0;
                for (var j = 0; j < values.length; ++j) {
                    sum += values[j][i];
                }
                var a = {
                    x: sum,
                    y: hash_names[i],
                    text: sum.toPrecision(4).toString(),
                    showarrow: false,
                    xanchor: 'left',
                    xshift: 10
                };
                layout.annotations.push(a);
            }

            Plotly.newPlot('map_benchmark_{{BENCHMARK_NAME}}', data, layout, { showSendToCloud: true });
        </script>
    </div>
END_PLOTLY_TEMPLATE

    tpl.gsub!("{{BENCHMARK_NAME}}", benchmark_name)
    tpl.gsub!("{{HEIGHT}}", "#{(full_hashmap_names.size*1.5).to_i}em")
    tpl.gsub!("{{MEASUREMENT_NAMES}}", all_measurement_names.map{ |m| "'#{m}'" }.join(", "))
    tpl.gsub!("\n", "")
    tpl.gsub!("\t", "")
    tpl.gsub!("  ", "")
    tpl.gsub!(", ", ",")

    text = ""
    sum_idx.each do |sum, idx|
        text << "'" << full_hashmap_names[idx] << "', "
    end
    tpl.gsub!("{{HASHMAP_NAMES}}", text)    

    text = ""
    measurement_values.sort.each do |name, data|
        text << "["
        sum_idx.each do |sum, idx|
            if (sum < 1e5)
                text << data[idx].to_s
            else
                text << "0"
            end
            text << ", "
        end
        text << "], "
    end
    tpl.gsub!("{{MEASUREMENTS}}", text)

    puts tpl
end


h.sort.each do |benchmark_name, measurement|
    print_plotly(benchmark_name, measurement, 1, all_hashmaps, all_hashes)
    print_plotly(benchmark_name, measurement, 0, all_hashmaps, all_hashes)
end
