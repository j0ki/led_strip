#!/usr/bin/env ruby

include Math

steps = 15
PERIOD = 2**15

r = (1-steps..steps).map{|k|(PI/2) - Math.acos(k.to_f/steps)}.
  map{|t|t*PERIOD/(PI/2)}.
  map(&:floor)
total = r[15,15] + r[0,15].map{|t|2**16 + t}

total[29] -= 1

puts '{'
puts total * ",\n"
puts '}'
