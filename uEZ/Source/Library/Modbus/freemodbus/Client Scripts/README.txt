README.txt
Modbusclient

This provides instructions for loading the tools to run some simple modbusclient scripts to test a Modbus TCP slave.  
They use the rmodbus ruby gem.

Test clients have been run on ubuntu 12.04 and OS X.

To install on ubuntu 12.04


sudo apt-get install curl

user$ \curl -sSL https://get.rvm.io | bash -s stable --ruby

May have to run this command if it fails, and then try again
command curl -sSL https://rvm.io/mpapis.asc | gpg2 --import -

user$ source ~/.rvm/scripts/rvm

Find the current list of rubies
user$ rvm list known

If not show run the following command
user$ rvm install "ruby-2.1.10"

Use the newly installed ruby
user$ rvm use 2.1

Verify this woked correctly
user$ ruby -v
should show a ruby matching the version you installed


user$ which ruby
should show ruby installed under ~/.rvm/rubies/...

Set this as your default ruby
user$ rvm use 2.1 --default

Install rmodbus gem
user$ gem install rmodbus

The modbusclient scripts reference a configuration file.  To create the configuration file modify mbc_config.rb to reference your target
ip address and port.  Then run
user$ ./mbc_config.rb
This will generate the config.yml file

To execute the other scripts it is simply
user$ ./script_to_run.rb

May requrie running the following command to make the files executable
chmod +c *.rb

Current scripts:
mbc_config.rb
	- used to generate config.yml which contains IP address and port for target slave device
mbc_coils.rb
	- used to test function 0x01 (read coils) for single and multiple coils.
	- used to test function 0x05 (write single coil)
	- used to test function 0x0f (write multiple coils)
mbc_discrete.rb
	- used to test function 0x02 (read discrete inputs) for single and multiple discrete inputs
mbc_input_registers.rb
	- 8sed to test funciton 0x04 (read input registers) for single and multiple input registers
mbc_quick_connections.rb
	- used to test repetitive connections.
		- open client connection to slave
		- perform a few function requests
		- close connection
		- repeat
mbc_registers.rb
	- used to test function 0x03 (read holding registers) for single and multiple holding registers
	- used to test function 0x06 (write single register)
	- used to test funciton 0x10 (write multiple registers)
mbc_single_connection.rb
	- used to test a single connection
		- open client connection to slave
		- perform a read coils request N times
		- close connection
mbc_test.rb
	- used to test simultaneous connections
		- open N (5) simultaneous connections
		- for all client connections
			- perform requests on a client
			- close connection
		- repeat
		


