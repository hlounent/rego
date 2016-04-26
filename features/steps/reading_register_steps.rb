=begin
rego -- tools for interfacing with the Rego ground heat pump controller
Copyright (C) 2016  Hannu Lounento

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
=end

require 'rspec'
require 'serialport'

Given(/^Rego is connected via serial line ([\/a-zA-Z0-9]+)$/) do |port_path|
  @serial_port_path = port_path
  @serial_port_ctl_path = port_path + "_ctl"
  command = "socat pty,raw,echo=0,link=#{@serial_port_path} pty,raw,echo=0,link=#{@serial_port_ctl_path}"
  @socat_pid = Kernel.spawn(command)
  
  until File.exists?(@serial_port_path) do
    sleep 0.1
  end
  @port = SerialPort.new(@serial_port_ctl_path, 19200, 8, 1, SerialPort::NONE)
  @port.binmode
end

When(/^Rego sends the response (.+)$/) do |response_in_hex|
  # Wait until the request is sent
  IO.select([@port], nil, nil, 1)

  response = [response_in_hex].pack('H*')
  count = @port.write(response)
end

Then(/^(only )?the message (.+) should be written to ([\/a-zA-Z0-9]+)$/) do |only, expected_message_in_hex, port_path|
  port = SerialPort.new(@serial_port_ctl_path, 19200, 8, 1, SerialPort::NONE)
  port.binmode
  port.read_timeout = 1
  message = port.read(9)
  
  ready_io_objects = IO.select([port], nil, nil, 0) unless only.nil?
  
  port.close

  message.length.should > 0
  expected_message = [expected_message_in_hex].pack('H*')
  message.should == expected_message
  
  ready_io_objects.should == nil unless only.nil?
end

When(/^there is a delay of (\d+) ms$/) do |delay|
  sleep (delay.to_i / 1000.0)
end
