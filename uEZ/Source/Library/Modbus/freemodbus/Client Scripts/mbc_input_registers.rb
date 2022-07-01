#!/usr/bin/env ruby 

require 'rmodbus'
require 'yaml'

ip, port = YAML.load_file("config.yml")

ModBus::TCPClient.new(ip, port) do |cl|
  cl.with_slave(1) do |slave|
    # Read a single input register at address 1000 
    slave.input_registers[999]

    # Read input registers 1000 through 1003 
    slave.input_registers[999..1002]
  end
end
