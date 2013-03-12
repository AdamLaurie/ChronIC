#! /usr/bin/env python

#  chronic.py - talk to TI chronos ez430 dev kit via RF access point
# 
#  Adam Laurie <adam@aperturelabs.com>
#  http://www.aperturelabs.com
# 
#  This code is copyright (c) Aperture Labs Ltd., 2012, All rights reserved.
#

import serial
import time

CHRONOS_COMMAND= 	{
			'START':'\x07',
			'STOP':'\x09',
			'SYNC_START':'\x30',	# this is disabled in AP firmware!
			'SYNC_SEND':'\x31',
			'SYNC_BUFFER_STATUS':'\x32',
			}

CHRONOS_SYNC_AP_COMMAND=	{
				# standard commands
				'NOP':'\x01',
				'GET_STATUS':'\x02',
				'SET_TIME':'\x03',
				'GET_MEM_MODE1':'\x04',
				'GET_MEM_MODE2':'\x05',
				'ERASE_MEM':'\x06',
				'EXIT':'\x07',
				# chronic extras
				'SET_UP':'\x08',
				'SET_DOWN':'\x09',
				'SET_FREQ':'\x0a',
				'SET_MANCHESTER':'\x0b',
				'SET_DATA_RATE':'\x0c',
				'SET_REPEAT':'\x0d',
				'SET_DELAY':'\x0e',
				'SET_MODULATION':'\x0f',
				'SET_SEQ':'\x10'
				}

BUTTON_DOWN=		'\x01'
BUTTON_UP=		'\x02'
BUTTON_CHUNK_SIZE=	15

# radio moulation
MOD_2_FSK= 		(0x00 << 4)
MOD_2_GFSK=		(0x01 << 4)
MOD_OOK=		(0x03 << 4)
MOD_MSK=		(0x07 << 4)

MOD_FORMAT= 		{
			'FSK':MOD_2_FSK,
			'GFSK':MOD_2_GFSK,
			'OOK':MOD_OOK,
			'MSK':MOD_MSK,
			}

HW_NO_ERROR=	0x06

class chronos:
	def __init__(self, dev, baud):
		self.dev= dev
		self.baud= baud
		self.start()
		# wait for access point to wake up
		time.sleep(1)
	def start(self):
                self.conn = serial.Serial(self.dev, self.baud, timeout = 1)
                ret, dummy= self.write('START', '')
		return ret
	def stop(self):
		ret, dummy= self.write('STOP', '')
		self.conn.close()
		return ret
	def exit(self):
		ret, dummy= self.sendsync('EXIT', '')
		return ret
        def write(self, command, payload):
		msg= '\xff' + CHRONOS_COMMAND[command]
		msg += chr(len(msg) + len(payload) + 1)
		msg += payload
		#print 'sending', msg.encode('hex')
		self.conn.flushInput()
		self.conn.flushOutput()
                self.conn.write(msg)
		time.sleep(1)
		ret= self.conn.read(len(msg))
		#print 'reply', ret.encode('hex')
		if ret[1] == chr(HW_NO_ERROR):
			return True, ret
		else:
			return False, ret
	def sendsync(self, command, payload):
		payload= CHRONOS_SYNC_AP_COMMAND[command] + payload
		return self.write('SYNC_SEND', payload)
	def setsequences(self, button, sequences):
		if button == 'UP':
			button= 0
		else:
			button= 1
		ret, dummy= self.sendsync('SET_SEQ', chr(button)+chr(sequences))
		return ret
	def settime(self, hour, minute, second, year_h, year_l, month, day):
		ret, dummy= self.sendsync('SET_TIME', chr(hour)+chr(minute)+chr(second)+chr(year_h)+chr(year_l)+chr(month)+chr(day)+'\x00'*3)
		return ret
	def sendbutton(self, button, fullpayload):
		if button == 'UP':
			command= 'SET_UP'
		elif button == 'DOWN':
			command= 'SET_DOWN'
		sequence= 0
		for payload in fullpayload:
			# we can only send 15 bytes at a time
			tosend= len(payload)
			if tosend > 63:
				return False 
			chunk= 0
			while(tosend):
				if tosend > BUTTON_CHUNK_SIZE:
					size= BUTTON_CHUNK_SIZE
				else:
					size= tosend
				ret, dummy= self.sendsync(command, chr(sequence) + chr(chunk) + chr(size) + payload[chunk * BUTTON_CHUNK_SIZE:chunk * BUTTON_CHUNK_SIZE + size])
				if not ret:
					return ret
				tosend -= size
				chunk += 1
			sequence += 1
		# now tell watch how many sequences to use
		sequences= 0
		for payload in fullpayload:
			if len(payload):
				sequences += 1
		ret= self.setsequences(button, sequences)
		return ret
	def getstatus(self, chunk):
		return self.sendsync('GET_STATUS',chunk)
	def setdelay(self, delay):
		ret, dummy= self.sendsync('SET_DELAY', chr(delay))
		return ret
	def setfreq(self, freq):
		mhz= 26
		freqmult = (0x10000 / 1000000.0) / mhz
		num = int(freq * freqmult)
		freq0= num & 0xff
		payload= chr(freq0)
		freq1= (num >> 8) & 0xff
		payload += chr(freq1)
		freq2= (num >> 16) & 0xff
		payload += chr(freq2)
                ret, dummy= self.sendsync('SET_FREQ', payload)
		#print '- FREQ2: %02x FREQ1: %02x FREQ0: %02x -' % (freq2, freq1, freq0),
		return ret
	def setmanchester(self, manchester):
		if manchester == 'ON':
			manchester= '\x01'
		elif manchester == 'OFF':
			manchester= '\x00'
		else:
			return False
		ret, dummy= self.sendsync('SET_MANCHESTER', manchester)
		return ret
	def setmodulation(self, modulation):
		mod= chr(MOD_FORMAT[modulation])
		ret, dummy= self.sendsync('SET_MODULATION', mod)
		return ret
	def setrepeat(self, repeat):
		ret, dummy= self.sendsync('SET_REPEAT', chr(repeat))
		return ret
	def setdatarate(self, drate):
		mhz= 26
		drate_e = None
		drate_m = None
		for e in range(16):
			m = int((drate * pow(2,28) / (pow(2,e)* (mhz*1000000.0))-256) + .5)        # rounded evenly
			if m < 256:
				drate_e = e
				drate_m = m
				break
		if drate_e is None:
			return False, None
		drate = 1000000.0 * mhz * (256+drate_m) * pow(2,drate_e) / pow(2,28)
		ret, dummy= self.sendsync('SET_DATA_RATE', chr(drate_e) + chr(drate_m))
		#print '- DRATE_E: %02x DRATE_M: %02x -' % (drate_e, drate_m),
		return ret, drate
        def debounce(self):
                i=self.deb
                while i:
                        self.read()
                        i-=1
