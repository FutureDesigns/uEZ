#!/usr/bin/env ruby 

# This file is meant to test the Modbus TCP Server
# Specifically the Read Discrete Input function

require 'rmodbus'
require 'yaml'

ip, port = YAML.load_file("config.yml")

ModBus::TCPClient.new(ip, port) do |cl|
  cl.with_slave(1) do |slave|
    # Read a single discrete input at address 4000 
    slave.read_discrete_inputs  4000, 1

    # Read read discrete inputs 4000 through 4003 
    slave.read_discrete_inputs 4000, 4
  end
end
