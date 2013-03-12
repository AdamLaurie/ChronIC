// *************************************************************************************************
//
//      Copyright (C) 2013 Aperture Labs Ltd. - http://aperturelabs.com/
//
//        Author: Adam Laurie <adam@aperturelabs.com>
//
//
//        Redistribution and use in source and binary forms, with or without
//        modification, are permitted provided that the following conditions
//        are met:
//
//          Redistributions of source code must retain the above copyright
//          notice, this list of conditions and the following disclaimer.
//
//          Redistributions in binary form must reproduce the above copyright
//          notice, this list of conditions and the following disclaimer in the
//          documentation and/or other materials provided with the
//          distribution.
//
//          Neither the name of Aperture Labs Ltd. nor the names of
//          its contributors may be used to endorse or promote products derived
//          from this software without specific prior written permission.
//
//        THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
//        "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
//        LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//        A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
//        OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
//        SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
//        LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//        DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//        THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//        (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
//        OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// *************************************************************************************************
//
//
// rf1a extension portions copied from:
// *************************************************************************************************
// cSLight
// Extension to drive the Conrad "Funk-Lichtschalter-mit-Sender"
// http://www.conrad.at/ce/de/product/620587/Funk-Lichtschalter-mit-Sender
// (c) Christian M. Schmid
// 1.1.2011


// system
#include "bm.h"

#define BUTTON_DATA_SIZE						64
#define BUTTON_SEQUENCES						3

// simpliciti has a maximum user payload of 20 bytes so we need to chunk it
#define BUTTON_CHUNK_SIZE						15

#define EMULATION_MODE_NONE						(0u)
#define EMULATION_MODE_DOORBELL					(1u)
#define EMULATION_MODE_RUKU						(2u)

// radio parameters

typedef struct {
    BYTE iocfg2;        // GDO2 Output Configuration
    BYTE iocfg1;        // GDO1 Output Configuration
    BYTE iocfg0;        // GDO0 Output Configuration
    BYTE fifothr;       // RX FIFO and TX FIFO Thresholds
    BYTE sync1;         // Sync Word, High Byte
    BYTE sync0;         // Sync Word, Low Byte
    BYTE pktlen;        // Packet Length
    BYTE pktctrl1;      // Packet Automation Control
    BYTE pktctrl0;      // Packet Automation Control
    BYTE addr;          // Device Address
    BYTE channr;        // Channel Number
    BYTE fsctrl1;       // Frequency Synthesizer Control
    BYTE fsctrl0;       // Frequency Synthesizer Control
    BYTE freq2;         // Frequency Control Word, High Byte
    BYTE freq1;         // Frequency Control Word, Middle Byte
    BYTE freq0;         // Frequency Control Word, Low Byte
    BYTE mdmcfg4;       // Modem Configuration
    BYTE mdmcfg3;       // Modem Configuration
    BYTE mdmcfg2;       // Modem Configuration
    BYTE mdmcfg1;       // Modem Configuration
    BYTE mdmcfg0;       // Modem Configuration
    BYTE deviatn;       // Modem Deviation Setting
    BYTE mcsm2;         // Main Radio Control State Machine Configuration
    BYTE mcsm1;         // Main Radio Control State Machine Configuration
    BYTE mcsm0;         // Main Radio Control State Machine Configuration
    BYTE foccfg;        // Frequency Offset Compensation Configuration
    BYTE bscfg;         // Bit Synchronization Configuration
    BYTE agcctrl2;      // AGC Control
    BYTE agcctrl1;      // AGC Control
    BYTE agcctrl0;      // AGC Control
    BYTE worevt1;       // High Byte Event0 Timeout
    BYTE worevt0;       // Low Byte Event0 Timeout
    BYTE worctrl;       // Wake On Radio Control
    BYTE frend1;        // Front End RX Configuration
    BYTE frend0;        // Front End TX Configuration
    BYTE fscal3;        // Frequency Synthesizer Calibration
    BYTE fscal2;        // Frequency Synthesizer Calibration
    BYTE fscal1;        // Frequency Synthesizer Calibration
    BYTE fscal0;        // Frequency Synthesizer Calibration
    BYTE fstest;        // Frequency Synthesizer Calibration Control
    BYTE ptest;         // Production Test
    BYTE agctest;       // AGC Test
    BYTE test2;         // Various Test Settings
    BYTE test1;         // Various Test Settings
    BYTE test0;         // Various Test Settings
    BYTE partnum;       // Chip ID
    BYTE version;       // Chip ID
    BYTE freqest;       // Frequency Offset Estimate From Demodulator
    BYTE lqi;           // Demodulator Estimate for Link Quality
    BYTE rssi;          // Received Signal Strength Indication
    BYTE marcstate;     // Main Radio Control State Machine State
    BYTE wortime1;      // High Byte of WOR Time
    BYTE wortime0;      // Low Byte of WOR Time
    BYTE pktstatus;     // Current GDOx Status and Packet Status
    BYTE vco_vc_dac;    // Current Setting from PLL Calibration Module
    BYTE txbytes;       // Underflow and Number of Bytes
    BYTE rxbytes;       // Overflow and Number of Bytes
    BYTE rf1aifctl0;    // Radio interface control register 0
    BYTE rf1aifctl1;    // Radio interface control register 1
    BYTE rf1aifctl2;    // Reserved
    BYTE rf1aiferr;     // Radio interface error flag register
    BYTE rf1aiferrv;    // Radio interface error vector word register
    BYTE rf1aifiv;      // Radio interface interrupt vector word register
    BYTE rf1ainstrw;    // Radio instruction word register
    BYTE rf1ainstr1w;   // Radio instruction word register with 1-byte auto-read (low-byte ignored)
    BYTE rf1ainstr2w;   // Radio instruction word register with 2-byte auto-read (low-byte ignored)
    BYTE rf1adinw;      // Radio word data in register
    BYTE rf1astat0w;    // Radio status word register without auto-read
    BYTE rf1astat1w;    // Radio status word register with 1-byte auto-read
    BYTE rf1astat2w;    // Radio status word register with 2-byte auto-read
    BYTE rf1adout0w;    // Radio core word data out register without auto-read
    BYTE rf1adout1w;    // Radio core word data out register with 1-byte auto-read
    BYTE rf1adout2w;    // Radio core word data out register with 2-byte auto-read
    BYTE rf1ain;        // Radio core signal input register
    BYTE rf1aifg;       // Radio core interrupt flag register
    BYTE rf1aies;       // Radio core interrupt edge select register
    BYTE rf1aie;        // Radio core interrupt enable register
    BYTE rf1aiv;        // Radio core interrupt vector word register
    BYTE rf1arxfifo;    // Direct receive FIFO access register
    BYTE rf1atxfifo;    // Direct transmit FIFO access register
} RADIO_REGS;

