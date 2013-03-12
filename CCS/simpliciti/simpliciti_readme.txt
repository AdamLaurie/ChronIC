Some notes about the SimpliciTI configuration used in this project

- The source code is based on the SimpliciTI 1.1.1 release.

- A full SimpliciTI installation contains configurations for many targets and device types. To avoid confusion,
  only the configuration (End Device) and target files (CC430EM) required for the eZ430-Chronos have been used.

- All source code files have been copied into the project physically. Symbolic links have been replaced with
  the real source code file. 
  
- Due to the indirect inclusion scheme of hardware-dependent source code, some source code files have been
  excluded from build. However, they will be included through higher level source code.  

- Some modifications where required to the original source code. All these changes have been marked with [BM].

	bsp_board.c/BSP_InitBoard(void)			Changed from TA0 to TA1 for delay function, because TA0 is already occupied.

	bsp_msp430_defs.h/BSP_EarlyInit(void)	Function removed, because SimpliciTI must run in watch context

	mrfi_radio_interface.c/mrfiRadioInterfaceCmdStrobe(uint8_t addr)
											Added code to properly synchronize with radio interface. Otherwise
											interface could get stuck.
											
	mrfi_radio.c							Changed channel assignment (mrfiLogicalChanTable) for three ISM bands
											Changed power output settings (mrfiRFPowerTable) for three ISM bands
				
	mrfi_radio.c/MRFI_Init(void)			Added frequency offset correction to use calibrated frequency offset
											when starting RF communication
											
	mrfi_radio.c/MRFI_RadioIsr(void)		Changed radio ISR to normal function, since we have a shared radio ISR
	
	nwk_api.c								Made variable sInit_done globally available to allow SimpliciTI to shutdown 
											and restart multiple times
	
	nwk.c/nwk_nwkInit						Added workaround to allow allow SimpliciTI to shutdown 
											and restart multiple times

- If you (for whatever reason) want to upgrade to a newer version of SimpliciTI, please bear in mind that

	a) the access point SimpliciTI version is 1.1.1 (and cannot be updated)

	b) the workarounds used here to enable SimpliciTI to shutdown and restart multiple times might not necessarily
	   work when used with later revisions
	   	   
	   
	