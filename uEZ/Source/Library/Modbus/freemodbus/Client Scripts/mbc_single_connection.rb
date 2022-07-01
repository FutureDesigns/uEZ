#! /usr/bin/env ruby

require 'rmodbus'
require 'yaml'

ip, port = YAML.load_file("config.yml")

cl = ModBus::TCPClient.new(ip, port)

#sleep(100)
cl.with_slave(1) do |slave|
  100.times do |index|
    puts index
    slave.read_coils 3000, 1
    sleep(5)
  end
end
cl.close