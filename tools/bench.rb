#!/usr/bin/env ruby

require 'timeout'

cmd_prefix = "taskset -c 5,11"

timeout_sec = 15*60

benchs = ARGV
if benchs.empty?
    benchs = `./#{Dir["bench*"].first} l`.split("\n")
end
apps = Dir["bench*"].sort.uniq
benchs.delete("CtorDtorEmptyMap")
benchs.delete("CtorDtorSingleEntryMap")
benchs.delete("ShowHash")

# TODO:
# skip boost_hash for non-std::string


# apps = apps.shuffle

STDERR.puts "apps:\n\t#{apps.join("\n\t")}"
STDERR.puts "benchmarks:\n\t#{benchs.join("\n\t")}"

bad_commands = [
    "bench_abseil_flat_hash_map__IdentityHash InsertHugeInt",
    "bench_abseil_flat_hash_map__std_hash InsertHugeInt",
    "bench_abseil_node_hash_map__IdentityHash InsertHugeInt",
    "bench_abseil_node_hash_map__std_hash InsertHugeInt",
    "bench_abseil_flat_hash_map__IdentityHash RandomDistinct2",
    "bench_abseil_flat_hash_map__std_hash RandomDistinct2",
    "bench_abseil_node_hash_map__IdentityHash RandomDistinct2",
    "bench_abseil_node_hash_map__std_hash RandomDistinct2",
    "bench_greg7mdp_spp_sparse_hash_map__IdentityHash RandomFind_200",
    "bench_greg7mdp_spp_sparse_hash_map__std_hash RandomFind_200",
    "bench_ktprime_hash_table8__IdentityHash RandomFind_200",
    "bench_ktprime_hash_table8__std_hash RandomFind_200",
    "bench_abseil_flat_hash_map__IdentityHash RandomFind_2000",
    "bench_abseil_flat_hash_map__std_hash RandomFind_2000",
    "bench_abseil_node_hash_map__IdentityHash RandomFind_2000",
    "bench_abseil_node_hash_map__std_hash RandomFind_2000",
    "bench_greg7mdp_spp_sparse_hash_map__IdentityHash RandomFind_2000",
    "bench_greg7mdp_spp_sparse_hash_map__std_hash RandomFind_2000",
    "bench_ktprime_hash_table7__IdentityHash RandomFind_2000",
    "bench_ktprime_hash_table7__std_hash RandomFind_2000",
    "bench_ktprime_hash_table8__IdentityHash RandomFind_2000",
    "bench_ktprime_hash_table8__std_hash RandomFind_2000",
    "bench_tessil_hopscotch_map__IdentityHash RandomFind_2000",
    "bench_tessil_hopscotch_map__std_hash RandomFind_2000",
    "bench_tessil_robin_map__IdentityHash RandomFind_2000",
    "bench_tessil_robin_map__std_hash RandomFind_2000",
    "bench_tessil_sparse_map__IdentityHash RandomFind_2000",
    "bench_tessil_sparse_map__std_hash RandomFind_2000",
    "bench_abseil_flat_hash_map__IdentityHash RandomFind_500000",
    "bench_abseil_flat_hash_map__std_hash RandomFind_500000",
    "bench_abseil_node_hash_map__IdentityHash RandomFind_500000",
    "bench_abseil_node_hash_map__std_hash RandomFind_500000",
    "bench_greg7mdp_spp_sparse_hash_map__IdentityHash RandomFind_500000",
    "bench_greg7mdp_spp_sparse_hash_map__std_hash RandomFind_500000",
    "bench_ktprime_hash_table7__IdentityHash RandomFind_500000",
    "bench_ktprime_hash_table7__std_hash RandomFind_500000",
    "bench_ktprime_hash_table8__IdentityHash RandomFind_500000",
    "bench_ktprime_hash_table8__std_hash RandomFind_500000",
    "bench_tessil_hopscotch_map__IdentityHash RandomFind_500000",
    "bench_tessil_hopscotch_map__std_hash RandomFind_500000",
    "bench_tessil_robin_map__IdentityHash RandomFind_500000",
    "bench_tessil_robin_map__std_hash RandomFind_500000",
    "bench_tessil_sparse_map__IdentityHash RandomFind_500000",
    "bench_tessil_sparse_map__std_hash RandomFind_500000",
    "bench_abseil_flat_hash_map__IdentityHash RandomInsertErase",
    "bench_abseil_flat_hash_map__std_hash RandomInsertErase",
    "bench_abseil_node_hash_map__IdentityHash RandomInsertErase",
    "bench_abseil_node_hash_map__std_hash RandomInsertErase",
    "bench_greg7mdp_spp_sparse_hash_map__IdentityHash RandomInsertErase",
    "bench_greg7mdp_spp_sparse_hash_map__std_hash RandomInsertErase",
    "bench_ktprime_hash_table7__IdentityHash RandomInsertErase",
    "bench_ktprime_hash_table7__std_hash RandomInsertErase",
    "bench_ktprime_hash_table8__IdentityHash RandomInsertErase",
    "bench_ktprime_hash_table8__std_hash RandomInsertErase",
    "bench_tessil_hopscotch_map__IdentityHash RandomInsertErase",
    "bench_tessil_hopscotch_map__std_hash RandomInsertErase",
    "bench_tessil_sparse_map__IdentityHash RandomInsertErase",
    "bench_tessil_sparse_map__std_hash RandomInsertErase",
    "bench_jiwan_dense_hash_map__std_hash RandomFind_2000",
    "bench_jiwan_dense_hash_map__std_hash RandomFind_500000",
    "bench_jiwan_dense_hash_map__std_hash RandomInsertErase",
]

# convert to hash
h = {}
bad_commands.each do |cmd_key|
    h[cmd_key] = true
end
bad_commands = h 

#first_skip_to = "RandomFind_500000"
first_skip_to = nil

10.times do |iter|
    benchs.each do |bench|
        if first_skip_to && bench != first_skip_to
            next
        end
        first_skip_to = nil

        STDERR.puts
        STDERR.puts "iteration #{iter}"
        apps.each do |app|

            # filter: benchmarks with 'String' run all hashes, benchmarks *without* don't run boost::hash, because std::hash and boost::hash is the same in that case.
            if !(bench =~ /String/) && app =~ /boost_hash/
                puts "SKIPPING #{app} #{bench} non-String boost_hash"
                next
            end
            
            # btree_map filter: only run std_hash, nothing else
            if app =~ /btree_map/ && !(app =~ /std_hash/)
                puts "SKIPPING #{app} #{bench} btree_map not std_hash"
                next
            end

            cmd = "#{cmd_prefix} ./#{app} #{bench}"
            cmd_key = "#{app} #{bench}"
            if bad_commands.key?(cmd_key)
                puts "SKIPPING '#{cmd_key}'"
            else
                pid = Process.spawn(cmd)
                begin
                    Timeout.timeout(timeout_sec) do
                        Process.wait(pid)
                    end                
                    if ($?.exitstatus != 0)
                        puts "ERROR #{$?.exitstatus}: '#{cmd_key}'"
                        bad_commands[cmd_key] = true
                    end
                rescue Timeout::Error
                    Process.kill('TERM', pid)
                    puts "TIMEOUT: #{app} #{bench}"
                    bad_commands[cmd_key] = true
                end
            end
        end
    end
end
