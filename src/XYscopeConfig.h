#ifndef XYsConfig
	#define XYsConfig
	
	//	v=========== Maximum Point Array Size =================================v	
	//	CFG_MaxArraySize sets the size of the XYlist[ ] array. The actual
	//	RAM consumed in bytes = CFG_MaxArraySize X 4.
	//		Example: 	CG_MaxArraySize = 25000 means that ~100KB of RAM 
	//					will be required and consumed by the XYlist[] array.
	//
	//	ARDUIONO DUE has 96KB of RAM; this limits max array to ~15-20K points.
	//	NOTE: YOUR limits may vary due to your other variables consuming RAM.
	//	When you exceed available RAM, the compiler errors-out w/message like this:
	//
	// 	|	collect2.exe: error: ld returned 1 exit status				|
	//	|	exit status 1												|
	//	|	Error compiling for board Arduino Due (Programming Port).	|
	//							
	//	TEENSY 3.6 has 256KB of RAM; MaxArraySize size can be upwards of 35K.
	//	NOTE: YOUR limits may vary due to your other variables consuming RAM.
	//	However, practical limits for reasonable refresh rates is 18-25K points. 
	#if defined(__SAM3X8E__)
		#define CFG_MaxArraySize 15000		//DUE Array Size Setting
	#endif
	#if defined(__MK66FX1M0__)
		#define CFG_MaxArraySize 35000  	//TEENSY 3.6 Array Size Setting
	#endif
	//	^=========== Maximum Point Array Size =================================^
	
	//=========== PreventScreenWrap - Screen Fold-Over Management ==========	
	//	Valid XY coordinates have values that are >= 0 AND <= 4095. The constant
	//	'PreventScreenWrap' defines how out-of-range XY coordinates are handled.
	//		If set true, out-of-range points are skipped (aka: not plotted).
	//		If set false, coordinates are masked to 12 bits and then plotted; this means out-of-range
	//		points will be plotted, but will appear to 'fold-over' to the opposite screen edge.
	const bool PreventScreenWrap = true;	//Set false to allow out-of-range points to 'fold over' & plot to CRT

	
	//=========== Set CRT Refresh Period ===================================	
	#define CFG_CrtMinRefresh_us 20000	//Defines the minimum refresh period (in microseconds)
										//Common settings: 20,000us (50 Hz refresh rate), 16,666us (60Hz Refresh Rate)
	
	//=========== FONT SELECTION ===========================================
	//	Two different FONT files and FONT generation methods are available.
	//	The VectorFontROM defines characters using arcs and lines.  The HersheyFontROM
	//	defines all characters using straight lines.  The VectorFontROM takes less
	//	program memory and scales well without degradation (large characters look good),
	//	the HersheyFontROM plots much quicker and is recommended as long as PROGRAM 
	//	memory is not an issue.
	//
	//	Select one (or both if desired) by setting the appropriate CFG_Include... to 'true'
	//	

							
	#define CFG_IncludeHersheyFontROM true	//Set to 'true' or 'false' If true, this font will be available for plotting
											//Set to false if you don't want this font available;it will save FLASH space
		
	#define CFG_IncludeVectorFontROM true	//Set to 'true' or 'false'; if true, this font will be available for plotting
											//Set to false if you don't want need this font available;it will save FLASH space
											
	
	#if CFG_IncludeHersheyFontROM==true 	//Will start up using Hershey IF it is loaded
		//Select HERSHEY font as the default at startup
		#define CFG_StartupFont 1	//Set to '1' to set startup-default-font to HershyFontROM
	#else
		//Select VECTOR font as the default at startup ONLY if Hershey is NOT PRESENT
		#define CFG_StartupFont 0	//Set to '0' to set startup-default-font to VectorFontROM						//Set to '1' to set startup-default-font to HersheyFontROM
	#endif
	
	
	//=========== IGNORE UNDEFINED CHARACTERS ============================== 
	#define CFG_IgnoreUndefinedCharacters true			//UNCOMMENT this line and any out-of-bounds/undefined characters will be ignored
	//#define CFG_IgnoreUndefinedCharacters false		//UNCOMMENT OUT this line and any out-of-bounds/undefined characters print as '~'
												
	//=========== Z-Blank Digital I/O pin Definition =======================
	//  For DUE running DMA, Pin 2 is MUST be used for DMA clock. CFG_Z_blank_pin can be any pin EXCEPT 2; Pin 3 is asignment typical.
	// 	For DUE running PIO, any digital pin may be assigned to this function; Pin 3 is the default assignment.
	//  For TEENSY 3.6, any digital pin can be used; Pin 3 is the default assignment.
	#define CFG_Z_blank_pin	3	//Define Z-Axis blanking I/O pin number
	
	//=========== DUE Paint Mode Selection (DMA or PIO) ===================
	//
	#define CFG_DUE_Use_DMA		//UNCOMMENT = DMA Mode, COMMENT OUT = DUE PIO Mode
								// Note: The DUE is not fast enough to perform well 
								// using the PIO method.  Because of this, further
								// effort to optimize DUE PIO has be been stopped.
								// ===> DUE PIO mode is NOT RECOMMENDED <====

	//=========== DUE DAC Pin Assignments (Not used for DUE DMA-mode) =====
	//	For Due Only.  Only used when running DUE_PIO mode.  Not used for DUE_DMA mode.
	#define CFG_Due_DAC0_pin DAC0
	#define CFG_Due_DAC1_pin DAC1
	
	//=========== PIO Blanking Logic Settings ==============================
	//	You can 'flip' the Z-blanking polarity in SOFTWARE using this bit or with the board jumper using HARDWARE
	//
	#define CFG_PioPositiveBlankingLogic	//UNCOMMENT this line for POSITIVE LOGIC 
											//	Logic level '1' (high) = BEAM ON 	
											//COMMENT OUT this line for NEGATIVE LOGIC
											//	Logic level '0' (low) = BEAM ON	
	
	//=========== TEENSY DAC Reference voltage Selection ===================
	//TEENSY 3.x ONLY DAC Voltage Reference	Option.  This sets the maximum DAC output voltage (when DAC value = 4095)	
	#define CFG_TNSY_DacRefVolts_LOW	//UNCOMMENT this line: 		DAC will initialize using 1.5V DAC reference
										//COMMENT OUT this line: 	DAC will initialize using 3.3V DAC reference
										//NOTE: 1.5V reference is PREFERRED! 3.3V ref exhibits non-linear
										//   	'flat spots' above about 3900 counts
										// 

	//=========== Minimum PIO Compute Time =================================
	//	This is the minimum extra time tacked onto the refresh period to guarantee some compute time is assured
	//	after every PIO refresh cycle is completed.  Make this value bigger if you need lots of compute time, 
	//	smaller if your application is not compute intensive. Note: Larger values mean slower refresh rates 
	// (and possible flicker) when plotting >20K points.
	//
	#define CFG_TNSY_3_6_MinimumComputeTimeUs 2000	//Example: By setting this to 2000, you will guarantee at least 2 MS 
													//of compute time will be allocated during every refresh cycle
													//DO NOT set number to ZERO as TEENSY will appear to lockup and
													//be unresponsive when plotting large numbers of points
													
	//=========== PIO DAC Settling time and Unblank Pulse Width ============	
	//	F_CPU = TEENSY 3.x ONLY. These values fine tune UNBLANK timing and cope with the relatively slow DAC
	//	performance of the TEENSY 3.x processors.  These values can also help cope with performance limitations
	//	that may exist in 'slow O-scopes'. Delay values are specified in 'counts'.  The actual delays achieved
	//	vary depending on the CPU operating Frequency.  For this reason, the user may make individual adjustments 
	//	based on CPU operating frequency.  Note: Larger values will increase settling time/increase unblank time.
	//	Smaller values decreases settling time/unblank time.
	//
	//	Note: 	The values below have been set with Vref = 1.5V using a high speed HP1332A XYZ Monitor; 
	//			These may not be the optimum values when you have Vref = 3.3V or when using a lower speed scope!
	//
	#if defined(__SAM3X8E__)	//Defined for use with ARDUINO DUE processor (PIO mode only, Not used for DMA!), 84 Mhz CPU Speed		
		#define CFG_PioSmallSettleCount 0	//Defines Small Step Settling time delay (where n=Num of NOP instructions)
		#define CFG_PioLargeSettleCount 0	//Defines Large Step Settling time delay (where n=Num of NOP instructions)
		#define CFG_PioUnblankCount 0		//Defines UNBLANK Pulse Width (where n=Num of NOP instructions)
		#define CFG_NoSettlingTimeReqd 200	//Defines Small Step Breakpoint (in DAC counts)
	#endif
	
	#if (F_CPU > 216000000)		//TEENSY - More than 216 Mhz CPU Speed (ie: 240 Mhz)
		#define CFG_PioSmallSettleCount 25	//Defines Small Step Settling time delay (where n=Num of NOP instructions)
		#define CFG_PioLargeSettleCount 50	//Defines Large Step Settling time delay (where n=Num of NOP instructions)
		#define CFG_PioUnblankCount 5		//Defines UNBLANK Pulse Width (where n=Num of NOP instructions)
		#define CFG_NoSettlingTimeReqd 1000	//Defines Small Step Breakpoint (in DAC counts)
	#endif
	#if (F_CPU == 216000000)	//TEENSY - 216 Mhz CPU Speed
		#define CFG_PioSmallSettleCount 18
		#define CFG_PioLargeSettleCount 40
		#define CFG_PioUnblankCount 5
		#define CFG_NoSettlingTimeReqd 1000
	#endif
	#if (F_CPU == 192000000)	//TEENSY - 192 Mhz CPU Speed
		#define CFG_PioSmallSettleCount 14
		#define CFG_PioLargeSettleCount 35
		#define CFG_PioUnblankCount 5
		#define CFG_NoSettlingTimeReqd 1000
	#endif
	#if (F_CPU == 180000000)	//TEENSY - 180 Mhz CPU Speed
		#define CFG_PioSmallSettleCount 16
		#define CFG_PioLargeSettleCount 30
		#define CFG_PioUnblankCount 4
		#define CFG_NoSettlingTimeReqd 1000
	#endif	
	#if (F_CPU == 168000000)	//TEENSY - 168 Mhz CPU Speed
		#define CFG_PioSmallSettleCount 15
		#define CFG_PioLargeSettleCount 29
		#define CFG_PioUnblankCount 3
		#define CFG_NoSettlingTimeReqd 1000
	#endif	
	#if (F_CPU == 144000000)	//TEENSY - 144 Mhz CPU Speed
		#define CFG_PioSmallSettleCount 30
		#define CFG_PioLargeSettleCount 24
		#define CFG_PioUnblankCount 2
		#define CFG_NoSettlingTimeReqd 1000
	#endif
		#if (F_CPU == 120000000)	//120 Mhz CPU Speed
		#define CFG_PioSmallSettleCount 20
		#define CFG_PioLargeSettleCount 18
		#define CFG_PioUnblankCount 1
		#define CFG_NoSettlingTimeReqd 1000
	#endif
	#if (F_CPU == 96000000)			//TEENSY - 96 Mhz CPU Speed
		#define CFG_PioSmallSettleCount 14
		#define CFG_PioLargeSettleCount 16
		#define CFG_PioUnblankCount 0
		#define CFG_NoSettlingTimeReqd 1000
	#endif
	#if (F_CPU == 72000000)			//TEENSY - 72 Mhz CPU Speed
		#define CFG_PioSmallSettleCount 8
		#define CFG_PioLargeSettleCount 8
		#define CFG_PioUnblankCount 0
		#define CFG_NoSettlingTimeReqd 1000
	#endif
	#if (F_CPU == 48000000)			//TEENSY - 48 Mhz CPU Speed
		#define CFG_PioSmallSettleCount 0
		#define CFG_PioLargeSettleCount 0
		#define CFG_PioUnblankCount 0
		#define CFG_NoSettlingTimeReqd 1000
	#endif
	#if (F_CPU == 24000000)			//TEENSY - 24 Mhz CPU Speed
		#define CFG_PioSmallSettleCount 0
		#define CFG_PioLargeSettleCount 0
		#define CFG_PioUnblankCount 0
		#define CFG_NoSettlingTimeReqd 1000
	#endif
	#if (F_CPU < 24000000)			//TEENSY - Less than 24 Mhz CPU Speed
		#define CFG_PioSmallSettleCount 0
		#define CFG_PioLargeSettleCount 0
		#define CFG_PioUnblankCount 0
		#define CFG_NoSettlingTimeReqd 1000
	#endif
	
	//=========== TEENSY 3.x Real Time Clock Settings ===================
	//	
	#if defined(__MK66FX1M0__)
		#define CFG_InitClockFromTNSY_RTC	true	//Set true if you want initialize Arduino TIME software with time
													//Set false if RTC battery not present or you do NOT want to use TNSY_RTC values
	#else
		#define CFG_InitClockFromTNSY_RTC	false	//If not using TEENSY, this must be set false.
	#endif

#endif	//End XYsConfig