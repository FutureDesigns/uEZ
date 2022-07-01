#!/usr/bin/env ruby 

require 'rmodbus'
require 'yaml'

ip, port = YAML.load_file("config.yml")

ModBus::TCPClient.new(ip, port) do |cl|
  cl.with_slave(1) do |slave|
    # Read a single holding register at address 2000 
    slave.holding_registers[2000]

    # Write a single holding register at address 2000
    slave.holding_registers[2000] = 123

    # Read holding registers 2000 through 2003 
    slave.holding_registers[2000..2003]

    # Write holding registers 2000 through 2003 with some values
    slave.holding_registers[2000..2003] = [1, 2, 3, 4]
  end
end
