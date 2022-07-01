#! /usr/bin/env ruby

require 'rmodbus'
require 'yaml'

ip, port = YAML.load_file("config.yml")

10000.times do |index|
  puts index
  
  cl = ModBus::TCPClient.new(ip, port)
  
  cl.with_slave(1) do |slave|
    slave.read_coils 3000, 1
  
    slave.write_single_coil 3000, 1
  
    slave.read_coils 3000, 4
  
    slave.write_multiple_coils 3000, [0, 1 , 0, 1]
  end
  cl.close
  #sleep(1)
end