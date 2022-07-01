#! /usr/bin/env ruby

# this file allows you to update the config file programatically
# modify and run
# ./mbc_config.rb

require 'yaml'

File.write("config.yml", ['192.168.1.168', 502].to_yaml)