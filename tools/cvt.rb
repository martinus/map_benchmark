#!/usr/bin/env ruby
require "tempfile"
require "fileutils"

filename = ARGV.shift
tmp = Tempfile.new("cvt")
searchstr = "\"#{ARGV.shift}\";"
rep = "\"#{ARGV.shift}\"; #{searchstr}"


File.open(tmp, "wt") do |out|
    File.open(filename).each_line do |l|
        out.print l.gsub(searchstr, rep)
    end
end

FileUtils.mv(tmp, filename)