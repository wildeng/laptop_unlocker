#!/usr/bin/env ruby

# @author wildeng@hotmail.com
#
# Simple ruby scripts that runs indefinitely and listens on the serial port
# getting every value passed in and using it to get the related password from
# a secret YAML file.
# It then writes it on serial port

require 'rubyserial'
require 'yaml'

#
# setting up some constants to read/write on the serial port
# finding the serial port path depends on the OS you use
PORT_STR = '/dev/tty.usbmodemHIDPC1'.freeze
BAUD_RATE = 9600
DATA_BITS = 8
STOP_BITS = 1

# opening communication with serial port
sp = Serial.new(PORT_STR, BAUD_RATE, DATA_BITS, :none, STOP_BITS)

# just read forever
loop do
  while (i = sp.gets.chomp) # see note 2
    puts 'getting password'
    puts 'current key is: ' + i
    # loading and parsing secrets YAML file which must be written in the
    # following way:
    # key:
    #  your_hex_rfid_tag_code: password
    authorized = YAML.load_file('secrets.yml')

    # writing the password on serial port
    sp.write(authorized['key'][i.to_s])
  end
end

sp.close
