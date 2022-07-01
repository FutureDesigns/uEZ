#!/usr/bin/env ruby 

# This file is meant to test the Modbus TCP Server
# Specifically the Read Coils and Write Coils functions

require 'rmodbus'
require 'yaml'

ip, port = YAML.load_file("config.yml")

ModBus::TCPClient.new(ip, port) do |cl|
  cl.with_slave(1) do |slave|
    # Read a single holding register at address 3000 
    slave.read_coils 3000, 1

    # Write a single holding register at address 3000
    slave.write_single_coil 3000, 1

    # Read holding registers 3000 through 3003 
    slave.read_coils 3000, 4

    # Write holding registers 3000 through 3003 with some values
    slave.write_multiple_coils 3000, [0, 1, 0, 1]
  end
end
