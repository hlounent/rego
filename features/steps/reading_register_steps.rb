=begin
SPDX-License-Identifier: GPL-2.0-or-later

rego -- tools for interfacing with the Rego ground heat pump controller
Copyright (C) 2016,2018  Hannu Lounento

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

require 'dbus'
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

Given(/^the D\-Bus service (.*) exists$/) do |service_name|
  bus = DBus::SessionBus.instance
  until bus.proxy.ListNames[0].include? service_name do
    sleep 0.1
  end
end

When(/^the D\-Bus method ([.a-zA-Z0-9]+) on ([\/a-zA-Z0-9]+) is called$/) do |service, arg2|
  # https://github.com/mvidner/ruby-dbus/blob/master/doc/Tutorial.md
  bus = DBus::SessionBus.instance
  service = bus.service("fi.iki.halo.Rego1")

  object = service.object("/fi/iki/halo/Rego1")
  object.default_iface = "fi.iki.halo.Rego1.Sensors"
  object.ReadAll
end

When(/^Rego sends the response (.+)$/) do |response_in_hex|
  # Wait until the request is sent
  IO.select([@port], nil, nil, 1)

  response = [response_in_hex].pack('H*')
  count = @port.write(response)
end

Then(/^a message should be written to ([\/a-zA-Z0-9]+) within (\d+) ms$/) do |port_path, time|
  port = SerialPort.new(@serial_port_ctl_path, 19200, 8, 1, SerialPort::NONE)
  port.binmode
  port.read_timeout = time.to_i
  # TODO: Remove the fixed length of expected data
  @data_written_to_serial_port = port.read(1024)

  port.close

  @data_written_to_serial_port.length.should > 0
end

Then(/^the message should (only )?contain the bytes ([0-9a-fA-F]+)$/) do |only, expected_message_in_hex|
  expected_bytes = [expected_message_in_hex].pack('H*')
  @data_written_to_serial_port.should == expected_bytes unless only.nil?
  # TODO: Make the check readable and make it dump values on failure
  #@data_written_to_serial_port.should include?(expected_bytes) if only.nil?
  #@data_written_to_serial_port.should =~ expected_bytes if only.nil?
  @data_written_to_serial_port.include?(expected_bytes).should == true if only.nil?
end

Then(/^(only )?the message (.+) should be written to ([\/a-zA-Z0-9]+)$/) do |only, expected_message_in_hex, port_path|
    puts "Expected: " + expected_message_in_hex
  port = SerialPort.new(@serial_port_ctl_path, 19200, 8, 1, SerialPort::NONE)
  port.binmode
  port.read_timeout = 100
  # Two characters per byte
  expected_message_in_hex.length.modulo(2).should == 0
  expected_message_length = expected_message_in_hex.length / 2
  message = port.read(expected_message_length)
  
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
