#! /usr/bin/env ruby

require 'rmodbus'
require 'yaml'

ip, port = YAML.load_file("config.yml")

15000.times do |index|
  puts index
  
  cl1 = ModBus::TCPClient.new(ip, port)

  cl2 = ModBus::TCPClient.new(ip, port)

  cl3 = ModBus::TCPClient.new(ip, port)

  cl4 = ModBus::TCPClient.new(ip, port)

  cl5 = ModBus::TCPClient.new(ip, port)

  clients = [cl1, cl2, cl3, cl4, cl5]
  
  clients.each_with_index do |cl, index2|
    puts "   " + index2.to_s
    
    cl.with_slave(1) do |slave|
      slave.read_coils 3000, 1
    
      slave.write_single_coil 3000, 1
    
      slave.read_coils 3000, 4
    
      slave.write_multiple_coils 3000, [0, 1 , 0, 1]
      
      #sleep(60)
    end
    
    cl.close
  end
end