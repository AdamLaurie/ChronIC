#! /usr/bin/env python

#  chronic-cli.py - ChronIC command utility
# 
#  Adam Laurie <adam@aperturelabs.com>
#  http://www.aperturelabs.com
# 
#  This code is copyright (c) Aperture Labs Ltd., 2013, All rights reserved.
#

import sys
import serial
import time
import chronic

if len(sys.argv) < 2:
	print
	print 'Usage: %s <COMMAND> [ARG(s)] ... [<COMMAND> [ARG(s)] ... ]' % sys.argv[0]
	print
	print '  Commands:'
	print
	print '    BAUD <RATE>                                                 Set RF modem baudrate'
	print '    BYRON                                                       Configure for Byron doorbell emulation (433)'
	print '    DELAY <0-255>                                               Delay in MS between each DATA transmission'
	print '    DOWN <HEX> <HEX> <HEX>                                      Set DATA for DOWN button - 3 * 63 bytes'
	print '    EXIT                                                        Force sync mode EXIT on Chronos'
	print '    FREQ <FREQUENCY>                                            Set Frequency (e.g. 433920000)' 
	print '    FRIEDLAND                                                   Configure for Friedland doorbell emulation (868)'
	print "    MAN <'ON'|'OFF'>                                            Set Manchester Encoding"
	print '    MOD <FSK|GFSK|OOK|MSK>                                      Modulation:'
        print '                                                                   FSK -  Frequency Shift Keying'
        print '                                                                   GFSK - Gaussian Frequency Shift Keying'
        print '                                                                   OOK -  On-Off Keying (in ASK mode)'
        print '                                                                   MSK - Multiple Frequency Shift Keying'
	print '    REPEAT <0-255>                                              Number of times to repeat DATA when button pressed'
	print '    RUKU                                                        Configure for Ruku garage door emulation (433)'
	print '    SERIAL <BAUD>                                               Set access point comms baudrate (default 115200)'
	print '    PULSE <WIDTH>                                               Set pulsewidth (baud rate = 1.0/pulsewidth)'
	print '    TIME                                                        Synchronise time/date'
	print '    UP <HEX> <HEX> <HEX>                                        Set DATA for UP button - 3 * 63 bytes'
	print
	print '  Commands will be executed sequentially and must be combined as appropriate.'
	print '  It is recommended to finish with an EXIT to help conserve battery.'
	print
	exit(True)

Port= "/dev/CHRONOS"
SerialBaud= 115200
try:
	ap= chronic.chronos(Port, SerialBaud)
except:
	print
	print '*** warning! default Access Point not found (%s)!' % Port

