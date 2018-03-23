#!/usr/bin/env ruby

include Math

steps = 15
PERIOD = 2**15

relative_step_times = (0...steps).map{|k|Math.sin((PI/2) * k/steps)}

rst = relative_step_times

result = rst.map{|t|t*PERIOD/rst.sum}.map(&:floor)

r = (0...steps).map{|k|Math.sin((PI/2)*k/steps)}.map{|t|t*PERIOD}.map(&:floor)
total = r + r.map{|t|2**16 - t}.reverse

total[29] -= 1

puts '{'
total.each{|v|puts v.to_s+','}
puts '}'