// Define section
#define st(x)      					do { x } while (__LINE__ == -1)
#define ENTER_CRITICAL_SECTION(x)  	st( x = __get_interrupt_state(); __disable_interrupt(); )
#define EXIT_CRITICAL_SECTION(x)    __set_interrupt_state(x)

#define FIFO_CAPACITY				64L

// radio modulation
#define MASK_MOD_FORMAT				(BIT6 | BIT5 | BIT4)
#define MOD_2_FSK					(0x00 << 4)
#define MOD_2_GFSK					(0x01 << 4)
#define MOD_OOK						(0x03 << 4)
#define MOD_MSK						(0x07 << 4)
// radio sync mode
#define MASK_SYNC_MODE				(BIT1 | BIT0)
#define SYNC_MODE_NONE				0x00
// manchester mode
#define MASK_MANCHESTER				BIT3
// packet control
#define MASK_LENGTH_CONFIG 			(BIT1 | BIT0)
#define LENGTH_FIXED				0x00
#define LENGTH_VARIABLE				0x01
#define LENGTH_INFINITE				0x02
// radio states
#define MASK_MARCSTATE				(BIT4 | BIT3 | BIT2 | BIT1 | BIT0)
#define MARCSTATE_IDLE				0x01
#define MARCSTATE_TX				0x13
#define MARCSTATE_TX_END			0x14
// FIFO
#define MASK_TXBYTES				(BIT6 | BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0)

// prototypes
void do_tx(unsigned char *data, unsigned char packet_length, unsigned long total_length);
void chronic_tx(unsigned char button_data[BUTTON_SEQUENCES][BUTTON_DATA_SIZE],unsigned char sequences);
void chronic_tx_up(u8 line);
void chronic_tx_down(u8 line);
void chronic_config_rf(void);
void WriteBurstPATable(unsigned char *buffer, unsigned char count);
void WriteRfSettings(RADIO_REGS *pRfSettings);

void config_doorbell(u8 line);
void display_doorbell(u8 line, u8 update);
// frequency specific
#ifdef ISM_LF // 433MHz
void config_ruku(u8 line);
void display_ruku(u8 line, u8 update);
// end ISM_LF 433MHz
#endif