current= 1
while current < len(sys.argv):
	command= sys.argv[current].upper()
	if command == 'BYRON':
		print
		print '  Setting up for Byron Doorbell'
		print
		print '    Setting Frequency: 433920000'
		ap.setfreq(433920000)
		print '    Setting Manchester Encoding: OFF'
		ap.setmanchester('OFF')
		print '    Setting Delay: 0'
		ap.setdelay(0)
		print '    Setting Repeat: 60'
		ap.setrepeat(60)
		print '    Setting PulseWidth: 0.000320',
		ret, rate= ap.setdatarate(1.0/0.000320)
		print '(%f Baud)' % rate
		print '    Setting UP button: 2C92496DB2000000'
		payload= ['2C92496DB2000000'.decode('hex'),'','']
		ap.sendbutton('UP', payload)
		print '    Setting DOWN button: 2C92496DB2000000'
		ap.sendbutton('DOWN', payload)
		current += 1
		continue
	if command == 'DELAY':
		current += 1
		delay= int(sys.argv[current])
		print
		print '  Setting delay:', delay,
		sys.stdout.flush()
		ret= ap.setdelay(delay)
		if ret:
			print '(OK)'
		else:
			print 'Failed!'
			break
		current += 1
		continue
	if command == 'DOWN' or command == 'UP':
		print
		payload= []
		for x in range(3):
			current += 1
			payload.append(sys.argv[current].decode('hex'))
		print '  Setting %s Button:' % command,
		sys.stdout.flush()
		if ap.sendbutton(command, payload):
			print '(OK)'
		else:
			print 'Failed!'
			break
		current += 1
		continue
	if command == 'EXIT':
		print
		print '  Sending EXIT command'
		ap.exit()
		print
		current += 1
		continue
	if command == 'FREQ':
		current += 1
		freq= int(sys.argv[current])
		print
		print '  Setting Frequency:', freq,
		sys.stdout.flush()
		if ap.setfreq(freq):
			print '(OK)'
		else:
			print 'Failed!'
			break
		current += 1
		continue
	if command == 'FRIEDLAND':
		print
		print '  Setting up for Friedland Doorbell'
		print
		print '    Setting Frequency: 868350000'
		ap.setfreq(868350000)
		print '    Setting Manchester Encoding: OFF'
		ap.setmanchester('OFF')
		print '    Setting Delay: 0'
		ap.setdelay(0)
		print '    Setting Repeat: 40'
		ap.setrepeat(40)
		print '    Setting PulseWidth: 0.000146',
		ret, rate= ap.setdatarate(1.0/0.000146)
		print '(%f Baud)' % rate
		print '    Setting UP button: E36934D24926DA49A4924D2492492492492498'
		payload= ['E36934D24926DA49A4924D2492492492492498'.decode('hex'),'','']
		ap.sendbutton('UP', payload)
		print '    Setting DOWN button: E36934D24926DA49A4924D2492492492492498'
		ap.sendbutton('DOWN', payload)
		current += 1
		continue
	if command == 'MAN':
		current += 1
		manchester= sys.argv[current].upper()
		print
		print '  Setting Manchester Encoding:', manchester,
		sys.stdout.flush()
		if ap.setmanchester(manchester):
			print '(OK)'
		else:
			print 'Failed!'
			break
		current += 1
		continue
	if command == 'MOD':
		current += 1
		modulation= sys.argv[current].upper()
		print
		print '  Setting modulation:', modulation,
		sys.stdout.flush()
		if ap.setmodulation(modulation):
			print '(OK)'
		else:
			print 'Failed!'
			break
		current += 1
		continue
	if command == 'PORT':
		current += 1
		Port= sys.argv[current]
		try:
			ap.close()
		except:
			pass
		print
		print '  Setting Port to %s:' % Port
		ap= chronic.chronos(Port, SerialBaud)
		print
		current += 1
		continue
	if command == 'PULSE':
		current += 1
		pulsewidth= float(sys.argv[current])
		print
		print '  Setting pulsewidth:', pulsewidth,
		sys.stdout.flush()
		ret, rate= ap.setdatarate(1.0/pulsewidth)
		if ret:
			print '(%f Baud)' % rate,
			sys.stdout.flush()
		else:
			print 'Failed!'
			break
		print '(OK)'
		current += 1
		continue
	if command == 'REPEAT':
		current += 1
		repeat= int(sys.argv[current])
		print
		print '  Setting repeat:', repeat,
		sys.stdout.flush()
		ret= ap.setrepeat(repeat)
		if ret:
			print '(OK)'
		else:
			print 'Failed!'
			break
		current += 1
		continue
	if command == 'RUKU':
		print
		print '  Setting up for RUKU'
		print
		print '    Setting Frequency: 393080000'
		ap.setfreq(393080000)
		print '    Setting Manchester Encoding: OFF'
		ap.setmanchester('OFF')
		print '    Setting Delay: 0'
		ap.setdelay(0)
		print '    Setting Repeat: 6'
		ap.setrepeat(6)
		print '    Setting PulseWidth: 0.000100',
		ret, rate= ap.setdatarate(1.0/0.000100)
		print '(%f Baud)' % rate
		print '    Setting UP button: 924B2DB6C80000'
		payload= ['924B2DB6C8'.decode('hex')]*3
		ap.sendbutton('UP', payload)
		print '    Setting DOWN button: 924B2D96C80000'
		payload= ['924B2D96C8'.decode('hex')]
		ap.sendbutton('DOWN', payload)
		current += 1
		continue
	if command == 'SERIAL':
		current += 1
		SerialBaud= int(sys.argv[current])
		ap.close()
		print
		print '  Setting Baud rate to %d:' % SerialBaud
		ap= chronic.chronos(Port, SerialBaud)
		print
		current += 1
		continue
	if command == 'TIME':
		print
		hour= int(time.localtime().tm_hour)
		minute= int(time.localtime().tm_min)
		second= int(time.localtime().tm_sec)
		year= int(time.localtime().tm_year)
		month= int(time.localtime().tm_mon)
		day= int(time.localtime().tm_mday)
		print '  Setting Time & Date to: %d-%02d-%02d %02d:%02d:%02d' % (year, month, day, hour, minute, second),
		sys.stdout.flush()
		hour += 0x80
		year_h= (year & 0xff00) >> 8
		year_l= year & 0xff
		if ap.settime(hour, minute, second, year_h, year_l, month, day):
			print '(OK)'
		else:
			print 'Failed!'
			break
		print
		current += 1
		continue
	print 'Unrecognised command:', sys.argv[current]
	exit(True)
print
ap.stop()
exit(False)
