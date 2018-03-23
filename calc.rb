#!/usr/bin/env ruby

include Math

steps = 14
PERIOD = 2**15

r = (-14...14).map{|k|(PI/2) - Math.acos(k.to_f/14)}
r = r.map{|t|t*PERIOD/(PI/2)}.map(&:floor)

total = r[15,14] + r[0,15].map{|t|2**16 + t}

total[29] -= 1

puts '{'
puts total * ",\n"
puts '}'
