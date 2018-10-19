/*
 CRT_SCOPE_CLOCK.ino  E.Andrews  Brookfield, WI USA

 This program is an EXAMPLE program to demonstrate and test the
 Arduino Graphics Interface (AGI) hardware.  This hardware provides
 an interface between an Arduino TEENSY 3.6 Or DUE processor and an
 Oscillocope that permits XY drawing & display of graphics objects
 onto the scope CRT.
 
 Hardware and software details can be are found in the EXTRA folder.
 
 ARDUINO DUE:  The Arduino DUE version of this Arduino Graphics Interface project, initially 
 just called "AGI", is further described in the February 2018 and March 2018 
 editions of the electronics magazine "Nuts & Volts"
 
 TEENSY 3.6: The TEENSY 3.6 version of this project, called "TGI" (Teensy Graphics Interface) is
 featured in the November/December 2018 edition of the electronics magazine "Nuts & Volts"
 
 The software drivers and support library, called XYscope, uses compiler switches to 
 supports both the DUE and TEENSY hardware platforms.
 
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.

 Note: This program was written and tested using Arduino IDE, Version 1.8.5
       Modifications may be required if using other platforms or versions of the IDE.
	   Also, you must select processor (Arduino DUE OR TEENSY 3.6) appropriately 
	   for correct and valid compilation.
 
 20170311 Ver  0.00	E.Andrews	Development begins
 20170826 Ver  1.00	E.Andrews	All key functions working, General Clean up
 20171122 Ver  1.58 E.Andrews	Clean up and tuning; Reset master revision level to 1.58
 20180103 Ver  1.59 E.Andrews   Added clarifying comments throughout code base
 20180103 Ver  1.60 E.Andrews   Adapt to Lib-structure, enhance comments, & test to ensure
 								 that package can be compiled within ARDUINO-IDE Ver 1.8.2 as a library
 20180425 Ver  2.00 E.Andrews   Adapt to AGI LIBRARY to also work with TEENSY 3.6 PIO interface
								  using just TEENSY DAC0(X), DAC1(Y), & D2 (UNBLANK) signals
 20180521 Ver  2.00 E.Andrews   Solid AGI PIO Driver and enhanced test routines for TEENSY 3.6 interface
								Implemented XYscopeConfig.h to define key driver parameters.
								DHRYSTONE benchmark include to evaluate refresh load on compute performance.
 20180612 Ver  2.01	E.Andrews	Add "Lunar Lander" into Games simulation --- Code Note, Added but not functional!
 20180705 Ver  3.00	E.Andrews	Release with CRT CLOCK functionality; Removed non-working Lunar Lander code.
 20180921 Ver  3.10	E.Andrews	Adding EEPROM storage & retrieval of HW timing & setup parameters
 20180928 Ver  3.20	E.Andrews	Added 4 push buttons and auto-startup into clock mode
 20181005 Ver  3.21	E.Andrews	Fine tuning to verify that it will run on both TEENSY and DUE platforms.
								Added more test patterns and packed up to accompany Nuts and Volts article.
								Note: DUE doesn't have a usable RTC (ie: No Battery Backup) so all RTC code
								is EXCLUDED from COMPILE.

 This mainline requires the use of XYscope library, the set of drivers
 that manage and drive the Arduino Graphics Interface (AGI) hardware 
 and TEENSY Graphics Interface (TGI) hardware.

 HISTORY: XYscope libraries were first created by E. Andrews (March-September 2017) to enable
 an Arduino DUE to drive a X-Y CRT such as an Oscilloscope or XYZ Monitor.  This base
 code set was updated in 2018 to work with a new TEENSY 3.6 Graphics Interface (TGI) board.
 
 COMPATIBILITY: Every attempt has been made to ensure compatibility within the AGI library
 so that one set of library code can drive EITHER the original DUE AGI hardware, or the new
 TEENSY TGI hardware.  As such, you will find many conditional compile #if... statements 
 are used so that one code base will run on either processor.  
 
 DIFFERENCES: There are some core hardware differences between the DUE and TEENSY 3.6 that
 prevent all CRT_SCOPE_CLOCK mainline features (such as the RTC and EEPROM)to be supported
 available on both platforms; It is hoped that the the extensive comments within the source
 code will help the programmer to understand and cope with these differences.

 -------------------------------------------------------------------------------------------
 This program requires the following XYscope library routines to function properly

		
	CRT_SCOPE_CLOCK.ino		The main program file (this file!)
	dhry.h					This is required when using DHRYSTONE performance measurement routines
	dhry.ccp				Note: dhry.h and dhry.cpp needs to be in the SAME DIRECTORY as CRT_SCOPE_CLOCK
 
	Standard Arduino Library	Part of standard IDE install.
		<Arduino.h>			Provided as part of the Arduino IDE	
		<TimeLib.h>			This library provides the basic clock functionality. This library is driven off of the 
							internal arduino millis() routines and doesn't require an RTC to function.
							If using a DUE: you can get this library through Arduino.cc  
							If using the TEENSY: You MUST use the TimeLib that's included w/TEENSY support install or TEENSY RTC don't work.
							
		<Bounce2.h>			library for push button debouncing. Found at Arduiono.cc or here: https://github.com/thomasfredericks/Bounce2
		
	The TEENSY/DUE XYscope Library which consists of:
		<XYscope.h>			Include file
							Other files Files which are INSIDE of XYscope Library are:
							==================  ===========================================================================
							XYscope.cpp			All graphics control and plotting routines
							VectorFontROM.h		FONT file used by XYscope based on arcs and vectors.
							HersheyFontROM.h	FONT file used by XYscope based on short straight lines.
												Use option 13P to see both fonts side by side.
							XYscopeConfig.h		This file defines startup DAC settling time values, & pin out assignments
												You can change file settings to customize the program as desired.
	
	
	Teensy Libraries		Required and added ONLY if using Arduino TEENSY 3.6 ProcessorThese
							Note: These libraries are automatically installed when you add TEENSY support into your IDE from Arduino.cc
							
		EEPROM.h			Teensy EEPROM support functions
		
		TimeLib.h			Teensy3.x Time library functions [provides support for Teensy 3.6 RTC functionality] 
		

	
	Due Libraries			Required and added ONLY if using Arduino DUE processor
		<DueTimer.h>		Timer library to drive refresh routines
 ------------------------------------------------------------------------------------------------------------------------------	
 */
 
//################# BEGIN CONFIGURATION, GLOBAL VARIABLES, GLOBAL CONSTANTS SECTION vvvvvvvvvvvvvvvvvvvvvvv
char startMsg[] = "CRT_SCOPE_CLOCK(3.21) ";	//Program Revision Text

#define CompileDate __DATE__
#define CompileTime __TIME__
char CompileDateStamp[] = CompileDate;	//This is how we get a compile date and time stamp into the program
char CompileTimeStamp[] = CompileTime;

//Global Variables & Constants

//...............ARDUINO DUE MENU PREFERENCES ..............................................................
bool showDueOptions=true;			//If set true, and we are using a DUE, show DMA clock and other timing setup options in main menu display
									//If set false, DMA clock and other timing setup options will be excluded from main menu display
//...............CLOCK DISPLAY PREFERENCES ..............................................................

//Define GLOBAL operator input variables (for use when interfacing through IDE Serial Monitor)
const int _inputEmpty=-999999;		//This is the default "no data entered" value put into the InputInteger[ ] array
const int _maxInputNumbers=5;
int InputInteger[_maxInputNumbers];	//Input Integer array
float InputFloat[_maxInputNumbers];	//Input Floating Point array
int InputPtr,InputDecPt;			//Define array points for Int and Float arrays
bool InputNegFlag;					//This remembers if input is positive or negative

#define LED_PIN 13					//This definition value is the same for both DUE and TEENSY

//...............CLOCK DISPLAY PREFERENCES ..............................................................
bool Gbl_Show_Digital_Time = true;	//Set to false to suppress digital display of time (ie: "11:27") on clock face 
bool Gbl_Show_DayOfWeek=true;		//Set to false to suppress day of week (ie: "Wednesday") on clock face
bool Gbl_Show_Date=true;			//Set to false to suppress date (ie: "November 6,2018") on clock face
bool Gbl_StartUp_In_Clock_Mode = true;	//Set this 'true' to auto=start into CLOCK MODE.
										//Set this to 'false' set to false to start TEST MODE (Serial Monitor)
//...............CLOCK DISPLAY PREFERENCES ..............................................................
									
//================ DEFINE GLOBAL CLOCK ROUTINES ==========================================//
//DEFINE Global (Gbl) Variables used to draw the clock features
//	These global variables are used to control CLOCK coordinates, size, etc.										
float Gbl_X_Center, Gbl_Y_Center, Gbl_X_FaceRadius, Gbl_Y_FaceRadius, Gbl_Center_Radius, Gbl_MinHubCenterRadius;
float Gbl_Hour_HandLength,Gbl_Hour_EndRadius, Gbl_Hour_End_X_Center, Gbl_Hour_End_Y_Center;
float Gbl_Min_HandLength, Gbl_Min_EndRadius, Gbl_Min_End_X_Center, Gbl_Min_End_Y_Center;
float Gbl_Sec_HandLength, Gbl_Sec_End_X, Gbl_Sec_End_Y;
//These are used for storing XYlist pointers to the start of the HR, Min, and Sec hands
int Gbl_HR_ListPtr, Gbl_MIN_ListPtr, Gbl_SEC_ListPtr;

//=============== DEFINE Global Variables Used by P25_BounceBallPlot Routine ==============//
float bb_NewGravity = 9.2; 
float bb_NewRestitution=.75;
float bb_NewSpeed=1.;	

//.........BEGIN INCLUDES....................................................................
#include <Arduino.h>	//Provided as part of the Arduino IDE

#include "dhry.h"		//This is required when using DHRYSTONE performance measurement routines
						//Note: dhry.h and dhry.cpp needs to be in the SAME DIRECTORY as CRT_SCOPE_CLOCK
						
#include <Bounce2.h>	//Include the Bounce2 library found here: https://github.com/thomasfredericks/Bounce2
						//This library only required when using 4-button control panel
						//You can comment out Bounce2 include if you aren't using the 4-button control panel


#if defined(__SAM3X8E__)    //Here's the #includes needed when running a DUE processor   
	#include <DueTimer.h>	//Timer library for DUE; download this library from the Arduino.org site
							//Timer library is also available from author at https://github.com/ivanseidel/DueTimer
#endif

#include <TimeLib.h>		//This library is used for all CLOCK functions
							//While TimeLib should work for both DUE and TEENSY, DUE needs an external RTC circuit
							//and additional RTC software to retain & recall time from RTC when power is interrupted.

time_t t;					//Define 't' as a 'time_t' data type (time_t data structure is defined in TimeLib.h)
							
#if defined(__MK66FX1M0__)	//Here's the #includes needed to support the RTC, EEPROM & TEENSY 3.6 counter/timers
		IntervalTimer RefreshTimer;		//Create an instance of the interval timer to control refresh events
										//This must be done in MAIN CODE block
		//===BEGIN: DEFINE RTC_CODE FLAG ===================
		#define TeensyRTC_PRESENT	//Tell compiler its OKAY to include TEENSY RTC related coded [Not valid if we're on the DUE platform!]
		//===END: DEFINE RTC_CODE FLAG =====================

		//============= BEGIN TEENSY EEPROM_LIBRARY & SETUP ==============
		#include <EEPROM.h>		//Add TEENSY EEPROM library functions; If we're on the DUE platform, skip it!

		//	Define structure for timing-setup data that is held in EEPROM; If we're on the DUE platform, skip it!
		struct EEpromData_1 {
			float DataStructRev;			//Data structure revision designation
			int	PioSmallSettleCount; 		//Defines Small Step Settling time delay (where n=Num of NOP instructions)
			int	PioLargeSettleCount; 		//Defines Large Step Settling time delay (where n=Num of NOP instructions)
			int PioBreakpointCount;			//Defines Small-to-Large Step Breakpoint value (where n is in DAC counts, range: 0-4095)
			int	PioUnblankCount;			//Defines UNBLANK Pulse Width (where n=Num of NOP instructions)
		};
				//	Define structure for timing-setup data that is held in EEPROM; If we're on the DUE platform, skip it!
		struct EEpromData_2 {
			float DataStructRev;			//Data structure revision designation
			int	PioSmallSettleCount; 		//Defines Small Step Settling time delay (where n=Num of NOP instructions)
			int	PioLargeSettleCount; 		//Defines Large Step Settling time delay (where n=Num of NOP instructions)
			int PioBreakpointCount;			//Defines Small-to-Large Step Breakpoint value (where n is in DAC counts, range: 0-4095)
			int	PioUnblankCount;			//Defines UNBLANK Pulse Width (where n=Num of NOP instructions)
			bool Show_Digital_Time;			//Set to false to suppress digital display of time (ie: "11:27") on clock face 
			bool Show_DayOfWeek;			//Set to false to suppress day of week (ie: "Wednesday") on clock face
			bool Show_Date;					//Set to false to suppress date (ie: "November 6,2018") on clock face
			bool StartUp_In_Clock_Mode;		//Set to true to auto-start in clock mode, set false to auto-start in SERIAL MONITOR mode
		};
		//EEpromData_1 GBL_EE_SetupData;		//Declare Structure and provide global (GBL) access to EEprom setup data-set (Format 1)
		EEpromData_2 GBL_EE_SetupData;		//Declare Structure and provide global (GBL) access to EEprom setup data-set (Format 2)
		
		//Set starting address for EEpromData array
		int eeStartAdr=0;
		const bool PowerUpInitializeFromEEPROM = true;	
											//Set to true and program will startup using EEPROM stored timing Data-Set
											//Set to false and program will startup using timing Data-Set found in XYscopeConfig.h
											
		#define TeensyEEPROM_PRESENT		//Tell compiler its OKAY to include TEENSY EEPROM related coded; 
											//If we're on the DUE platform, skip it!
		//============ END TEENSY EEPROM_LIBRARY & SETUP ================

#endif

						
//UNCOMMENT ONE OF THE FOLLOWING TWO INCLUDE STATEMENTS TO SET THE PATH TO THE LIBRARY PROGRAMS
//	(Download this library from GitHub & N&V)

#include <XYscope.h>	//UNCOMMENT THIS LINE (NORMAL) if XYscope package is installed normally in the Arduino\Library\XYscope folder.
						
						
//#include "XYscope.h"	//UNCOMMENT THIS LINE (Not Normal) if XYscope routines are installed within the same folder as your mainline code.


//.........END INCLUDES....................................................................

//.........DEFINE 4-BUTTON CONTROL PANEL - Optional Push Buttons for easier clock-set & timing variables adjustment
//	These buttons use <bounce2.h)> library
//  [Comment-out next four lines if no Push buttons are connected so that PB code will be omitted from build]
#define DecrPB_Pin 4	//    +-------+ +-------+ +-------+ +-------+ 
#define IncrPB_Pin 5	//    | [PB1] | | [PB2] | | [PB3] | | [PB4] |
#define NxtPB_Pin 6		//    | DECR  | | INCR  | |  NEXT | | Enter | 
#define EnterPB_Pin 7	//    +-------+ +-------+ +-------+ +-------+

//.........Declare GLOBAL PUSH BUTTON variables
//	These are Global PUSH BUTTON variables that enable the button scanning routines to be called
//	from anywhere in the mainline or it's subroutines.  Note: While defined, values are NOT USED
// 	if PB_pin_numbers are not defined.

unsigned long PB_RepeatMs;
unsigned long PB_LastPushedMs;						//Define "LastPushedMs" TimeStamp
int PB_EndOfListPtr;
 
const long PB_RepeatInitialDelayMs=900;	//Initial Delay in milliseconds (900 means an initial delay of 0.9 Second before repeat begins)
const long PB_RepeatRatePeriodMs=330;		//Repeat Rate Period in milliseconds (330 means a repeat rate of  about 3 x per second)

int PB_Last_IncrBtn_value=HIGH, PB_IncrBtn_value=HIGH;
int PB_IncrBtn_UpEdge=LOW;
int PB_IncrBtn_DownEdge=LOW;

int PB_Last_DecrBtn_value=HIGH, PB_DecrBtn_value=HIGH; 
int PB_DecrBtn_UpEdge=LOW; 
int PB_DecrBtn_DownEdge=LOW;

int PB_Last_NxtBtn_value=HIGH, PB_NxtBtn_value=HIGH; 
int PB_NxtBtn_UpEdge=LOW; 
int PB_NxtBtn_DownEdge=LOW;

int PB_Last_EnterBtn_value=HIGH, PB_EnterBtn_value=HIGH; 
int PB_EnterBtn_UpEdge=LOW; 
int PB_EnterBtn_DownEdge=LOW;
//.........END PUSH BUTTON Variables definitions

//Initialize PushButton debounce routines ONLY IF Pins are defined
#if defined (IncrPB_Pin)
	// Instantiate a Bounce object
	Bounce debounce_IncrBtn = Bounce(); 
#endif
#if defined (DecrPB_Pin)
	// Instantiate a Bounce object
	Bounce debounce_DecrBtn = Bounce(); 
#endif
#if defined (NxtPB_Pin)
	// Instantiate a Bounce object
	Bounce debounce_NxtBtn = Bounce(); 
#endif
#if defined (EnterPB_Pin)
	// Instantiate a Bounce object
	Bounce debounce_EnterBtn = Bounce(); 
#endif

//.........END 4-BUTTON CONTROL PANEL............................................................



XYscope XYscope;

//################# END CONFIGURATION, GLOBAL VARIABLES, GLOBAL CONSTANTS SECTION ^^^^^^^^^^^^^^^^^^^^^^^^^^^^


// 	+---------Begin Critical Interrupt Service Routines ------------+
//	|  These routines MUST be declared as shown at the top of the	|
//	|      user's main line code for all XYscope Projects!			|
// 	+---------------------------------------------------------------+
//	|																|
//	V																V

void DACC_Handler(void) {	//Req'd for DUE ONLY
	//	DACC_Handler Interrupt Service Routine. This routine
	//	provides a 'wrapper' to link the AVR DAC INTERRUPT
	//	to the 'XYscope-class' ISR routine.
	XYscope.dacHandler();	//Link the AVR DAC ISR/IRQ to the XYscope.
							//It is called whenever the DMA controller
							//'DAC_ready_for_More_data' event occurs
}

void paintCrt_ISR(void) {	//Used for both DUE an TEENSY implementation
	//	paintCrtISR  Interrupt Service Routine. This routine
	//	provides a 'wrapper' to link Timer3.AttachedInterrupt()
	//	function to the 'XYscope-class' ISR routine.
	
	//------------------------------------------------------------
	//	DUE CODE BLOCK 
	//------------------------------------------------------------
	//
	#if defined(__SAM3X8E__) 
		#if defined(CFG_DUE_Use_DMA)				//Set this CFG_... variable in XYscopeConfig.h file
			XYscope.initiateDacDma();				//Do screen paint using DMA transfer methods
		#else
			XYscope.initiateDuePioScreenPaint();	//Do screen paint using PIO methods
		#endif
	#endif
	//------------------------------------------------------------
	//	TEENSY CODE BLOCK 
	//------------------------------------------------------------	
	//
	#if defined(__MK66FX1M0__)
		XYscope.initiatePioScreenPaint();
	#endif
}

//------------------------------------------------------------
//	TEENSY CODE BLOCK 
//------------------------------------------------------------	
//
#if defined(__MK66FX1M0__) 

	void ChangeTeensyRefreshInterval(long duration_us){	//Req'd for TEENSY ONLY
		//This line links the TEENSY RefreshTimer to the XYscope library
		RefreshTimer.update(duration_us);
		
	}
#endif
//	V																V
//	|																|
// 	+---------- END Critical Interrupt Service Routines ------------+

//	Define/initialize critical global constants and variables

char LastKeyPressed=' ';			//Defines last command key entered through the Serial Monitor interface

double dacClkRateHz, dacClkRateKHz;	//These are DUE variables

int EndOfSetup_Ptr;
double TimeForRefresh;

// The following 5-lines are used for RAM monitoring routines--These only work on DUE!
#include <malloc.h>				//Required for DUE "ShowMemory" RAM-usage monitoring routines
extern char _end;
extern "C" char *sbrk(int i);
char *ramstart = (char *) 0x20070000;
char *ramend = (char *) 0x20088000;






 
 void setup() {
	// Mainline program SETUP routine.
	//
	//	Passed Parameters	NONE
	//
	//	Returns: NOTHING
	//
	//	20170708 Ver 0.0	E.Andrews	First cut
	//						(Updated throughout development cycle without version change)
	//	20180501 Ver 1.0	E.Andrews	Updated for use with DUE and TEENSY - Many Drhystone metrics and many TEENSY options added
	//						(Updated throughout development cycle without version change)
	//	20181010 Ver 2.0	E.Andrews	Cleaned up comments and deleted unneeded debug print statements.
	//	
	
	pinMode(LED_PIN,OUTPUT);	//Init LED in case we want to use this for debug purposes (Same pin# for DUE and TEENSY!)
	digitalWrite(LED_PIN,LOW);	//Init LED OFF

	//Serial Monitor must be setup to agree with this initialization.
	//  [X] Autoscroll
	//  "No Line Ending"
	//  "115200 baud"
	Serial.begin(115200);	

	long unsigned serialWakeupWaitUntillMs=millis()+3000;
	bool serialPortNotReady=true;
	while (serialPortNotReady) {

		digitalWrite(LED_PIN,LOW);		//Turn board LED OFF
		//	Give processor time to fully initialize serial port.  This happens almost immediately
		//	if IDE DEGUB window is open.  If IDE DEBUG WINDOW is NOT OPEN, we wait 3 seconds and
		//	then exit this wakeup wait loop and go on without verifying serial port.
		if (millis()>serialWakeupWaitUntillMs) {
			//Stop Waiting and 'move on'!
			serialPortNotReady=false;
		}
	}
	//Send startup messages out to Serial monitor port...
	Serial.println("");
	Serial.print(startMsg);	
	//Now print Date and Time code was compiled...
	Serial.print(" (Compiled Date-Time Stamp: ");	Serial.print(CompileDateStamp);	Serial.print(" ");	Serial.print(CompileTimeStamp);
	Serial.println(")");

	
	//===== TEENSY RTC_CODE START =================
	#if defined (TeensyRTC_PRESENT)
		// Initialize Arduino Time library with Teensy 3.6's RTC (if present)
		

		if (CFG_InitClockFromTNSY_RTC){	//NOTE: This variable is defined as true/false within XYscopeConfig.h file

			setTime(Teensy3Clock.get());
			setSyncProvider(getTeensy3Time);	//Fetch value current time/date from TNSY RTC
			setSyncInterval(60);				//Resync to TEENSY RTC every 60 seconds
			//Send startup Diag Message to Monitor
			Serial.println (" Startup: Clock Data retrieved from Teensy RTC!");
		}
		if (timeStatus()!= timeSet) {
			//Send startup Diag ERROR Message to Monitor
			Serial.println(" Startup CLOCK ERROR: Not set by RTC");
		  }
		else {
			//Send startup Diag STATUS Message to Monitor
			Serial.println(" Startup CLOCK OKAY:  Clock was set by RTC");
		}

	#else
		Serial.println("WARNING: Program was compiled without TNSY_RTC...");
	#endif
	//===== TEENSY RTC_CODE END ===================


	//------------------------------------------------------------
	//	DUE STARTUP CODE BLOCK 
	//------------------------------------------------------------
	//
	#if defined(__SAM3X8E__)       
		//For Arduino DUE only: Timer3 is used as the CRT refresh timer.  This timer is setup 
		//inside of XYscope.begin( ),  however, paintCRT_ISR must be "attached" to timer 3 here.
		//To be properly linked to the refresh-screen XYscope interrupt service routine, it must 
		//be linked in the Arduino setup() code as follows:
		Timer3.attachInterrupt(paintCrt_ISR);
		XYscope.begin();	//Note1: It is best to precede begin() by attachedInterrupt() or system may hang during startup
							//Note2: You can specify DmaFreq as a calling parameter in begin().  
							//		 When not spec'd during call, default will be: DueDmaFreq=800000 (800kHz).
	#endif
	
	//------------------------------------------------------------
	//	TEENSY STARTUP CODE BLOCK 
	//------------------------------------------------------------	
	//
	#if defined(__MK66FX1M0__)
		//For TEENSY 3.6 only: TEENSY IntervalTimer functions are used as the CRT refresh timer.  
		//	First, we create and an instance of the IntervalTimer called "RefreshTimer()".
		//	Then we must attach the paintCRT_ISR routine to the timer and initialize it to run 
		//	at the defined minimum value
		//
		
		//TEENSY PLATFORM - Turn OFF DUE options on menu displays
		showDueOptions=false;		
		
		XYscope.begin();					//TEENSY startup
		RefreshTimer.begin(paintCrt_ISR,XYscope.CrtMinRefresh_us);	//CrtMinRefresh_us defined in XYscope.h
		
		//Timing Data-Set initialization
		if(PowerUpInitializeFromEEPROM) {	//Value is set true/false in this file (~line 225).  
											//Set false and startup params from XYscopeConfig.h will be used.
											//Set true and startup params as stored in TEENSY 3.6 will be used.
			InitializeTiming_To_EEpromDataSet();
			//Diag printouts to Montior
			Serial.println ("COMPLETED: Read & Initialization from EEPROM ");
			if(is_ActiveTiming_SameAs_EEPROM())Serial.println (" SUCCESSFUL!"); else Serial.println (" FAILED!");
		}else Serial.println (" NOTE: Configured to SKIP Read and Initialization from EEPROM");

	#endif
	//------------------------------------------------------------
	//	END TEENSY CODE INIT
	//------------------------------------------------------------	
	
	
	//------------------------------------------------------------
	//	Initialize PUSH BUTTONS CODE 
	//  (Include code ONLY WHEN PB_Pins are defined, other wise SKIP it!) 
	//------------------------------------------------------------	
	//

	#if defined (IncrPB_Pin)	
		// Setup the button with an internal pull-up :
		pinMode(IncrPB_Pin,INPUT_PULLUP);
		// After setting up the button, setup the Bounce instance :
		debounce_IncrBtn.attach(IncrPB_Pin);
		debounce_IncrBtn.interval(5); // interval in ms
		//Setup the LED 
		pinMode(LED_PIN,OUTPUT);
		PB_EndOfListPtr=0;
	#endif
	
	#if defined (DecrPB_Pin)
		// Setup the button with an internal pull-up :
		pinMode(DecrPB_Pin,INPUT_PULLUP);
		// After setting up the button, setup the Bounce instance :
		debounce_DecrBtn.attach(DecrPB_Pin);
		debounce_DecrBtn.interval(5); // interval in ms
		//Setup the LED :
		pinMode(LED_PIN,OUTPUT);
		PB_EndOfListPtr=0;
	#endif	
	
	#if defined (NxtPB_Pin)
		// Setup the button with an internal pull-up :
		pinMode(NxtPB_Pin,INPUT_PULLUP);
		// After setting up the button, setup the Bounce instance :
		debounce_NxtBtn.attach(NxtPB_Pin);
		debounce_NxtBtn.interval(5); // interval in ms
		//Setup the LED :
		pinMode(LED_PIN,OUTPUT);
		PB_EndOfListPtr=0;
	#endif
	
	#if defined (EnterPB_Pin)
		// Setup the button with an internal pull-up :
		pinMode(EnterPB_Pin,INPUT_PULLUP);
		// After setting up the button, setup the Bounce instance :
		debounce_EnterBtn.attach(EnterPB_Pin);
		debounce_EnterBtn.interval(5); // interval in ms
		//Setup the LED :
		pinMode(LED_PIN,OUTPUT);
		PB_EndOfListPtr=0;
	#endif
	//------------------------------------------------------------
	//	END PUSH BUTTON SETUP
	//------------------------------------------------------------	
	//	

	
	//Here is just some stuff to paint onto CRT at startup
	//v----------BEGIN SETUP SPLASH SCREEN ---------------v
	PowerUpSplash();
	
	//Send option menu out to PC via Serial.print
	Serial.println();
	Print_CRT_Scope_Main_Menu();
	PrintStatsToConsole();
	ClearOperInputArray();

	
	//................ GO DIRECTLY INTO CLOCK MODE IF Gbl_StartUp_In_Clock_Mode=TRUE ....................
	if(Gbl_StartUp_In_Clock_Mode){	//If configured true (~line# 150 of this file), auto-start up in CLOCK MODE!
		delay(2000);			//Pause so oper can see rev numbers, etc...
		ClearOperInputArray();
		ResetPushButtons();
		P23_BeginClockMode();	//We will stay in this loop until opr pushes a key on serial monitor keyboard.
		ClearOperInputArray();	
	}
	//................ END CLOCK MODE - Go on to run TEST MODE (Serial Monitor Intfc)...................

}


void loop() {	
	// Begin CRT_SCOPE "NON-CLOCK MODE" Test and Configure Mainline code!
	//
	//	Passed Parameters	NONE
	//
	//	Returns: NOTHING
	//
	//	20170708 Ver 0.0	E.Andrews	First cut
	//						(Updated throughout development cycle without further version change)
	//	20180518 Ver 2.0	E.Andrews	Major rewrite to accommodate a simpler menu structure & additional TEENSY options
	//						(Updated throughout development cycle without further version change)

	//Here is the start of the DEMO CONTROL panel, run via Serial.print link to PC...

	// This is main operator input decoder loop
	bool DataValid;

	ReadPushButtons();

	if (Serial.available()) {	//MAIN SERIAL MONTIOR DECODER
								//If serial monitor keyboard data is available, read and process it!
		char d = Serial.read();	//Fetch a character
		if (d!=0)LastKeyPressed=d;	//Capture and save input in LastKeyPressed - This is used for push button changes
		//=========================== BEGIN Main Serial Input Decoder Loop =============================
		switch (d) {	//Decode operator input received via the serial monitor link-up
			case ' ':	//Ignore "Spaces"
			break;
			
			case ';':
			case ',':	//Comma Separator...Just bump data entry pointer one locations
				// Verify that current variable is present and that we still have room for more 
				// Ignore ',' if we don't have room to accept more numbers
				// if (InputInteger[InputPtr] == _inputEmpty) InputInteger[InputPtr]=0;
				InputPtr=InputPtr+1;	//Increment InputPtr
				InputDecPt=0;
				InputNegFlag=false;		//Reset Neg Number flag
				if(InputPtr > _maxInputNumbers) InputPtr=_maxInputNumbers;	//Range limit InputPtr value
				
			break;
			
			case '-':	//For entry of negative numbers....
				//Set InputNumNegative flag
				if (InputNegFlag==false) {	// Find first 'negative sign'; ignore all others
					InputNegFlag=true;
					InputDecPt=0;
				}
			break;
			
			case '.':	//Decimal Point...Apparently we are entering a floating pt number..Set InputDecPt counter to 1
				if (InputDecPt ==0)InputDecPt=1;	//Only detect first decimal point...ignore erroneous extra dec point entries
			break;			
			
			case '0'...'9':	//Numeric input...accumulate and store it as an integer into the InputInteger[array]
							//Numeric input...accumulate and store it as a floating point number into into the InputFloat[array]
				if (InputInteger[InputPtr] ==_inputEmpty) {
					InputInteger[InputPtr]=0;
					InputFloat[InputPtr]=0.;
				}
				
				//Make additions to a positive number (if decimal point not present)...
				if (InputDecPt==0){	//Add new digit to the left of the decimal point
					InputInteger[InputPtr]=abs(InputInteger[InputPtr])*10+ (d-48);	//Remove ASCII offset and add it to the integer input Num
					InputFloat[InputPtr]=abs(InputFloat[InputPtr])*10+(d-48);	//Remove ASCII offset and add it to the float input Num
				}
				else{	//Add new digit to the right of the decimal point (Don't add more to the Integer input...)
					float Fraction;
					Fraction = float(d-48)*pow(0.1,InputDecPt);
					InputFloat[InputPtr]=abs(InputFloat[InputPtr])+Fraction;	//Remove ASCII offset and add it to the input Num
					InputDecPt++;
				}
				
				//Restore to negative if neg flag has been set
				if (InputNegFlag) {
					InputInteger[InputPtr]=-1*InputInteger[InputPtr];
					InputFloat[InputPtr]=-1.*InputFloat[InputPtr];
				}
			break;
			
 			case 'B': 	//See/Change BACK PORCH timing, for DUE ONLY. ignore otherwise
			case 'b': {	//See/Change BACK PORCH timing, for DUE Only, ignore otherwise.
				if (showDueOptions){	//Ignore this entry if we're not running on a DUE CPU
					//Detailed timing adjustment...you shouldn't normally need to use this options
					if (InputInteger[0] ==_inputEmpty){	//If no value entered, just show value

						Serial.print (F(" <- Current Back Porch Value = "));Serial.print(XYscope.backPorchBlankCount);Serial.println(F(" (Not Changed)"));
						if (XYscope.backPorchBlankCount<0) XYscope.backPorchBlankCount=0;
					}
					else;{
						if (InputInteger[0]>-1 && InputInteger[0]<201)	{
							XYscope.backPorchBlankCount=InputInteger[0];
							Serial.print (F(" <- New Back Porch Value = "));Serial.print(XYscope.backPorchBlankCount);Serial.println(F(" (Changed!)"));
						} else Serial.print (F(" <- Back Porch Value = "));Serial.print(XYscope.backPorchBlankCount);Serial.println(F(" (Entry Out of Range, Not Changed)"));
					}
				}
				ClearOperInputArray();
				break;
			}

			case 'C': 		//Change/Increment DMA Clock Frequency, for DUE Only, ignore otherwise.
			case 'c':	{	//Change/Increment DMA Clock Frequency, for DUE Only, ignore otherwise.
				if (showDueOptions){	//Ignore this entry if we're not running on a DUE CPU
					//INCREASE DMA Clock Frequency in 50 kHz steps (Limit max value to 1.2 Mhz)
					// Try different values to see how well your scope and setup perform.
					// Look for artifacts and defects in the graphics display, particularly when
					// graphics objects are spaced far apart.
					uint32_t New_XfrRateHz;
					if (InputInteger[0] ==_inputEmpty){	//If no value entered, just report the current value
						//Report current frequency
						Serial.print (F(" <- DMA Clock Freq = "));Serial.print(XYscope.DmaClkFreq_Hz / 1000);Serial.println(F(" Khz (Not Changed)"));						

					}
					else {
						if (InputInteger[0]>249 && InputInteger[0]<1801)	{
							New_XfrRateHz=InputInteger[0]*1000;
							XYscope.setDmaClockRate(New_XfrRateHz);
							Serial.print (F(" <- DMA Clock Freq = "));Serial.print(XYscope.DmaClkFreq_Hz / 1000);Serial.println(F(" Khz (Value Changed!)"));						
							
						} else {
							Serial.print (F(" <- DMA Clock Freq = "));Serial.print(XYscope.DmaClkFreq_Hz / 1000);Serial.println(F(" Khz (Entry Out of Range, Not Changed)"));
						}
					}
				}
				XYscope.autoSetRefreshTime();	//Only required for DUE; Ignored by TEENSY
				ClearOperInputArray();
				break;
			}
			
			case 'D':	//Display or Set date  
				DataValid = true;
				if (InputInteger[0]==_inputEmpty){	//Show current date if no parameters specified
					Serial.print("  <-Current TIME: ");
					SerialPrintTime();
					Serial.print (" (Not Changed)  ");
					SerialPrintDate(); Serial.print (" (Not Changed)  ");
					Serial.println();
				}
				else {	//Some input parameters detected...This must be a SET DATE command.  Look for Month, Day, Year
					//month
					if (InputInteger[0]>12 || InputInteger[0] <1) DataValid=false;	//Make sure Month is in range [1-12]
					//day
					if (InputInteger[1]>31 || InputInteger[1] <1) DataValid=false;	//Make sure Day is in range [1-31]	
					//year
					if ((InputInteger[2]>-1) && (InputInteger[2]<100))InputInteger[2]=InputInteger[2]+2000;	//Make 2 digit YR entry a 4 digit value
					if ((InputInteger[2]<2000) || (InputInteger[2] >3000)) DataValid=false;	//Make sure YR are in range [2000-3000]						
					
					if (DataValid){
						//Data is valid, Set System Clock
						//	Format is: setTime(int hr,int min,int sec,int day, int month, int yr);
						setTime(hour(),minute(),second(),InputInteger[1],InputInteger[0],InputInteger[2]);
						Serial.println("Current Time: ");
						SerialPrintTime();Serial.print (" (Not Changed)");
						Serial.print("    DATE set to: ");
						SerialPrintDate();
						//Data is valid, IF TEENSY, Set RTC as well						
						#if defined (TeensyRTC_PRESENT)
							Teensy3Clock.set(t); // set the Teensy RTC
							Serial.println ("   >>> TEENSY RTC has been Set! <<<");
						#endif
					}
					else{	//Bad parameters were detected, don't modify clock & Print Error Message
						Serial.println();
						Serial.print(" <--- ERROR, invalid Data Entered (Date Not Changed!) ");
					}
				}
				ClearOperInputArray();
			break;
			
			case 'd':	//Run Dhrystone test and print results to serial port.
				RunDhrystoneTest();
				ClearOperInputArray();
			break;

			
			#if defined (TeensyEEPROM_PRESENT)	//Option e/E only active for TEENSY when EEPROM is enabled
				case 'e':
				case 'E':	//Read-Write-Display EEPROM contents (Only TEENSY 3.6 ONLY)
					if (!showDueOptions){
						//OPTION 0 - Read EEprom & display data-set via serial port
						if (InputInteger[0]==_inputEmpty || InputInteger[0]==0){	
								Serial.print ("  Reading data-set from EEPROM...");
								Serial.print ("EEPROM Size: ");Serial.print(EEPROM.length()); Serial.println (" bytes");						
								EEPROM.get(eeStartAdr,GBL_EE_SetupData);	//Read data structure from EEpromData
								//Print retrieved data structure
								PrintEEpromDataToConsole();

								//for (int E_Adr = 0;E_Adr<16;E_Adr++){	//Debug code to dump EEPROM contents
								//	Serial.print("  EEPROM Adr:");Serial.print(E_Adr);Serial.print(" = ");
								//	Serial.println(EEPROM.read(E_Adr));
								//} 
								
					
						}
						//OPTION 1 - Initialize to values stored in EEPROM
						if (InputInteger[0]==1 ){	
								Serial.println ("  Initializing to EEPROM-stored timing values...");	
								InitializeTiming_To_EEpromDataSet();
								PrintEEpromDataToConsole();
								Serial.println("     +-- DONE Initializing from EEPROM!");	
						}
						//OPTION 10 - Save (Write) current Timing Dataset to EEprom
						if (InputInteger[0]==10 ){	//Oper wants to UPDATE (write) EEPROM with current data-set
								GBL_EE_SetupData.DataStructRev = 2.0;
								GBL_EE_SetupData.PioSmallSettleCount = XYscope.PioSmallSettleCount;
								GBL_EE_SetupData.PioLargeSettleCount= XYscope.PioLargeSettleCount;
								GBL_EE_SetupData.PioBreakpointCount = XYscope.NoSettlingTimeReqd;
								GBL_EE_SetupData.PioUnblankCount=XYscope.PioUnblankCount;
								GBL_EE_SetupData.Show_Digital_Time=Gbl_Show_Digital_Time;
								GBL_EE_SetupData.Show_DayOfWeek=Gbl_Show_DayOfWeek;
								GBL_EE_SetupData.Show_Date=Gbl_Show_Date;
								GBL_EE_SetupData.StartUp_In_Clock_Mode=Gbl_StartUp_In_Clock_Mode;
								EEPROM.put(eeStartAdr,GBL_EE_SetupData);	//Write data structure TO EEprom
								Serial.println ("  Saving Active-data-set to EEPROM...");
								PrintEEpromDataToConsole();
								Serial.println("     +-- DONE Writing to EEPROM!");	
						}
						if (InputInteger[0]==3 ){	//Oper wants to CLEAR eeprom data-set
								Serial.print ("  Clearing EEPROM Data Set Area...");
								//Reserved for Future function...Put code here!
								Serial.println ("...OPTION NOT IMPLEMENTED! (Nothing Done to EEPROM)");
								//Serial.println ("...DONE!");				
						}
					}
					else {
						//Emergency error condition...If you see this message TeensyEEPROM_PRESENT is set incorrectly
						Serial.println (" *** This Option NOT AVAILABLE for Arduino DUE Processor ***");
					}
					ClearOperInputArray();
				break;
			#endif
			
			case 'F': 	
			case 'f': 	//See or Change FRONT PORCH timing, for DUE Only, ignore otherwise.
				if (showDueOptions){	//Ignore this entry if we're not running on a DUE CPU
					//Detailed timing adjustment...you shouldn't normally need to use this options
					if (InputInteger[0] ==_inputEmpty){	//If no value entered, just report the current value
						Serial.print (F(" <- Front Porch Value = "));Serial.print(XYscope.frontPorchBlankCount);Serial.println(F(" (Not Changed)"));
						
					}
					else;{
						if (InputInteger[0]>-1 && InputInteger[0]<201)	{
							XYscope.frontPorchBlankCount=InputInteger[0];
							Serial.print (F(" <- Front Porch Value = "));Serial.print(XYscope.frontPorchBlankCount);Serial.println(F(" (Updated!)"));
						} 
						else Serial.print (F(" <- Front Porch Value = "));Serial.print(XYscope.frontPorchBlankCount);Serial.println(F(" (Not Changed, entered value out of range!)"));
					}
					
				}
				ClearOperInputArray();
			break;
			
			case 'G': 
			case 'g':	//Change Graphics Intensity Value
				if (InputInteger[0]==_inputEmpty){
					Serial.print(" <-Current Graphics Intensity is: ");
					Serial.print(XYscope.getGraphicsIntensity());
					Serial.println(" %");
				}
				else {
					if (InputInteger[0]>0 && InputInteger[0]<256){	//If number is in-range, set new value
						XYscope.setGraphicsIntensity(InputInteger[0]);
						Serial.print(" <-GRAPHICS intensity for NEW PLOTS set to: ");
						Serial.print(XYscope.getGraphicsIntensity());
						Serial.println(" %");
					}
					else{	//Number is out of range...Let oper know
						Serial.println (" <- ERROR: Value out of range (must be 1-255)");
					}
				}
				ClearOperInputArray();	
			break;
			 
			case 'H':
			case 'h':
			case '?': 	//Print HELP MENU & current XYlist memory usage stats to console
				clearMonitorTerminalScreen();
				Print_CRT_Scope_Main_Menu();	//Show Help Menu
				PrintStatsToConsole();	//Print current STATS to console
			break;
			
			case 'i':	//TEENSY PIO Timing info
			case 'I':	//TEENSY PIO Timing info - Post HW Info to CRT in upper LH corner
				PlotToCRT_HwSettings();	//Print selected timing data to CRT
				PrintStatsToConsole();	//Print current STATS to console
				ClearOperInputArray();
			break;
			
			case 'K':
			case 'k':	//Clear the display
				XYscope.plotClear();	//This really isn't needed; plotStart() also 'erases' the screen...
				XYscope.plotStart();
				clearMonitorTerminalScreen();
				Serial.println(" <-CLEAR CRT & MONITOR DISPLAY SCREEN...Done");Serial.println();
			
				Print_CRT_Scope_Main_Menu();	//Show Help Menu
				PrintStatsToConsole();	//Print current STATS to console

				PrintStatsToConsole();
				//ClearOperInputArray();
			break;
			
			case 'L':
			case 'l':	//Change PIO Threshold LIMIT count.
						//Only used for TEENSY & DUE_PIO modes.  Value is not used in DUE_DMA mode.
				//Increase LIMIT Count
				if (InputInteger[0]>-1 && InputInteger[0]<4095){	//If number is in-range, set new value
					XYscope.NoSettlingTimeReqd=InputInteger[0];
					Serial.print(" <-Big-Step Threshold LIMIT count changed to: ");
					Serial.println(XYscope.NoSettlingTimeReqd);
				}
				else {
					Serial.print(" <-Current Big-Step Threshold LIMIT count is: ");
					Serial.print(XYscope.NoSettlingTimeReqd);
					Serial.println (" (Not changed)");
				}
				delay(100);	//wait until changes ripple through before printing 'stats'
				//PrintStatsToConsole();
				ClearOperInputArray();
			break;

			case 'M':	//Toggle Active Font
					if (XYscope.getActiveFont() == _VectorFont) {
					XYscope.setActiveFont(_HersheyFont);
					Serial.println ( " <-Text Font for new TEXT PRINTS = HERSHEY" );
				}
				else {
					XYscope.setActiveFont(_VectorFont);
					Serial.println ( " <-Text Font for new TEXT PRINTS = VECTOR" );
				}
				ClearOperInputArray();
			
			break;
			case 'm': 	//Toggle Font Space mode back and forth between Mono and Proportional
				if (XYscope.getFontSpacing() == XYscope.mono) {
					XYscope.setFontSpacing(XYscope.prop);
					Serial.println ( " <-Text Spacing new TEXT PRINTS = PROPORTIONAL" );
				}
				else {
					XYscope.setFontSpacing(XYscope.mono);
					Serial.println ( " <-Text Spacing new TEXT PRINTS = MONO" );
				}
				ClearOperInputArray();
			break;

			case 'p':
			case 'P':	//Select and a Display a Test Pattern....
				switch (InputInteger[0]){
					case _inputEmpty:	//No value entered, just present the Pattern-SubMenu
						Print_CRT_Scope_TestPattern_Menu();
						ClearOperInputArray();
					break;
					
					case 0:	//XY Gain Setup Test Pattern
						P0_PlotOpAmpAdjustPattern();	//View Sine/Cosine waveform on an oscilloscope
						ClearOperInputArray();
					break;
					
					case 1:	//Centering Test Pattern
						P1_PlotCenteringPattern();		//Use this pattern for scope and gain adjustments
						ClearOperInputArray();
					break;
									
					case 2:	//Corner Dots Pattern
						P2_PeakToPeak_Dots();			//Not terribly useful; can use to see large-step signal ringing
						ClearOperInputArray();
					break;
					
					case 3:	//Stair Case Pattern (for SETTLING and UNBLANK setup)
						P3_PeakToPeak_Settle_Unblk_Adj();	//Really useful for setting settling times
						ClearOperInputArray();
					break;
					
					case 4:		//Vertical Peak-To-Peak Test Pattern
						P4_PeakToPeak_Vert();				//Useful for scoping Y signals to see large step Y-ringing
						ClearOperInputArray();
					break;
					
					case 5:		//Horizontal Peak-To-Peak Test Pattern
						P5_PeakToPeak_Horiz();				//Useful for scoping X signals to see large step X-ringing
						ClearOperInputArray();
					break;
					

					case 11:{	//Random Number Print Test
						bool UnderlineNO=false;				//Simple test of numeric text-print routines
						P12_PlotRandNums(UnderlineNO);
						ClearOperInputArray();
						break;
					}
					case 12:{	//Random Number Print Test w/alternate lines underlined
						bool UnderlineYES = true;			//Simple test of numeric text-print routines
						P12_PlotRandNums(UnderlineYES);
						ClearOperInputArray();
						break;
					}
					case 13:	//Toggles Text Set back and forth
						if(InputInteger[1]==0) XYscope.setActiveFont(0);
						if(InputInteger[1]==1) XYscope.setActiveFont(1);
						P13_PlotCharSet_1();

						if(XYscope.getActiveFont()==0) Serial.println(" <-DONE, Now using CharSet 0 (VectorFont)");
						if(XYscope.getActiveFont()==1) Serial.println(" <-DONE, Now using CharSet 1 (HersheyFont)");
						PrintStatsToConsole();
					break;
					
					case 14:	//Text Set #2 -- Random Chars
						//P14_PlotCharSet_2();			//Simple text printout test
						//Plot a char... 
						int CharHeight,CharToPlot;
						CharHeight=InputInteger[1];	//Character Height				
						CharToPlot=InputInteger[2];	//Ascii Char Code  To Plot
						if(XYscope.getActiveFont()==0) Serial.print(" <-Plotting TEXT using CharSet 0 (VectorFont)");
						if(XYscope.getActiveFont()==1) Serial.print(" <-Plotting TEXT using CharSet 1 (HersheyFont)");

						P14_ShowSingleChar(CharToPlot,CharHeight);  //if CharToPlot = _inputEmpty, Full Set will plot
																	//if CharHeight = _inputEmpty, Size will default to 3000
						Serial.println ( " ... DONE");
						ClearOperInputArray();
					break;
					
					case 15:	//Plot Random Points
						P15_PlotRandomPoints(InputInteger[1],InputInteger[2]);
						ClearOperInputArray();
					break;
					
					case 16:	//Plot Random Vectors
						P16_PlotRandomVectors(InputInteger[1],InputInteger[2]);
						ClearOperInputArray();
					break;
					
					case 17:	//Plot Random Rectangles
						P17_PlotRandomRectangles(InputInteger[1],InputInteger[2]);
						ClearOperInputArray();
						break;
					
					case 18:	//Plot Random Circles
					P18_PlotRandomCircles(InputInteger[1],InputInteger[2]);
						ClearOperInputArray();
					break;
					
					case 19:	//Plot Random Ellipses
						P19_PlotRandomEllipes(InputInteger[1],InputInteger[2]);
						ClearOperInputArray();
					break;
					
					case 20:	//Demo Plot: Arduino Logo
						P20_PlotAnimatedLogo();
						ClearOperInputArray();
					break;
					
					case 21:	//Demo Plot: AGI Coordinate System
						P21_SimCoordinateSys();
						ClearOperInputArray();
					break;
					
					case 22:	//Demo Plot: Sample Chart
						P22_SimGraph_0();
						ClearOperInputArray();
					break;
					
					case 23:	//Plot: GO TO CLOCK-MODE; stay there until key press
						P23_BeginClockMode();
						ClearOperInputArray();
					break;
					
					case 24:	//Demo Plot: PONG
						P24_PongDemo();
						ClearOperInputArray();
					break;

					case 25:	//Demo Plot: Bouncing Ball demo
						if (InputInteger[1] != _inputEmpty) bb_NewGravity = InputFloat[1];
						if (InputInteger[2] != _inputEmpty) {
							bb_NewRestitution=InputFloat[2];
							if (bb_NewRestitution>1.0) bb_NewRestitution=bb_NewRestitution*.01;	//Correct bad op input if needed
						}
						if (InputInteger[3] != _inputEmpty) bb_NewSpeed=InputFloat[3];
						Serial.println();
						Serial.print("   bb_NewGravity=");Serial.println(bb_NewGravity);
						Serial.print("      InputDecPt=");Serial.print(InputDecPt);Serial.print(", InputNegFlag=");Serial.println(InputNegFlag);
						Serial.print("   bb_NewRestitution=");Serial.println(bb_NewRestitution);
						Serial.print("   bb_NewSpeed=");Serial.println(bb_NewSpeed);
						P25_BouncingBallPlot(bb_NewGravity,bb_NewRestitution,bb_NewSpeed);	//Plot bouncing ball simulation...
						ClearOperInputArray();
					break;
					
					case 26:	//Demo Plot: "Happy Holidays from Nuts and Volts"
						P26_HappyHolidaysDemo();
						ClearOperInputArray();
					break;
					
					case 27:	//Demo Plot: "Fractal Tree" (This is pretty cool!)
						P27_FractalTreeDemo();
						ClearOperInputArray();
					break;
					
					default:
						Serial.println("  >>>> VALUE UNASSIGNED (Nothing Plotted)");
						ClearOperInputArray();
					break;
					
				}
			break;	//End 'p' Option
			
			case 'q':
			case 'Q':{	//Quick Start of CLOCK MODE; Passed parameters will define clock display format
				if (InputInteger[0]==0) Gbl_Show_Digital_Time=false;
				else if (InputInteger[0]>0 && InputInteger[0]!=_inputEmpty) Gbl_Show_Digital_Time=true;
				
				if (InputInteger[1]==0) Gbl_Show_DayOfWeek=false;
				else if (InputInteger[1]>0 && InputInteger[2]!=_inputEmpty) Gbl_Show_DayOfWeek=true;
				
				if (InputInteger[2]==0) Gbl_Show_Date=false;
				else if (InputInteger[2]>0 && InputInteger[2]!=_inputEmpty) Gbl_Show_Date=true;
				
				if (InputInteger[3]==0) Gbl_StartUp_In_Clock_Mode=false;
				else if (InputInteger[3]>0 && InputInteger[3]!=_inputEmpty) Gbl_StartUp_In_Clock_Mode=true;
				ClearOperInputArray();
				P23_BeginClockMode();
				ClearOperInputArray();
			}
			break;
			
			case 'S':	//Change LARGE STEP PIO SETTLING TIME count.
						//Only used for TEENSY & DUE_PIO modes.  Value is not used in DUE_DMA mode.
				//Increase settling time Count
				if (InputInteger[0]>-1 && InputInteger[0]<200){	//If number is in-range, set new value
					XYscope.PioLargeSettleCount=InputInteger[0];
					Serial.print(" <-LARGE DAC SETTLING count changed to: ");
					Serial.println(XYscope.PioLargeSettleCount);
				}
				else {
					Serial.print(" <-Current LARGE DAC SETTLING count is: ");
					Serial.print(XYscope.PioLargeSettleCount);
					Serial.println (" (Not changed)");
				}
				delay(100);	//wait until changes ripple through before printing 'stats'
				//PrintStatsToConsole();
				ClearOperInputArray();
			break;
			case 's':	//Change SMALL STEP PIO SETTLING TIME count
						//Only used for TEENSY & DUE_PIO modes.  Value is not used in DUE_DMA mode.
				//Increase settling time Count
				if (InputInteger[0]>-1 && InputInteger[0]<150){	//If number is in-range, set new value
					XYscope.PioSmallSettleCount=InputInteger[0];
					Serial.print(" <-SMALL DAC SETTLING count changed to: ");
					Serial.println(XYscope.PioSmallSettleCount);
				}
				else {
					Serial.print(" <-Current SMALL DAC SETTLING count is: ");
					Serial.print(XYscope.PioSmallSettleCount);
					Serial.println (" (Not changed)");
				}
				delay(100);	//wait until changes ripple through a couple of refresh cycles before printing 'stats'
				//PrintStatsToConsole();
				ClearOperInputArray();
			break;
			
			case 'T':	//Set TIME (hh,mm,ss T)
				DataValid = true;
				if (InputInteger[0]==_inputEmpty){	//display current time if no values entered
					Serial.print("  <-Current TIME: ");
					SerialPrintTime();
					Serial.print (" (Not Changed)  ");
					SerialPrintDate(); Serial.print (" (Not Changed)  ");
					Serial.println();

				}
				else {
					
					if (InputInteger[0]>24 || InputInteger[0] <0) DataValid=false;	//Make sure Hours are in range [0-23]
					if (InputInteger[1]>59 || InputInteger[1] <0) DataValid=false;	//Make sure Minutes are in range [0-59]	
					if (InputInteger[2]>59 || InputInteger[2] <0) DataValid=false;	//Make sure Seconds are in range [0-59]						
					
					if (DataValid){
						//Data is valid
						setTime(InputInteger[0],InputInteger[1],InputInteger[2],day(),month(),year());
						Serial.println();
						Serial.print("    TIME set to: ");
						SerialPrintTime();
						
						Serial.print (" (Updated)  ");
						SerialPrintDate(); Serial.print (" (Not Changed)  ");
						
						#if defined (TeensyRTC_PRESENT)
							Teensy3Clock.set(t); // set the Teensy RTC
							Serial.println ("   >>> TEENSY RTC has been Set! <<<");
						#endif
					}
					else{
						Serial.println();
						Serial.print(" <--- ERROR, invalid Data Entered (Time Not Changed!) ");
					}
				}
				ClearOperInputArray();
				
			break;
			case 't':	//Change Text Intensity Value
				if (InputInteger[0]==_inputEmpty){
					Serial.print("  <-Current Text Intensity is: ");
					Serial.print(XYscope.getTextIntensity());
					Serial.println(" %");
				}
				else {
					if (InputInteger[0]>0 && InputInteger[0]<256){	//If number is in-range, set new value
						XYscope.setTextIntensity(InputInteger[0]);
						Serial.print(" <-TEXT intensity for NEW PLOTS set to: ");
						Serial.print(XYscope.getTextIntensity());
						Serial.println(" %");
					}
					else{	//Number is out of range...Let oper know
						Serial.println (" <- ERROR: Value out of range (must be 1-255)");
					}
				}
				ClearOperInputArray();	
			break;
			
			case 'U':
			case 'u':	//Change PIO UNBLANK count
						//Only used for TEENSY & DUE_PIO modes.  Value is not used in DUE_DMA mode.
				//Increase Unblank Count
				if (InputInteger[0]>-1 && InputInteger[0]<200){	//If number is in-range, set new value
					XYscope.PioUnblankCount=InputInteger[0];
					Serial.print(" <-UNBLANK count changed to: ");
					Serial.println(XYscope.PioUnblankCount);
				}
				else {
					Serial.print(" <-Current UNBLANK count is: ");
					Serial.print(XYscope.PioUnblankCount);
					Serial.println (" (Not changed)");
				}
				delay(100);	//wait until changes ripple through before printing 'stats'
				//PrintStatsToConsole();
				ClearOperInputArray();
			break;
			
			case 'W':
			case 'w': // Screen Saver WAKE-UP and SET-SCREEN-SAVE-TIME test		
				if(InputInteger[0]==_inputEmpty) {
					//Example of How To Wakeup from a screen saver blank event
					//To wakeup the screen, just call the setScreenSaverSecs routine with the Current Setting...
					// 1) First, retrieve the Current TIMEOUT Setting (in seconds)
					long curScreenSaverSetting = XYscope.getScreenSaveSecs();
					// 2) Then just Call the setScreenSave...routine with Current TIMEOUT value
					XYscope.setScreenSaveSecs(curScreenSaverSetting);	//This sequence wakes up screen and resets the time 
					Serial.print("  ScreenSaver AWAKE. Will sleep again in ");Serial.print(curScreenSaverSetting);Serial.println(" Secs.");													//without changing the defined timer interval;
				}													
				else {
					//check to ensure new setting is reasonable...
					if (InputInteger[0]>1 && InputInteger[0]<1000)	{
						//Value is okay...Reset the screen save timeout
						long NewScreenSaverSetting = InputInteger[0];	//Retrieve the desired new timer-out value from InputInteger[] buffer
						XYscope.setScreenSaveSecs(NewScreenSaverSetting);	//This sets the ScreenSave Time-out value (in seconds)
						Serial.print("  ScreenSaver set to ");Serial.print(NewScreenSaverSetting);Serial.println(" Secs.");
					}
					else Serial.println(" -- Sorry, NO CHANGE MADE; timeout value in this demo must be >0 and <1000 sec");				
				}
				ClearOperInputArray();
			break;
		case 'z':
		case 'Z':
			if(InputInteger[0]<0 || InputInteger[0]>4095 || InputInteger[1]<0 || InputInteger[1]>4095){
				Serial.print (" <-- Nothing Plotted - Bad X,y coordinates entered)");
			} else{
				XYscope.plotPoint(InputInteger[0],InputInteger[1]);
				Serial.print(" <-- Plot Point at X,Y = ("); Serial.print(InputInteger[0]);Serial.print(","); Serial.print(InputInteger[1]); Serial.println(")");
			}
			ClearOperInputArray();
		break;
		
		default: 
			//Undefined input character detected; just print HELP screen to user
			Serial.println(" <- invalid/unknown command character entered ");
			XYscope.plotStart();		//Clear Screen and plot message...
			XYscope.setFontSpacing(XYscope.prop);	//Set Proportional Spacing
			XYscope.printSetup(100, 2047, 230, 100);
			XYscope.print((char *)"Unknown Option Entered...Try Again");
			XYscope.plotEnd();
			XYscope.autoSetRefreshTime();
			delay(3000);	//Wait 3 seconds
			//P1_PlotCenteringPattern();
			setup();		//Reset All and draw startup image
			//PrintStatsToConsole();
			ClearOperInputArray();
		break;
		

		
		}	//End of operator main Operator input decoder
		//=========================== END Main Serial Input Decoder Loop =============================
		
		
	}	//END WAIT FOR OPR INPUT ..... if(Serial.available()) Loop
	else {
		//No new serial character is available...Lets see if the operator has pushed any of the push buttons
		//Start Special Logic to allow push buttons to be used to alter settings based on last keyboard command entered...
		
		switch (LastKeyPressed) {	//Decode last operator input received via the serial monitor link-up and let the UP/DOWN buttons change values
			case 's':	//Change SMALL STEP PIO SETTLING TIME count
						//Only used for TEENSY & DUE_PIO modes.  Value is not used in DUE_DMA mode.
				if (PB_EndOfListPtr==0) PB_EndOfListPtr=XYscope.XYlistEnd;
				if (PB_IncrBtn_DownEdge==HIGH){  //Increase settling time Count
					PB_IncrBtn_DownEdge=LOW;	//Reset the edge detector...Only 
					if (XYscope.PioSmallSettleCount<150){	//If number is still in-range, Increment it!
						XYscope.PioSmallSettleCount++;
						
						PB_EndOfListPtr=XYscope.XYlistEnd=PB_EndOfListPtr; 
						PlotToCRT_HwSettings();
						
						Serial.print(" <-SMALL DAC SETTLING count changed to: ");
						Serial.println(XYscope.PioSmallSettleCount);
						
					}
				}
				if (PB_DecrBtn_DownEdge==HIGH){  //Decrement settling time Count
					PB_DecrBtn_DownEdge=LOW;	//Reset the edge detector...Only 
					if (XYscope.PioSmallSettleCount>0){	//If number is still in-range, Decrement it!
						XYscope.PioSmallSettleCount--;
						
						PB_EndOfListPtr=XYscope.XYlistEnd=PB_EndOfListPtr; 
						PlotToCRT_HwSettings();
						
						Serial.print(" <-SMALL DAC SETTLING count changed to: ");
						Serial.println(XYscope.PioSmallSettleCount);
					}
				}

			break;
			case 'S':	//Change LARGE STEP PIO SETTLING TIME count
						//Only used for TEENSY & DUE_PIO modes.  Value is not used in DUE_DMA mode.
				if (PB_EndOfListPtr==0) PB_EndOfListPtr=XYscope.XYlistEnd;
				if (PB_IncrBtn_DownEdge==HIGH){  //Increase settling time Count
					PB_IncrBtn_DownEdge=LOW;	//Reset the edge detector...Only 
					if (XYscope.PioLargeSettleCount<200){	//If number is still in-range, Increment it!
						XYscope.PioLargeSettleCount++;
						
						PB_EndOfListPtr=XYscope.XYlistEnd=PB_EndOfListPtr; 
						PlotToCRT_HwSettings();
												
						Serial.print(" <-LARGE DAC SETTLING count changed to: ");
						Serial.println(XYscope.PioLargeSettleCount);
					}
				}
				if (PB_DecrBtn_DownEdge==HIGH){  //Decrease settling time Count
					PB_DecrBtn_DownEdge=LOW;	//Reset the edge detector...Only 
					if (XYscope.PioLargeSettleCount>0){	//If number is still in-range, Decrement it!
						XYscope.PioLargeSettleCount--;
						
						PB_EndOfListPtr=XYscope.XYlistEnd=PB_EndOfListPtr; 
						PlotToCRT_HwSettings();
						
						Serial.print(" <-Large DAC SETTLING count changed to: ");
						Serial.println(XYscope.PioLargeSettleCount);
					}
				}
			break;
			
			case 'l':	//Change LARGE STEP PIO SETTLING TIME count
			case 'L':
						//Only used for TEENSY & DUE_PIO modes.  Value is not used in DUE_DMA mode.
				if (PB_EndOfListPtr==0) PB_EndOfListPtr=XYscope.XYlistEnd;
				if (PB_IncrBtn_DownEdge==HIGH){  //Increase Threshold Count
					PB_IncrBtn_DownEdge=LOW;	//Reset the edge detector...Only 
					if (XYscope.NoSettlingTimeReqd<(4095-32)){	//If number is still in-range, Increment it!
						XYscope.NoSettlingTimeReqd = XYscope.NoSettlingTimeReqd+32;
						
						PB_EndOfListPtr=XYscope.XYlistEnd=PB_EndOfListPtr; 
						PlotToCRT_HwSettings();
						
						int TempIntensity=XYscope.getGraphicsIntensity();
						XYscope.setGraphicsIntensity(50);
						XYscope.plotLine(50,XYscope.NoSettlingTimeReqd,4045,XYscope.NoSettlingTimeReqd);
						XYscope.setGraphicsIntensity(TempIntensity);
						
						Serial.print(" <-Big-Step Threshold LIMIT count changed to: ");
						Serial.println(XYscope.NoSettlingTimeReqd);
					}
				}
				if (PB_DecrBtn_DownEdge==HIGH){  //Decrease Threshold Count
					PB_DecrBtn_DownEdge=LOW;	//Reset the edge detector...Only 
					if (XYscope.NoSettlingTimeReqd>31){	//If number is still in-range, Decrement it!
						XYscope.NoSettlingTimeReqd = XYscope.NoSettlingTimeReqd-32;
						
						PB_EndOfListPtr=XYscope.XYlistEnd=PB_EndOfListPtr; 
						PlotToCRT_HwSettings();
						
						int TempIntensity=XYscope.getGraphicsIntensity();
						XYscope.setGraphicsIntensity(50);
						XYscope.plotLine(50,XYscope.NoSettlingTimeReqd,4045,XYscope.NoSettlingTimeReqd);
						XYscope.setGraphicsIntensity(TempIntensity);
						
						Serial.print(" <-Big-Step Threshold LIMIT count changed to: ");
						Serial.println(XYscope.NoSettlingTimeReqd);
					}
				}
			break;	

			case 'u':	//Change Pio Unblank time count
			case 'U':
						//Only used for TEENSY & DUE_PIO modes.  Value is not used in DUE_DMA mode.
				if (PB_EndOfListPtr==0) PB_EndOfListPtr=XYscope.XYlistEnd;
				if (PB_IncrBtn_DownEdge==HIGH){  //Increase Unblank time Count
					PB_IncrBtn_DownEdge=LOW;	//Reset the edge detector...Only 
					if (XYscope.PioUnblankCount<(200)){	//If number is still in-range, Increment it!
						XYscope.PioUnblankCount++;
						
						PB_EndOfListPtr=XYscope.XYlistEnd=PB_EndOfListPtr; 
						PlotToCRT_HwSettings();
						
						Serial.print(" <-UNBLANK count changed to: ");
						Serial.println(XYscope.PioUnblankCount);
					}
				}
				if (PB_DecrBtn_DownEdge==HIGH){  //Decrement Unblank time Count
					PB_DecrBtn_DownEdge=LOW;	//Reset the edge detector...Only 
					if (XYscope.PioUnblankCount>0){	//If number is still in-range, Decrement it!
						XYscope.PioUnblankCount--;
						
						PB_EndOfListPtr=XYscope.XYlistEnd=PB_EndOfListPtr; 
						PlotToCRT_HwSettings();
						
						Serial.print(" <-UNBLANK count changed to: ");
						Serial.println(XYscope.PioUnblankCount);
					}
				}
			break;

			
			default:
			PB_EndOfListPtr=0;
			break;
		}

	}

}	//END MAIN LOOP




//RTC Clock Setup function - Only add into code base if RTC is enabled =================

	#if defined (TeensyRTC_PRESENT)
		time_t getTeensy3Time()	{
				
		  return Teensy3Clock.get();	//Fetch the time from the Teensy 3.x On-board Clock
		}
	#endif
	
	
	void SerialPrintRaw_T(){
		// Diagnostic Serial.print Raw Time Routine
		//
		//	Passed Parameters	NONE 	Returns: NOTHING
		//
		//	20180701 Ver 0.0	E.Andrews	First cut
		
		Serial.print (" <t = ");Serial.print(now()); Serial.print("> ");
	}
	
	void SerialPrintDate() {
		// Diagnostic Serial.print current-DAY-DATE Routine
		//
		//	Passed Parameters	NONE 	Returns: NOTHING
		//
		//	20180701 Ver 0.0	E.Andrews	First cut
		//
		Serial.print("Day of week: ");
		Serial.print(dayStr(weekday())); Serial.print("  (Day: ");
		Serial.print(weekday()); 	// Print current DayOfWeek (1-7, 1=Sunday)
		Serial.print(")");
		Serial.print(", Date: ");
		Serial.print(monthStr(month())); Serial.print("  ");
		if (month()<10) Serial.print (" ");
		Serial.print(month());
		Serial.print("/");
		if (day()<10) Serial.print ("0");
		Serial.print(day());
		Serial.print("/");
		Serial.print(year());
	}
	void SerialPrintTime() {
		// Diagnostic Serial.print current-CLOCK-TIME Routine
		//
		//	Passed Parameters	NONE 	Returns: NOTHING
		//
		//	20180701 Ver 0.0	E.Andrews	First cut
		//
		if (hour()<10) Serial.print (" ");
		Serial.print(hour());
		Serial.print(":");
		if (minute()<10) Serial.print ("0");
		Serial.print(minute());
		Serial.print(":");
		if (second()<10) Serial.print ("0");
		Serial.print(second());		
	}
//RTC_CODE END ===================


//===============================================
//	Start of Plotting Test Routines
//===============================================

void P17_PlotRandomRectangles(int NumOfPoints,int NumOfLoops) {
	//	Graphics Demo Routine
	//
	//	Passed Parameters	int NumOfPoints,int NumOfLoops
	//
	//	Returns: NOTHING
	//
	//	20180701 Ver 0.0	E.Andrews	First cut
	//
	if(NumOfPoints<1 || NumOfPoints>XYscope.MaxBuffSize) NumOfPoints=10000;	//Default to 10K Points
	Serial.print (" <- Plotting Rectangles, ");Serial.print (NumOfPoints);Serial.print(" points...");
	if (NumOfLoops<1)NumOfLoops=1;
	for (int c=0;c<NumOfLoops;c++){	
		XYscope.plotStart();
		for (int i=1;i<NumOfPoints;i++){
			XYscope.plotRectangle(random(4096),random(4096),random(4096),random(4096));
			if (XYscope.XYlistEnd>NumOfPoints) break;
		}
		delay(100);
		if (Serial.available())break; //ESCAPE if oper pushes any key
	}	
	if (Serial.available()){
		char d = Serial.read(); //If we ESCAPED, flush character 
		d=d;	//Add this just to kill off a unused variable compiler warning		
	}
	
	Serial.print("DONE, Points Actually Plotted: ");Serial.println(XYscope.XYlistEnd);
	//PrintStatsToConsole();
}
void P16_PlotRandomVectors(int NumOfPoints,int NumOfLoops) {
	//	Graphics Demo Routine
	//
	//	Passed Parameters	int NumOfPoints,int NumOfLoops
	//
	//	Returns: NOTHING
	//
	//	20180701 Ver 0.0	E.Andrews	First cut
	//
	if(NumOfPoints<1 || NumOfPoints>XYscope.MaxBuffSize) NumOfPoints=10000;	//Default to 10K Points
	Serial.print (" <- Plotting Vectors, ");Serial.print (NumOfPoints);Serial.print(" points...");
	if (NumOfLoops<0)NumOfLoops=1;
	for (int c=0;c<NumOfLoops;c++){
		XYscope.plotStart();	
		for (int i=1;i<NumOfPoints;i++){
			XYscope.plotLine(random(4096),random(4096),random(4096),random(4096));
			if (XYscope.XYlistEnd>NumOfPoints) break;
		}	
		delay(100);
		if (Serial.available())break; //ESCAPE if oper pushes any key
		
	}
	if (Serial.available()){
		char d = Serial.read(); //If we ESCAPED, flush character
		d=d;	//Add this just to kill off a unused variable compiler warning
	}
	Serial.print("DONE, Points Actually Plotted: ");Serial.println(XYscope.XYlistEnd);
	//PrintStatsToConsole();
}
void P18_PlotRandomCircles(int NumOfPoints,int NumOfLoops) {
	//	Graphics Demo Routine
	//
	//	Passed Parameters	int NumOfPoints,int NumOfLoops
	//
	//	Returns: NOTHING
	//
	//	20180701 Ver 0.0	E.Andrews	First cut
	//
	if(NumOfPoints<1 || NumOfPoints>XYscope.MaxBuffSize) NumOfPoints=10000;	//Default to 10K Points
	Serial.print (" <- Plotting Circles, ");Serial.print (NumOfPoints);Serial.print(" points...");
	if (NumOfLoops<1)NumOfLoops=1;
	for (int c=0;c<NumOfLoops;c++){
		XYscope.plotStart();
		for (int i=1;i<NumOfPoints;i++){
			XYscope.plotCircle(random(4096),random(4096),random(2048));
			if (XYscope.XYlistEnd>NumOfPoints) break;
		}
		delay(100);
		if (Serial.available())break; //ESCAPE if oper pushes any key	
	}
	if (Serial.available()){
		char d = Serial.read(); //If we ESCAPED, flush character	
		d=d;	//Add this just to kill off a unused variable compiler warning
	}
	Serial.print("DONE, Points Actually Plotted: ");Serial.println(XYscope.XYlistEnd);
	//PrintStatsToConsole();
}
void P19_PlotRandomEllipes(int NumOfPoints,int NumOfLoops) {
	//	Graphics Demo Routine
	//
	//	Passed Parameters	int NumOfPoints,int NumOfLoops
	//
	//	Returns: NOTHING
	//
	//	20180701 Ver 0.0	E.Andrews	First cut
	//
	if(NumOfPoints<1 || NumOfPoints>XYscope.MaxBuffSize) NumOfPoints=10000;	//Default to 10K Points
	Serial.print (" <- Plotting Ellipses, ");Serial.print (NumOfPoints);Serial.print(" points...");
	if (NumOfLoops<1)NumOfLoops=1;
	for (int c=0;c<NumOfLoops;c++){	
		XYscope.plotStart();
		for (int i=1;i<NumOfPoints;i++){
			XYscope.plotEllipse(random(4096),random(4096),random(2048),random(2048));
			if (XYscope.XYlistEnd>NumOfPoints) break;
		}
		delay(100);
		if (Serial.available())break; //ESCAPE if oper pushes any key	
	}
	if (Serial.available()){
		char d = Serial.read(); //If we ESCAPED, flush character		
		d=d;	//Add this just to kill off a unused variable compiler warning
	}
	Serial.print("DONE, Points Actually Plotted: ");Serial.println(XYscope.XYlistEnd);
	//PrintStatsToConsole();
}
void P14_ShowSingleChar(int ChCode, int ChHt){
	// Routine to plot a single character of the current font to the screen
	//
	//	Passed Parameters	
	//		ChCode	Ascii code for the signel character to display
	//		ChHt	Character height in pixels
	//
	//	Returns: NOTHING
	//
	//	20180701 Ver 0.0	E.Andrews	First cut
	//
	//Serial.print (" ChCode, ChHt = ");Serial.print(ChCode);Serial.print(",");Serial.println(ChHt);
	int CurTextInt=0;
	CurTextInt=XYscope.getTextIntensity();
	
	int NumOfChars=127-32;
	if (ChCode!= _inputEmpty) NumOfChars=1;
	if (ChCode<32 || ChCode>127)ChCode=random(48,128);	//Plot random character if ChCode not spec'd or is out-of-range
	if (NumOfChars >1) ChCode=32;
	if (ChHt>3250 || ChHt<50)ChHt=2500;	//Did Oper Spec a character Size to plot?
										//Use default size if ChHt out-of-range
										
	for (int i=ChCode;i<ChCode+NumOfChars;i++){
		if (Serial.available()) break;	//Oper can break out of this if needed...
	
		XYscope.plotStart();
		int TextHt=250;
		XYscope.printSetup(10, 100, TextHt);
		XYscope.setTextIntensity(100);
		XYscope.print("ASCII:");XYscope.print(i);XYscope.print(",'");
		XYscope.plotChar(i,XYscope.charX,XYscope.charY,TextHt);
		XYscope.print("',Size:");XYscope.print(ChHt);
		short CurrentFont = XYscope.getActiveFont();
		
		int LhCornerX=10,LhCornerY=700;

		int NextFontX = LhCornerX;
		int NextFontY = LhCornerY;
		if (CFG_IncludeHersheyFontROM) 	{	//Print HERSHEY font version to screen
			XYscope.setTextIntensity(100);
			XYscope.setActiveFont(1);	//Set HersheyFont
			XYscope.printSetup(LhCornerX, LhCornerY+ChHt+50, 200);
			XYscope.print(" Hershey Font");
			
			int CurrentGraphIntensity=XYscope.getGraphicsIntensity();
			
			XYscope.setGraphicsIntensity(40);
			XYscope.plotRectangle(LhCornerX,LhCornerY,LhCornerX+ChHt*.7,LhCornerY+ChHt);
			NextFontX=LhCornerX+ChHt*.7+50;
			XYscope.setTextIntensity(250);
			XYscope.plotChar(i, LhCornerX,LhCornerY,ChHt);
			XYscope.setGraphicsIntensity(CurrentGraphIntensity);

		}
		if (CFG_IncludeVectorFontROM) 	{	//Print the VECTOR font version to screen
			XYscope.setTextIntensity(100);
			XYscope.setGraphicsIntensity(40);
			LhCornerX=NextFontX;
			LhCornerY=NextFontY;
			
			XYscope.printSetup(LhCornerX, LhCornerY+ChHt+50, 200);
			XYscope.print(" Vector Font");
			
			int CurrentGraphIntensity=XYscope.getGraphicsIntensity();
			XYscope.setActiveFont(0);	//Set VectorFont
			
			XYscope.plotRectangle(LhCornerX,LhCornerY,LhCornerX+ChHt*.7,LhCornerY+ChHt);
			NextFontX=LhCornerX+2000+10;
			XYscope.setTextIntensity(250);
			XYscope.plotChar(i, LhCornerX,LhCornerY,ChHt);
			XYscope.setGraphicsIntensity(CurrentGraphIntensity);
			
		}		
		XYscope.autoSetRefreshTime();
		XYscope.setActiveFont(CurrentFont);
		delay(1000);
	}
	if (Serial.available()){
		char d = Serial.read(); //If we ESCAPED, flush character
		d=d;	//Add this just to kill off a unused variable compiler warning
	}
	XYscope.setTextIntensity(CurTextInt);
}
void P15_PlotRandomPoints(int NumOfPoints,int NumOfLoops){
	// Routine to plot one or more random YX points to the screen
	//
	//	Passed Parameters	
	//		NumOfPoints		Number of random points to plot
	//		NumOfLoops		Program will repeat the random plotting for "NumOfLoops"
	//
	//	Returns: NOTHING
	//
	//	20180701 Ver 0.0	E.Andrews	First cut
	//
	if(NumOfPoints<1 || NumOfPoints>XYscope.MaxBuffSize) NumOfPoints=10000;	//Default to 10K Points
	Serial.print (" <- Plotting ");Serial.print (NumOfPoints);Serial.print(" points...");
	if (NumOfLoops<1)NumOfLoops=1;
	for (int c=0;c<NumOfLoops;c++){	
		XYscope.plotStart();
		for (int i=1;i<NumOfPoints;i++){
			XYscope.plotPoint(random(4096),random(4096));
		}
		XYscope.plotEnd();
		XYscope.autoSetRefreshTime();
		delay(100);
		if (Serial.available())break; //ESCAPE if oper pushes any key	
	}
	if (Serial.available()){
		char d = Serial.read(); //If we ESCAPED, flush character			
		d=d;	//Add this just to kill off a unused variable compiler warning
	}
	Serial.println("DONE");
	//PrintStatsToConsole();
}

void ClearOperInputArray(){
	// Routine to clear Operator Parameter Input Data array
	//
	//	Passed Parameters:	NONE

	//
	//	Returns: NOTHING
	//
	//	20180701 Ver 0.0	E.Andrews	First cut
	//	20180808 Ver 1.0	E.Andrews	Added Floating point capability to input process
	//
	//Serial.println("  ClearOperInputArray....");
	Serial.println(); Serial.print(" RDY-> ");
	for (int i=0;i<_maxInputNumbers;i++){
		InputInteger[i]=_inputEmpty;
		InputFloat[i]=_inputEmpty;
	} 
	InputPtr=0;			//Array Pointer
	InputDecPt=0;		//Pointer that shows when a decimal point has been entered
	InputNegFlag=false;	//default to POSITIVE numbers
}

void P20_PlotAnimatedLogo(){
	// Routine to show a simple animated display
	//
	//	Passed Parameters	NONE
	//
	//	Returns: NOTHING
	//
	//	20170725 Ver 0.0	E.Andrews	First cut
	//
	int Lx = 100, Ly = 2000, Lht = 1000;
	XYscope.plotStart();
	bool const Underlined = true;
	XYscope.printSetup(2047 - int(float(4000 / 16) * 1.5), 1200, 400, 100);
	XYscope.print((char *)"AGI", Underlined);
	short Tx = 150, Ty = 900;	//Postion for MONO spacing mode
	if (XYscope.getFontSpacing() != XYscope.mono) Tx = 775;	//Reposition if PROP spacing is active
	XYscope.printSetup(Tx, Ty, 230, 100);
	XYscope.print((char *)"A", Underlined);
	XYscope.print((char *)"rduino ");		//(No-underline)
	XYscope.print((char *)"G", Underlined);
	XYscope.print((char *)"raphics ");		//(No-underline)
	XYscope.print((char *)"I", Underlined);
	XYscope.print((char *)"nterface");		//(No-underline)
	XYscope.printSetup(1100, 100, 175, 100);
	XYscope.print(startMsg);				//(No-underline)
	XYscope.plotRectangle(0, 0, 4095, 4095);

	XYscope.printSetup(20, 100, 175, 100);
	int StartOfLogo = XYscope.XYlistEnd;//Grab the current pointer value

	for (Lht = 30; Lht < 2025; Lht = Lht + 50) {
		XYscope.XYlistEnd = StartOfLogo;//Here's how to do ANIMATION at the endof XYlist...
										//Keep the pointer fixed to keep rewriting
										//the same part of the array
		XYscope.plotErr = 1;
		Lx = 2047 - int(float(Lht / 16) * 14);
		Ly = 2000;
		XYscope.plotArduinoLogo(Lx, Ly, Lht);
		delay(40);	//Just a slight delay to slow redraws down a bit
	}
}

void P0_PlotOpAmpAdjustPattern(){
	// Routine that plots a sine/cosine wave output for setting op-amp gain and offset.
	//
	//	Passed Parameters	NONE
	//
	//	Returns: NOTHING
	//
	//	20170725 Ver 0.0	E.Andrews	First cut
	//	20180425 Ver 1.0	E.Andrews	Now handles DUE and TEENSY processors	
	//

	XYscope.plotClear();	//This is not really needed...plotStart() does a clear and start.
	XYscope.plotStart();
	XYscope.plotPoint(0, 4095);
	int xCtr = 2047;
	int yCtr = 2047;
	int xRadius = 2047;
	int yRadius = 2047;

	float numTestPoints = XYscope.getGraphicsIntensity();
	float testAngle = 0;
	int patternCount = 0;
	float angleStep = 2 * 3.141526 / numTestPoints;
	testAngle = testAngle - angleStep;
	for (patternCount = 0; patternCount < numTestPoints; patternCount++) {
		testAngle = testAngle + angleStep;
		XYscope.plotPoint(int(xCtr + xRadius * sin(testAngle)),int(yCtr + yRadius * cos(testAngle)));
	}
	XYscope.plotPoint(4095, 4095);
	XYscope.plotEnd();
	XYscope.autoSetRefreshTime();

	PrintStatsToConsole();	//Send mem usage report to console
	//ClearOperInputArray();

}

void PrintDmaStatusReport(){
	// Routine to dump a DMA Timing Status report to Serial Port.
	//
	//	Passed Parameters	NONE
	//
	//	Returns: NOTHING
	//
	//	20170725 Ver 0.0	E.Andrews	First cut
	//	20180521 Ver 1.0	E.Andrews	Reworked for TEENSY release (Not Used for TEENSY)	
	//
	Serial.println("==== DMA Timing Status Report ====");
	Serial.print("    DMA Clock Freq: ");
	Serial.print(XYscope.DmaClkFreq_Hz / 1000);
	Serial.print(" Khz");
	Serial.print(" (Period: ");
	Serial.print(XYscope.DmaClkPeriod_us);
	Serial.println(" us)");
	Serial.print("   BACK PORCH: New Value = ");
	Serial.println(XYscope.backPorchBlankCount);
	Serial.print("   FRONT PORCH: New Value = ");
	Serial.println(XYscope.frontPorchBlankCount);
	Serial.println("==================================");
}
void P12_PlotRandNums(bool UnderlineFlag) {	
	//	Test routine to test XYscope.print (number) routines.
	//	Print formatted Numbers WITH Underline
	//	This is just a bunch of random numbers sent to screen
	//	using various number formats with every other row underlined
	//
	//	Passed Parameters	NONE
	//
	//	Returns: NOTHING
	//
	//	20170725 Ver 0.0	E.Andrews	First cut
	//	20180521 		 	E.Andrews	Checked for use with TEENSY release	
	//			
	const int ChInt = 100;	//Brightness of text for this test
	float TstNum = random(-160000000, 160000000);
	TstNum = TstNum / 10000;
	Serial.print((char *)" ...Trying to print: ");
	Serial.print(TstNum, 9);
	Serial.println("  ");

	XYscope.plotClear();
	XYscope.plotStart();
	int CharSize = random(200, 325);
	int xPos = 20, yPos = 4095 - CharSize - 50;
	XYscope.printSetup(xPos, yPos, CharSize, ChInt);
	XYscope.print(int(TstNum), UnderlineFlag);	//Print Integer

	yPos = yPos - CharSize - 50;
	XYscope.printSetup(xPos, yPos, CharSize, ChInt);
	XYscope.print(TstNum);					//Print float to default (2) places

	yPos = yPos - CharSize - 50;
	XYscope.printSetup(xPos, yPos, CharSize, ChInt);
	XYscope.print(TstNum, 3, UnderlineFlag);	//Print Floating point to specified number of places

	yPos = yPos - CharSize;
	XYscope.printSetup(xPos, yPos, CharSize, ChInt);
	XYscope.print(TstNum, 4);				//Print Floating point to specified number of places

	yPos = yPos - CharSize;
	XYscope.printSetup(xPos, yPos, CharSize, ChInt);
	XYscope.print(TstNum, 5, UnderlineFlag);	//Print Floating point to specified number of places

	yPos = yPos - CharSize;
	XYscope.printSetup(xPos, yPos, CharSize, ChInt);
	XYscope.print(TstNum, 6);				//Print Floating point to specified number of places

	yPos = yPos - CharSize;
	XYscope.printSetup(xPos, yPos, CharSize, ChInt);
	XYscope.print(TstNum, 7, UnderlineFlag);	//Print Floating point to specified number of places

	yPos = yPos - CharSize;
	XYscope.printSetup(xPos, yPos, CharSize, ChInt);
	XYscope.print(TstNum, 8);				//Print Floating point to specified number of places

	yPos = yPos - CharSize;
	XYscope.printSetup(xPos, yPos, CharSize, ChInt);
	XYscope.print(TstNum, 9, UnderlineFlag);	//Print Floating point to specified number of places

	yPos = yPos - CharSize;
	XYscope.printSetup(xPos, yPos, CharSize, ChInt);
	XYscope.print(TstNum, 10);				//Print Floating point to specified number of places

	yPos = yPos - CharSize;
	XYscope.printSetup(xPos, yPos, CharSize, ChInt);
	XYscope.print(TstNum, 11, UnderlineFlag);	//Print Floating point to specified number of places

	XYscope.autoSetRefreshTime();
	PrintStatsToConsole();

}
void P13_PlotCharSet_1(){
	//	Test routine to test XYscope.print (number) routines.
	//	Print formatted Numbers WITH Underline
	//	This is just a bunch of random numbers sent to screen
	//	using various number formats with every other row underlined
	//
	//	Passed Parameters	NONE
	//
	//	Returns: NOTHING
	//
	//	20170725 Ver 0.0	E.Andrews	First cut
	//	20180521 		 	E.Andrews	Checked for use with TEENSY & HersheyFont release	
	//	

	//Display sample text, Test #1
	XYscope.plotClear();
	XYscope.plotStart();
	XYscope.plotErr = 1;
	int cX = 50, cY = 4050, charHt = 500;
	const int charPerLine = 12;
	uint8_t startChar = ' ' - charPerLine;
	for (int Row = 1; Row < 9; Row++) {
		startChar = startChar + charPerLine;
		cY = 4050 - Row * 500;
		cX = 0;
		for (uint8_t tstChar = startChar;
				tstChar < startChar + charPerLine; tstChar++) {
			XYscope.plotChar(tstChar, cX, cY, charHt);
		}
	}
	//XYscope.autoSetRefreshTime();
}

void P14_PlotCharSet_2(){	
	//	Test routine to test XYscope.print (number) routines.
	//	Print formatted Numbers WITH Underline
	//	This is just a bunch of random numbers sent to screen
	//	using various number formats with every other row underlined
	//
	//	Passed Parameters	NONE
	//
	//	Returns: NOTHING
	//
	//	20170725 Ver 0.0	E.Andrews	First cut
	//	20180521 		 	E.Andrews	Checked for use with TEENSY & HersheyFont release	
	//
	
	//Display sample text, Test #2
	XYscope.plotClear();
	XYscope.plotStart();
	XYscope.plotErr = 1;
	int cX = 50, cY = 4050, charHt = 500;
	const int charPerLine = 12;
	uint8_t startChar = '0' - charPerLine;
	for (int Row = 1; Row < 9; Row++) {
		startChar = startChar + charPerLine;
		cY = 4050 - Row * 500;
		cX = 0;
		for (uint8_t tstChar = startChar;
				tstChar < startChar + charPerLine; tstChar++) {
			XYscope.plotChar(tstChar, cX, cY, charHt);
		}
	}
	XYscope.autoSetRefreshTime();
	PrintStatsToConsole();

}

//=======================================================
//	PONG Demo
//=======================================================
void P24_PongDemo() {
	// PONG style game display to demo how to do animation.
	// This demo just moves a 'ball' around the screen bouncing off of walls.
	// Touch any key to terminate the demo and select another program
	// option.  The program could easily sample some analog ports and
	// use that data to 'move the paddles' and in turn actually code
	// a working 'PONG-Style' game.
	//
	//	Passed Parameters	NONE
	//
	//	Returns: NOTHING
	//
	//	20170708 Ver 0.0	E.Andrews	First cut
	//						(Updated slightly throughout development cycle without version change)
	//	20181521			E. Andrews  Checked/Updated slightly for use with TEENSY release
	//
	float ballX = 2048, ballY = 2048;//define and initialize ball at center of screenBall Center Point
	int ballR = 75;					//Define Ball Radius
	float ballDX = 4.5, ballDY = 2.31;	//Define initial Ball Velocities

	int Lwall = 0, Rwall = 4095;		//Define Left & right Wall	X-Coord

	int Twall = 4095, Bwall = 0;		//Top & Bottom wall Y-Coord

	int ballCount = 5, ballMax = 1;	//Define number of times to draw ball

	// Clear screen and draw court
	XYscope.plotStart();
	XYscope.plotLine(Lwall, Twall, Rwall, Twall);	//Top line
	XYscope.plotLine(Rwall, Twall, Rwall, Bwall);	//Right line
	XYscope.plotLine(Rwall, Bwall, Lwall, Bwall);	//Bottom line
	XYscope.plotLine(Lwall, Bwall, Lwall, Twall);	//Left line
	//Draw "net" as a dashed line down the center of the screen
	const int dashLength = 200;
	for (int dash = dashLength / 2; dash < Twall - dashLength / 2;
			dash = dash + dashLength) {
		XYscope.plotLine(Rwall / 2, dash, Rwall / 2, dash + dashLength / 2);//Center line
	}

	//Draw a simulated score...Note, scores do not change in DEMO code!
	XYscope.printSetup(700, 3000, 800);
	XYscope.print((char *)"18");
	XYscope.printSetup(2700, 3000, 800);
	XYscope.print((char *)"09");

	//Draw simulated paddles...Paddles remain stationary in DEMO code!
	XYscope.plotRectangle(200, 1200, 300, 1700);	//Left Paddle
	XYscope.plotRectangle(3800, 1600, 3900, 2100);	//Right Paddle

	//Remember the current XYlistEnd value so we can return to it every time we
	//want to move and rewrite the ball
	int ixEmptyCourt = XYscope.XYlistEnd;

	Serial.print(" <- Running PONG DEMO; Press a key to stop DEMO...");
	int wait = true;
	ballCount = 0;

	while (wait)	//In this loop, we just move the ball around the court!
	{
		if (Serial.available()) {//Check for key press...any key = Leave the DEMO
			int d=0;
			while (d>=0) {
				d=Serial.read();	//clean out the buffer (Retrieved data is NOT USED)
			} 						//...flush buffer, keep reading until buffer is empty.  Note, Serial.read will return as -1 when no data is available

			wait = false;
			Serial.println(F("(Stopped by Key Press)"));
			delay(100);
		}
		//Draw Ball
		XYscope.plotCircle(ballX, ballY, ballR);
		ballCount = ballCount + 1;

		//Plot the ball multiple times before moving it to make it brighter
		if (ballCount > ballMax) {
			XYscope.XYlistEnd = ixEmptyCourt;
			ballCount = 0;
		}

		//Move Ball...
		ballX = ballX + ballDX;
		ballY = ballY + ballDY;
		//Define Ball to Wall collision limits based on size of ball
		int TopLimit = Twall - ballR;
		int BotLimit = Bwall + ballR;
		int RightLimit = Rwall - ballR;
		int LeftLimit = Lwall + ballR;

		//---Check for Top-Bottom collisions with walls...
		if (ballY >= TopLimit) {	//Top Wall Impact
			ballY = +TopLimit - (abs(ballDY) - abs(ballY - TopLimit));
			ballDY = -ballDY;
		}
		if (ballY <= BotLimit) {	//Bottom wall Impact
			ballY = BotLimit + (abs(ballDY) - abs(ballY - BotLimit));
			ballDY = -ballDY;
		}
		//Check Left-Right collisions with walls...
		if (ballX >= RightLimit) {	//Right Wall Impact
			ballX = RightLimit - (abs(ballDX) - abs(ballX - RightLimit));
			ballDX = -ballDX;
		}
		if (ballX <= LeftLimit) {	//Left Wall Impact
			ballX = LeftLimit + (abs(ballDX) - abs(ballX - LeftLimit));
			ballDX = -ballDX;
		}
		//---Check for & react to ball collisions with Paddles.....
		//	Need to add scoring & reporting, ball reaction, ball sounds, etc...
		//  You Add this Part!!!

	}

}
//=========== END PONG DEMO =============================


//=======================================================
//	HappyHolidays Demo
//=======================================================
void P26_HappyHolidaysDemo() {
	// This routine paints a holiday tree onto the screen
	//
	//	Passed Parameters: NONE		Returns: NOTHING
	//
	//	20181012 Ver 0.0	E.Andrews	First cut
	//
	const float pi = 3.14;
	float X_Gain=1000.;				//Set gain so tree base width=2000
	int Y=300;						//Base of tree start point
	const float Y_Step = 3.8;		//Y step size
	int X=0;						//Initialize X start point
	const float deltaGain=1.;		//Gain change parameter
	const int X_Offset=3000;		//Center tree about X=3000
	float Tau = pi/2.;				//Init Sine wave angle value
	const float deltaAng =2.*pi/100.;	//Angle change parameter
	//Plot startup
	XYscope.plotStart();
	XYscope.setGraphicsIntensity(100);
	XYscope.plotRectangle(0,0,4095,4095);	//Plot a nice screen boarder
	//Now Plot the Christmas Tree (actually a decaying sine-wave)
	for (int i=1;i<1001;i++) {		//Make a tree out of 1000 points
		X=int(X_Gain*sin(Tau)+X_Offset);
		XYscope.plotPoint(X,Y);		//Plot a point of the waveform (tree)
		Tau=Tau+deltaAng;			//Increment angle
		X_Gain=X_Gain-deltaGain;	//Decr amplitude so waveform decays 
		Y=Y+Y_Step;					//Increment Y position
	}
	//Put a Star on Top Of The Tree
	plotStar(X,Y+150,300);
	//plot some random snow flakes..
	int NumOfPoints=500;
	for (int i=1;i<NumOfPoints;i++){
		XYscope.plotPoint(random(4096),random(4096));
	}
	//Position and plot the greeting text
	short Xtext=50, Ytext=3000,TextSize=525,TextInten=220;
	XYscope.setActiveFont(1);		//Choose Vector Font
	XYscope.setFontSpacing(0);		//Choose Proportional Spacing
	XYscope.printSetup(Xtext,Ytext,TextSize,TextInten);
	XYscope.print("    Happy");
	Ytext=Ytext-1.*TextSize;		//Increment to next row
	XYscope.printSetup(Xtext,Ytext,TextSize,TextInten);
	XYscope.print("  Holidays");
	Ytext=Ytext-1.*TextSize;		//Increment to next row
	XYscope.printSetup(Xtext,Ytext,TextSize,TextInten);
	XYscope.print("     From");
	Ytext=Ytext-1.*TextSize;		//Increment to next row
	XYscope.printSetup(Xtext,Ytext,TextSize,TextInten);
	XYscope.print("    Nuts &");
	Ytext=Ytext-1.*TextSize;		//Increment to next row
	XYscope.printSetup(Xtext,Ytext,TextSize,TextInten);
	XYscope.print("    Volts!");
	XYscope.plotEnd();
}

void plotStar(int Xc,int Yc,int Radius){
	//	 This routine paints a star onto the screen
	//
	//	Passed Parameters	
	//		int Xc, Yc		Center (X,Y) coordinates of star
	//		int Radius		Radius of outer spoke-tips of the star
	//
	//	Returns: NOTHING
	//
	//	20181012 Ver 0.0	E.Andrews	First cut
	//	
	const float pi = 3.14;
	const float pi_5=(2*pi)/5.;
	int X0=Radius*sin(0*pi_5)+Xc;
	int Y0=Radius*cos(0*pi_5)+Yc;
	int X1=Radius*sin(pi_5)+Xc;
	int Y1=Radius*cos(pi_5)+Yc;
	int X2=float(Radius)*sin(2.*pi_5)+Xc;
	int Y2=float(Radius)*cos(2.*pi_5)+Yc;
	int X3=float(Radius)*sin(3.*pi_5)+Xc;
	int Y3=float(Radius)*cos(3.*pi_5)+Yc;
	int X4=float(Radius)*sin(4.*pi_5)+Xc;
	int Y4=float(Radius)*cos(4.*pi_5)+Yc;
	XYscope.setGraphicsIntensity(150);
	/* 
		//Plot Star spokes with intersecting lines
		XYscope.plotLine(X0,Y0,X2,Y2);
		XYscope.plotLine(X2,Y2,X4,Y4);	
		XYscope.plotLine(X4,Y4,X1,Y1);
		XYscope.plotLine(X1,Y1,X3,Y3);
		XYscope.plotLine(X3,Y3,X0,Y0);
	 */
	//Plot Star spokes without intersecting lines
	float dx=abs(X2-X0)/3;		//Spoke End-point lengths
	float dy=abs(Y0-Y2)/3;
	XYscope.plotLine(X0,Y0,X0+dx,Y0-dy);
	XYscope.plotLine(X2-dx,Y2+dy,X2,Y2);
	
	dx=abs(X2-X4)/3;
	dy=abs(Y4-Y2)/3;	
	XYscope.plotLine(X2,Y2,X2-dx,Y2+dy);	
	XYscope.plotLine(X4+dx,Y4-dy,X4,Y4);
	
	dx=abs(X1-X4)/3;
	dy=abs(Y1-Y4)/3;	
	XYscope.plotLine(X4,Y4,X4+dx,Y4+dy);
	XYscope.plotLine(X1,Y1,X1-dx,Y1-dy);
	
	dx=abs(X3-X0)/3;
	dy=abs(Y3-Y0)/3;
	XYscope.plotLine(X0,Y0,X0-dx,Y0-dy);
	XYscope.plotLine(X3,Y3,X3+dx,Y3+dy);
	
	dx=abs(X3-X1)/3;
	dy=abs(Y3-Y1)/3;
	XYscope.plotLine(X3,Y3,X3+dx,Y3+dy);
	XYscope.plotLine(X1,Y1,X1-dx,Y1-dy);
}

//Define some global variables needed for Recursive Fractal Processes
int FR_X, FR_Y,FR_Length;
float FR_d_Len,FR_Angle;
float FR_d_Angle;
float FR_PI = 3.14;

void P27_FractalTreeDemo() {
	// This recursive routine plots a fractal tree
	//
	//	Passed Parameters: NONE		Returns: NOTHING
	//
	//	Input parameter 1, if entered, specifies starting branch length
	//		and more or less defines overall tree size (range: 500-2000)
	//
	//	Input parameter 2, if entered, specifies branch angle and ~defines
	//		overall tree shape(range: .2-.7  Note: .4 ~45 deg branch angle)
	//
	//	20181012 Ver 0.0	E.Andrews	First cut
	//
	FR_X=2000;
	FR_Y=100;
	
	int Start_Length=1000;
	
	//Variable #1 (if entered) sets tree size (range 500-2000)	
	if(InputInteger[1]!=_inputEmpty) Start_Length=InputInteger[1];
	FR_Angle=FR_PI/2.;
	FR_d_Angle=FR_PI/8;		//Set default branch to 45 Degrees
	//User variable #2 (if entered) sets branch angle (range: .2-.7  Note: .4 ~45Deg)
	if(InputFloat[2]!=_inputEmpty) FR_d_Angle=InputFloat[2];
	FR_d_Len = 0.67;
	XYscope.plotStart();
	XYscope.setGraphicsIntensity(100);
	XYscope.plotRectangle(0,0,4095,4095);	//Plot a nice screen boarder
	plot_Fractal_Branch(Start_Length);		//Now Plot a fractal Tree in just one call!
	
	//Position and plot the summary text
	short Xtext=200, Ytext=800,TextSize=225,TextInten=220;
	XYscope.setActiveFont(1);		//Choose Vector Font
	XYscope.setFontSpacing(0);		//Choose Proportional Spacing
	XYscope.printSetup(Xtext,Ytext,TextSize,TextInten);
	XYscope.print("XYscope Plots");
	Ytext=Ytext-1.*TextSize;		//Increment to next row
	XYscope.printSetup(Xtext,Ytext,TextSize,TextInten);
	XYscope.print("a Fractal Tree");
	
}

void plot_Fractal_Branch(int BranchLength){
	// This routine plots a branch set to the screen.  This routine is recursively called.
	// Ref: https://www.bing.com/videos/search?q=how+to+program+a+fractal+tree+video&&view=detail&mid=AA132E7501DB9EDE347BAA132E7501DB9EDE347B&&FORM=VRDGAR
	//
	//	Passed Parameters: int BranchLength - Starting branch length (in pixels,range: ~500-1500)
	//			
	//	Global Parameters:	int FR_X,FR_Y    float FR_Angle, FR_d_Angle, FR_d_Len
	//
	//	Returns: NOTHING
	//
	//	20181014 Ver 0.0	E.Andrews	First cut
	//
	
	//Draw branch at current angle and specified BranchLength
	int X0=FR_X,Y0=FR_Y;
	float Wiggle_Length=float(random(70,130))/100.;		//For a more natural look, randomly vary length(+/- 30%)
	int X1=X0+BranchLength*cos(FR_Angle)*Wiggle_Length;	//Compute branch end point
	int Y1=Y0+BranchLength*sin(FR_Angle)*Wiggle_Length;
	XYscope.plotLine(X0,Y0,X1,Y1);						//Draw the Branch
	//delay(50);	//<-----Uncomment this line to slow things down & watch the plotting in action
	FR_X=X1; FR_Y=Y1;									//Reset FR_X,FR_Y to end point of this branch
	//Plot another branch by recursive calls to plot_Fractal(Branch()
	if (BranchLength>40){	//Stop drawing more branches when they gets too short
		int Xtemp,Ytemp;float AngTemp;				//Create a 1-deep stack for X,Y,and Angle
		Xtemp=X1;Ytemp=Y1;AngTemp=FR_Angle;			//PUSH X,Y,Angle to stack	
		FR_Angle=FR_Angle+FR_d_Angle;				//Rotate branch angle Forward
		plot_Fractal_Branch(BranchLength*FR_d_Len);	//Recursive Call to Branch at ~1/3 shorter length
		FR_X=Xtemp; FR_Y=Ytemp; FR_Angle=AngTemp;	//POP X,Y,Angle from stack
		Xtemp=X1;Ytemp=Y1;AngTemp=FR_Angle;			//PUSH X,Y,Angle to stack
		FR_Angle=FR_Angle-FR_d_Angle;				//Rotate branch angle Backward
		plot_Fractal_Branch(BranchLength*FR_d_Len);	//Recursive Call to Branch at 1/3 smaller length
		FR_X=Xtemp; FR_Y=Ytemp; FR_Angle=AngTemp;	//POP X,Y,Angle from stack
	}
}

//=======================================================
//	SPLASH SCREEN DEMO
//=======================================================
void ArduinoSplash() {
	// Locate and plot the Arduino-Logo to screen.
	//
	//	Passed Parameters	NONE
	//
	//	Returns: NOTHING
	//
	//	20171008 Ver 0.0	E.Andrews	First cut
	//

	int Lx = 1150, Ly = 2000, Lht = 1000;
	XYscope.plotStart();
	XYscope.plotArduinoLogo(Lx, Ly, Lht);

}
//=========== END SPLASH =================================

void P1_PlotCenteringPattern(void) {
	// Plots a circle and square that can be used for scope centering and gain adjust
	//
	//	Passed Parameters	NONE
	//
	//	Returns: NOTHING
	//
	//	20170708 Ver 0.0	E.Andrews	First cut
	//	20170811 Ver 1.0	E.Andrews	Added 8X10 internal rectangle option

	XYscope.plotStart();
	int X_Center = 2047;
	int Y_Center = 2047;
	int Circ_Radius = 800;
	//Draw small circle
	XYscope.plotCircle(X_Center, Y_Center, Circ_Radius);		//Draw circle
	//Now draw large circle
	Circ_Radius = 2047;
	XYscope.plotCircle(X_Center, Y_Center, Circ_Radius);		//Draw a circle
	//Now draw large circle that is 80% of Full scale
	Circ_Radius = .8*2047;
	XYscope.plotCircle(X_Center, Y_Center, Circ_Radius);		//try a circle

	XYscope.plotPoint(X_Center, Y_Center);	// Put a DOT at the center of the circ.
	XYscope.plotLine(4095, 4095, 0, 4095);	//TOP Horizontal Border
	XYscope.plotLine(0, 4095, 4095, 0);		//TopLt to BotRt Diag
	XYscope.plotLine(4095, 0, 0, 0);		//BOTTOM Horizontal Border
	XYscope.plotLine(0, 0, 4095, 4095);		//TopRt to BotLt Diag

	XYscope.plotLine(4095, 0, 4095, 4095);	//RIGHT Vertical Border
	XYscope.plotLine(0, 0, 0, 4095);		//LEFT Vertical Border

	//Now draw rectangle to represent 10X8 screen aspect ratios
	int TempInt=XYscope.getGraphicsIntensity();
	XYscope.setGraphicsIntensity(20);
	XYscope.plotRectangle(0, .9*4096, 4095, .1 * 4096);
	XYscope.setGraphicsIntensity(TempInt);
	
	EndOfSetup_Ptr = XYscope.XYlistEnd;		//Remember end of buffer

	XYscope.plotEnd();						//end with plot to 0,0
	XYscope.autoSetRefreshTime();
	ShowMemory();
	PrintStatsToConsole();
}
void PowerUpSplash(){
	//Here is just some stuff to paint onto CRT at startup
	//v----------BEGIN SETUP SPLASH SCREEN ---------------v
	ArduinoSplash();				//Paint an Arduino logo
	int xC = 1800, yC = 2800;		//Set values of XY center coordinates for start of text
	int textSize = 400;				//Set Text Size (in pixels)
	bool const UndrLined = true;	//Turn underline ON
	int textBright = 250;
	XYscope.printSetup(xC - 150, yC + 50 + 700, textSize, textBright);
	XYscope.print((char *)"AGI", UndrLined);
	xC = 100;
	yC = 2900;
	textSize = 250;
	XYscope.printSetup(xC + 50, yC + 50 + textSize, textSize, textBright);
	XYscope.setFontSpacing(XYscope.prop);			//Select Proportional Spacing
	if (XYscope.getFontSpacing() != XYscope.mono)	//Adjust coordinate in currently active spacing mode is 'prop'
		XYscope.printSetup(xC + 50 + 500, yC + 50 + textSize, textSize,
				textBright);
	XYscope.print((char *)"Arduino Graphics Interface", false);	//(false=No underline)
	XYscope.setFontSpacing(XYscope.mono);

	XYscope.autoSetRefreshTime();
	XYscope.plotRectangle(0, 0, 4095, 4095);
	XYscope.printSetup(0, 1300, 175, 100);
	
	XYscope.print ("     Open Serial Monitor debug        window (115200 Baud,No line ending)  to communicate with this program.");
	XYscope.printSetup(350, 275, 175, 100);
	XYscope.print(startMsg);XYscope.print((char *)" LibRev:");XYscope.print(XYscope.getLibRev(),2);

	//Now put the compile date & time onto the screen
	//The time and data stamp come from #define statements at the top of the program...
	XYscope.printSetup(1100, 100, 150, 100);
	XYscope.print((char *)"(");
	XYscope.print(CompileDateStamp);
	XYscope.print((char *)"  ");
	XYscope.print(CompileTimeStamp);
	XYscope.print((char *)")");

	//^----------END SETUP SPLASH SCREEN ---------------^
}


//================ START OF CLOCK ROUTINES ========================//
void P23_BeginClockMode() {
	//  This routine demonstrates the basics of a CRT clock application.
	//	It shows how DrawClockHourHand(Hr), DrawClockMinuteHand(Min),
	//	and DrawClockSecondHand(Sec) routines can be written and used.
	//	Take note of how the XYscope.XYlistEnd is managed to animate and
	//	redraw the hands. Also, note that ClockHandsSetup(...) is a routine
	//	that is used to define the location and size parameters for hands.
	//
	//	Passed Parameters	NONE
	//
	//	Returns: NOTHING
	//
	//	20171106 Ver 0.0	E.Andrews	First cut
	//						(Minor updates throughout development without version change)
	//	20180710 Ver 1.0	E.Andrews	Now driven off of TIME routines!
	//						(Passed parameters are ignored)
	//	20181010 Ver 1.1	E.Andrews	Removed some test parameters and converted to BeginClockMode function
	//						Once called, program will stay in this routine unless a Serial link Key is pressed or
	//						one of the set clock push buttons is detected.
	//
	
	ResetPushButtons();
	int xCtr = 2047;
	int yCtr = 2047;
	int CenterRadius=100;			//was 200
	int Min_HubCenterRadius = 50;	//was 100
	int ClockRadius=1900;
	int HR_armRadius=ClockRadius-500;
	int MN_armRadius=ClockRadius;
	int SEC_armRadius=ClockRadius+120;
	int xRadius = 1900;
	int yRadius = 1900;
	
	XYscope.plotClear();	//Erase Screen
	XYscope.plotStart();	//Init for fresh plot!
	
	//Initialize ClockHands global variables
	ClockHandsSetup(xCtr,yCtr,xRadius,yRadius,CenterRadius,Min_HubCenterRadius, HR_armRadius,MN_armRadius,SEC_armRadius);
	//Draw the clock face
	DrawClockFace();
	Gbl_HR_ListPtr = XYscope.XYlistEnd;
	float CurMin=0.;	
	float CurHour=3.00;	
	float CurSec=0.;
	
	int PriorSec=-1;
	int PriorMin=minute()-1;	//Force a full face refresh
	int PriorHour=-1;

	
	int StayInClockMode=5;
	Serial.println();
	Serial.print("Starting CLOCK-MODE (Press a any key to EXIT CLOCK-MODE)");
	

	//--------------------------------------------------------------
	//     This is CLOCK MODE LOOP!
	//--------------------------------------------------------------
	while(StayInClockMode>0){
		ReadPushButtons();	//Check for push button event		
		if ( PB_IncrBtn_DownEdge == HIGH || PB_DecrBtn_DownEdge == HIGH || PB_NxtBtn_DownEdge== HIGH ||PB_EnterBtn_DownEdge==HIGH) {
		
			//Serial.println (">>>Begin CLOCK SET Function...");
			
			//For any button press, go into clock set mode
			ResetPushButtons();
			ClockSetWithPushButtons();	//This shows SET screen and updates the actual settings
										//Upon return from ClockSet, we Return to CLOCK MODE
			//Serial.println ("<<< Back from CLOCK SET");
			//Replot everything from scratch
			XYscope.plotStart();
			DrawClockFace();
			//Draw HR,MIN,& SEC
			
			//Draw HourHand
			XYscope.XYlistEnd=Gbl_HR_ListPtr;	//Set Buffer pointer to HOUR_ListPtr
			
			//Draw Digital Readout of Time			
			ClockPaintDigitalTimeDisplay(CurHour,CurMin);
			ClockPaintCurrentDayDate();
			DrawClockHourHand(CurHour+CurMin/60);
			
			//Capture MIN_XYlist_Pointer value
			Gbl_MIN_ListPtr = XYscope.XYlistEnd;
			//Draw Minute Hand
			DrawClockMinHand(CurMin);
			
			//Capture Second_XYlist_Pointer value
			Gbl_SEC_ListPtr =XYscope.XYlistEnd;
			
			//Draw Second Hand
			Gbl_SEC_ListPtr=XYscope.XYlistEnd;
			DrawClockSecHand(CurSec);
			
		}
		if (PriorSec!=second()){	//Did one second go by?
									//If yes, then we need to update the display
			CurSec=second();		//Update Sec Hand Value
			PriorSec=CurSec;
			CurMin=minute();		//Update Min Hand Value
			CurHour=hour();		//Update Hr Hand Value
	
			if (CurHour>12) CurHour=CurHour-12;	//Make it a 12 Hr Display

			if ((PriorMin == CurMin)&&(PriorHour==CurHour)){	//If minute and hour haven't changed, just update Second Hand
			
				//Update just SecondHand Plot 		
				XYscope.XYlistEnd = Gbl_SEC_ListPtr;
				DrawClockSecHand(CurSec);
		
			}
			else{	//if Minute has changed, we need to re-draw HR,MIN,& SEC
				//Update HourHand
				XYscope.XYlistEnd=Gbl_HR_ListPtr;
				//Now Update Digital Readout of Time			
				ClockPaintDigitalTimeDisplay(CurHour,CurMin);
				ClockPaintCurrentDayDate();
				DrawClockHourHand(CurHour+CurMin/60);
				//Update Minute Hand
				DrawClockMinHand(CurMin);
				//Update Second_Pointer
				Gbl_SEC_ListPtr =XYscope.XYlistEnd;
				//Update Second Hand
				Gbl_SEC_ListPtr=XYscope.XYlistEnd;
				DrawClockSecHand(CurSec);

				StayInClockMode=5;	//Keep it going forever!
				
				PriorMin=CurMin;	//Update PriorMin variable
				PriorHour=CurHour;
			}	

			
		}


		//Check to see if operator touched a key to stop the clock display
		if (Serial.available()){	//Did Oper touch a key? If yes, stop demo & dump the serial Buffer
			Serial.println (" *** Clock stopped by Oper Key Press ***");
			StayInClockMode=0;	//Stop the demo by setting counter to zero
			int d=0;
			while (d>=0) {			//clean out the buffer (Retrieved data is NOT USED)
				d=Serial.read();	//...flush buffer, keep reading until buffer is empty.
				
				
			} 						// (Serial.read returns -1 when no data is available)
		}
	//--------------------------------------------------------------
	//     END of CLOCK MODE LOOP, Oper pushed a key on monitor program
	//--------------------------------------------------------------
	}
	XYscope.plotStart();
	//How about a nice boarder for this screen...
	XYscope.setGraphicsIntensity(80);
	XYscope.plotRectangle(0,0,4095,4095);
	
	XYscope.setFontSpacing(0);	//Select Proportional mode
	XYscope.printSetup(300, 2500, 300, 100);
	XYscope.print("   CLOCK MODE STOPPED");
	XYscope.printSetup(150, 2000, 300, 100);
	XYscope.print("By Key press from Computer");
	XYscope.plotEnd();
	XYscope.autoSetRefreshTime();

	Serial.println(" ....EXITING CLOCK MODE by Oper Key press");

	XYscope.plotEnd();
	EndOfSetup_Ptr = XYscope.XYlistEnd - 1;
	XYscope.autoSetRefreshTime();
	//flush serial buffer
	if (Serial.available()){
		char d = Serial.read();	//Clear out read buffer contents and toss away
		d=d;					//This line added to remove "unused variable" compiler warning

	}
	Print_CRT_Scope_Main_Menu();//show text and diag menu on monitor screen
}

void ClockSetWithPushButtons(){
	//  This routine plots a SET TIME AND  DATE screen to scope CRT
	//	and ues operator through the set process using the 4-push button
	//	control panel.
	//
	//	Passed Parameters	NONE
	//
	//	Returns: NOTHING
	//
	//	20181010 Ver 1.1	E.Andrews	Initial cut
	//						Note: We stay in this routine until oper has completed set process 
	//						or a idle-button time-out has occurred
	//
	//Clear Screen and Start Fresh Plot...
	XYscope.plotStart();
	//Initialize SetTime variables to the current time
	int ST_hour=hour();		//Start with current Hour
	int ST_minute=minute();	//Start at current min
	int ST_second=second();	//Start at current Sec
	int ST_day = day();		//Start at current day
	int ST_month=month();		//Start at current month
	int ST_year=year();		//Start at current year
	// Define what HIGHLIGHT box we need to display
	int ST_State=0;			//0=Hr, 1=Min, 2=Sec, 3=Mnth, 4=Day, 5=Yr, 6=ACCEPT, 7=CANCEL, All others=ignored
	const int StateMax=7;
	boolean ClockSetCompleted=false;	//Define Success Flag
	bool DataValid=true;				//Define flag for data validation
	ResetPushButtons();
	//Show Set Time and Date Screen
	PlotToCRT_SetTimeDateScreen(ST_hour, ST_minute, ST_second, ST_month, ST_day,ST_year,ST_State);
	delay (500);
	const long SetMaxTime_ms=60000;	//Set Max Timeout value to 1 minute
	unsigned long OptionTimeOut_ms = millis() + SetMaxTime_ms;
	
	while (OptionTimeOut_ms > millis()){
		//Check for Console Keypress
		if (Serial.available()) OptionTimeOut_ms=millis()-5;	//Force a time out for console keypress!
		
		ReadPushButtons();	//Update push button states
		int ChangeCount = 0;
		boolean RedrawScreenReqd = false;
	
		if (PB_IncrBtn_DownEdge==HIGH){ //Increase Button Pushed
			PB_IncrBtn_DownEdge=LOW;	//Reset the edge detector...Only 
			ChangeCount=1;
			OptionTimeOut_ms = millis() + SetMaxTime_ms;	//Reset TimeOut Timer...
			RedrawScreenReqd=true;						
		}
		
		if (PB_DecrBtn_DownEdge==HIGH){ //Decrease Button Pushed
			PB_DecrBtn_DownEdge=LOW;	//Reset the edge detector...Only 
			ChangeCount=-1;
			OptionTimeOut_ms = millis() + SetMaxTime_ms;	//Reset TimeOut Timer...
			RedrawScreenReqd=true;
		}	

		if (PB_NxtBtn_DownEdge==HIGH){
			ST_State++;
			if (ST_State>StateMax) ST_State=0;		//wrap around and go back to the start
			RedrawScreenReqd=true;
			PB_NxtBtn_DownEdge=LOW;		//Reset the edge detector
			OptionTimeOut_ms = millis() + SetMaxTime_ms;	//Reset TimeOut Timer...
			RedrawScreenReqd=true;
		}
		if (PB_EnterBtn_DownEdge==HIGH && ST_State<6){
			//	If oper hits enter button but we're not at the ACCEPT or CANCEL display,
			//	treat it just like a "NEXT" button push
			ST_State++;
			RedrawScreenReqd=true;
			PB_EnterBtn_DownEdge=LOW;		//Reset the edge detector
			OptionTimeOut_ms = millis() + SetMaxTime_ms;	//Reset TimeOut Timer...
			RedrawScreenReqd=true;
		}
		switch (ST_State){
			case 0: //Adjust Hours
				ST_hour=ST_hour+ChangeCount;
				if (ST_hour>23) ST_hour=23;
				if (ST_hour<0) ST_hour=0;
				
				break;
				
			case 1:	//Adjust Minutes
				ST_minute=ST_minute+ChangeCount;
				if (ST_minute>59) ST_minute=59;
				if (ST_minute<0) ST_minute=0;
					
				break;
				
			case 2:	//Adjust Seconds
				ST_second=ST_second+ChangeCount;
				if (ST_second>59) ST_second=59;
				if (ST_second<0) ST_second=0;
					
				break;
			
			case 3:	//Adjust Month
				ST_month=ST_month+ChangeCount;
				if (ST_month>12) ST_month=12;
				if (ST_month<1) ST_month=1;
			
			
				break;
				
			case 4:	//Adjust Day
				ST_day=ST_day+ChangeCount;
				if (ST_day>31) ST_day=31;
				if (ST_day<1) ST_day=1;
				
			
				break;
				
			case 5:	//Adjust Year
				ST_year=ST_year+ChangeCount;
				if (ST_year>2099) ST_year=2099;
				if (ST_year<2000) ST_year=2000;					
				break;
				
			case 6:	//"ACCEPT"
				//Look for ENTER button here!
				if (PB_EnterBtn_DownEdge==HIGH){
					PB_EnterBtn_DownEdge=LOW;		//Reset the edge detector
					//Update time and data entry!
					ClockSetCompleted=false;
					DataValid=true;
					//Validate Time
					if (ST_hour>24 || ST_hour <0) DataValid=false;	//Make sure Hours are in range [0-23]
					if (ST_minute>59 || ST_minute <0) DataValid=false;	//Make sure Minutes are in range [0-59]	
					//Validate Date
					if (ST_month>12 || ST_month <1) DataValid=false;	//Make sure Month is in range [1-12]
					if (ST_day>31 || ST_day <1) DataValid=false;	//Make sure Day is in range [1-31]							
					if (ST_year<2000 || ST_year>2100) DataValid=false;	//Make sure YR are in range [2000-3100]						
		
		
					if (DataValid){
						//Data is valid
						
						setTime(ST_hour,ST_minute, ST_second,ST_day,ST_month,ST_year);
						now();	//Req'd to actually complete setting?
						Serial.println();
						Serial.print("    TIME set to: ");
						SerialPrintTime();
						
						Serial.print (" DATE set to:");
						SerialPrintDate(); 
						Serial.print (" now()=");Serial.println (now());
						#if defined (TeensyRTC_PRESENT)
							Teensy3Clock.set(now()); // set the Teensy RTC
							Serial.println();
							Serial.println ("   >>> TEENSY RTC has been Set! <<<");
						#endif
						ClockSetCompleted=true;
				
					}
					else{
						Serial.println();
						Serial.print(" <--- ERROR, invalid Data Entered (Time Not Changed!) ");
					}
					
					
					OptionTimeOut_ms = millis()-10;	//End loop by causing time out!
				}
				break;
			
			case 7:	//"CANCEL"
				//Look for Enter Button here!
				if (PB_EnterBtn_DownEdge==HIGH){
					PB_EnterBtn_DownEdge=LOW;		//Reset the edge detector
					//Update time and data entry!
					OptionTimeOut_ms = millis()-10;	//End loop by causing time out!
				}
			
				break;
			
			default:
				break;
		}
		// Update screen based on oper input as needed.
		if (RedrawScreenReqd)  {
			XYscope.plotStart();	//Start with a fresh screen
			PlotToCRT_SetTimeDateScreen(ST_hour, ST_minute, ST_second, ST_month, ST_day,ST_year,ST_State);
			RedrawScreenReqd=false;
		}
	
		
	}
	
	//Serial.println(" Clock Set Function Terminated!");
	//PrintStatsToConsole();
	ClearOperInputArray();
	XYscope.plotStart();
	if (ClockSetCompleted){	//
		XYscope.plotStart();
		//How about a nice boarder for this screen...
		XYscope.setGraphicsIntensity(80);
		XYscope.plotRectangle(0,0,4095,4095);
		
		XYscope.setFontSpacing(0);	//Select Proportional mode
		XYscope.printSetup(300, 2500, 300, 100);
		XYscope.print("   CLOCK SET COMPLETED");
		XYscope.printSetup(150, 2000, 300, 100);
		XYscope.print("     (Time & Date Updated)");
		XYscope.plotEnd();
		
		delay(3000);
		ResetPushButtons();
	}
	else{
			XYscope.plotStart();
			//How about a nice boarder for this screen...
			XYscope.setGraphicsIntensity(80);
			XYscope.plotRectangle(0,0,4095,4095);
			
			XYscope.setFontSpacing(0);	//Select Proportional mode
			XYscope.printSetup(300, 2500, 300, 100);
			XYscope.print("    CLOCK-SET STOPPED");
			XYscope.printSetup(150, 2000, 300, 100);
			XYscope.print("      (Cancelled by Operator)");
			XYscope.plotEnd();
		
		delay(5000);
		ResetPushButtons();
	}
}
void ClockPaintDigitalTimeDisplay(int DigitalHour,int DigitalMin){
	//  Routine that displays digital HOURS:MINUTES to CRT screen.
	//
	//	Passed Parameters	int DigitalHour,int DigitalMin
	//
	//	Returns: NOTHING
	//
	//	20181010 Ver 1.1	E.Andrews	General clean up at time of release
	//
	//
	XYscope.setFontSpacing(0);
	int TimeHt=800;	//Ht should be in 450 to 800
	int TimeIntensity = 250;	//Set to desired display intensity...Typically 90-200
	float numTimeCharacters = 4.;
	if (Gbl_Show_Digital_Time){	//If true, show digital time on face of clock, otherwise, skip it
		if (DigitalHour>12.)DigitalHour=DigitalHour-12.;	//Incase we get hour in a 24 Hr format
		if (DigitalHour<10.) numTimeCharacters=4;
		if ((DigitalHour>9.)) numTimeCharacters=5;//Do we have a two digit Hour?
		float Xstart=2047-int((numTimeCharacters*float(TimeHt)/3.8));	//This horizontally centers TIME display on screen
		// Diag printout to set spacing right...
		//Serial.print(" Hour,NumChar,Xstart ");Serial.print (int(DigitalHour));Serial.print(", ");Serial.print(numTimeCharacters);Serial.print(", ");Serial.println(Xstart);
		
		XYscope.printSetup(int(Xstart),2047+400,TimeHt,TimeIntensity);
		XYscope.setFontSpacing(XYscope.prop);	//Use proportional Spacing
		XYscope.print(int(DigitalHour));
		//Add leading ZERO to "minutes" display if needed
		if (DigitalMin<10)XYscope.print((char *) ":0"); else XYscope.print((char *) ":");
		XYscope.print(int(DigitalMin));
		//=== SUPRESS SECONDS DIGITAL DISPLAY
		//Add leading ZERO to "Seconds" display if needed
		//if (CurSec<10)XYscope.print((char *) ":0"); else XYscope.print((char *) ":");
		//XYscope.print(int(CurSec));	
	}
}
void ClockPaintCurrentDayDate(){
	//  Routine that displays current day and date to CRT screen.
	//  Format:           Day
	//                 Month d,yyyy
	//
	//	Passed Parameters	NONE	Returns: NOTHING
	//
	//	20181010 Ver 0.0	E.Andrews	General clean up at time of release
	//
	//

	int TimeHt=300;	//Ht should be in 450 to 800 range
	int TimeIntensity = 100;	//Set to desired display intensity...Typically 90-200
	int numTimeCharacters = 6;
	XYscope.printSetup(2047-(TimeHt/2)*(numTimeCharacters/2),2047+250,TimeHt,TimeIntensity);
	XYscope.setFontSpacing(XYscope.prop);	//Use proportional Spacing
	//Print day of week if enabled
	if(Gbl_Show_DayOfWeek) XYscope.print(dayStr(weekday()));XYscope.print("  ");
	
	//Print Month, Day, Year if enabled
	numTimeCharacters = 15;
	XYscope.printSetup(2047-(TimeHt/2)*(numTimeCharacters/2),2047-250-TimeHt,TimeHt,TimeIntensity);
	if(Gbl_Show_Date) {
		XYscope.print(monthStr(month()));XYscope.print(" ");
		XYscope.print(day());XYscope.print(", ");XYscope.print(year());
	}
	
}

void ClockHandsSetup(int X_Center, int Y_Center, int X_FaceRadius, int Y_FaceRadius, int Center_Radius,int MinCenterRadius, int Hour_Length, int Min_Length, int Sec_Length){
	//	Routine to set global variables used to drive the clock hands display
	//
	//	Calling parameters (All coordinates, radius, and length parameters are in "pixels")
	//		X_Center, Y-Center - These are the coordinates of the center of the clock
	//		Center_Radius - Defines the radius of the center of the Hour hand
	//		MinCenterRadius - Defines the radius for center of the Minute hand
	//		Hour_Length, Min_Length - Defines the length of the Hour/Minute hands
	//
	//	Returns:
	//		NOTHING
	//
	//	20171106 Ver 0.0	E.Andrews	First cut

	Gbl_X_Center=X_Center;
	Gbl_Y_Center=Y_Center;
	Gbl_X_FaceRadius =X_FaceRadius;
	Gbl_Y_FaceRadius = Y_FaceRadius;
	Gbl_Center_Radius=Center_Radius;
	Gbl_MinHubCenterRadius=MinCenterRadius;
	Gbl_Hour_HandLength=Hour_Length;
	Gbl_Min_HandLength=Min_Length;
	Gbl_Sec_HandLength=Sec_Length;


}
void DrawClockMinHand(float CurrentMinutes){
	//	CLOCK routine that plots a minute-hand
	//
	//	Passed Parameters:
	//		float Minutes	This is the current minutes value, 0<= Minutes <= 59
	//						If this value is out of range, 0 will be the minutes value used
	//
	//	This routine uses the following global parameters:
	//		float Gbl_X_Center, Gbl_Y_Center	- This is the XY center of the clock face.
	//		float Gbl_Center_Radius			- Radius of the central clock face 'circle'
	//		float Gbl_Min_HandLength			- This is the length of the clock arm
	//		int XYscope.ListEnd					- Standard XY_List array pointer
	//
	//	Returns: NOTHING
	//
	//	20171106 Ver 0.0	E.Andrews	First cut
	//
	int X0, Y0, X1, Y1;
	float mins;
	double TwoPi=atan(1)*8;

	//Figure out the Theta, the ANGLE (in radians) of the hand based on the passed parameter.FYI: 0=12 o'clock, Pi=6 o'clock, etc...
	mins=CurrentMinutes;
	if (mins<0 or mins>59) mins=0;			//Bound the incoming value
	double theta = TwoPi * mins/60.;		//This is the angle of the Minute hand
	//theta = const_2pi / 60. * seconds;	//This is the angle of the Second Hand
	//theta = const_2pi / 12. * hours;		//This is the angle of the Hour Hand

	//Calculate the Coordinates for the center point of the END of the hand
	//Gbl_Hour_End_X_Center = Gbl_X_Center + Gbl_Hour_HandLength * sin(theta);	//=+(Xc0)+Hand_Len*SIN(Theta)
	//Gbl_Hour_End_Y_Center = Gbl_Y_Center + Gbl_Hour_HandLength * cos(theta);	//=+(Yc0)+Hand_Len*COS(Theta)
	Gbl_Min_End_X_Center = Gbl_X_Center + Gbl_Min_HandLength * sin(theta);	//=+(Xc0)+Hand_Len*SIN(Theta)
	Gbl_Min_End_Y_Center = Gbl_Y_Center + Gbl_Min_HandLength * cos(theta);	//=+(Yc0)+Hand_Len*COS(Theta)



	//Calculate the Coordinates for LINE#1 of arm and draw this line	(min and hour hand only)
	X0= int(Gbl_X_Center - Gbl_MinHubCenterRadius*cos(theta));					//+=Xc0-Radius_1*COS(Theta)
	Y0= int(Gbl_Y_Center + Gbl_MinHubCenterRadius*sin(theta));					//=+Yc0-Radius_1*SIN(Theta)
	X1=	int(Gbl_Min_End_X_Center);			//=+(Xc1_)-Radius_2*COS(Theta)
	Y1=	int(Gbl_Min_End_Y_Center);			//=Yc1_+Radius_2*SIN(Theta)
	XYscope.plotLine(X0,Y0,X1,Y1);

	//Calculate the Coordinates for LINE#2 of arm and draw this line	(min and hour hand only)
	X0= int(Gbl_X_Center + Gbl_MinHubCenterRadius*cos(theta));					//=+Xc0+Radius_1*COS(Theta)
	Y0= int(Gbl_Y_Center - Gbl_MinHubCenterRadius*sin(theta));					//=+Yc0-Radius_1*SIN(Theta)
	X1=	int(Gbl_Min_End_X_Center);			//=+Xc1_+Radius_2*COS(Theta)
	Y1=	int(Gbl_Min_End_Y_Center);			//=+Yc1_-Radius_2*SIN(Theta)
	XYscope.plotLine(X1,Y1,X0,Y0);	//Reverse order of vector for best plot quality
}

void DrawClockFace(){
	//	DRAW CLOCK FACE routine
	//
	//	Passed Parameters:
	//		none
	//
	//	This routine uses the following global parameters:
	//	float Gbl_X_Center				//X center of clock face
	//	float Gbl_Y_Center				//Y center of clock face
	//	float Gbl_X_FaceRadius			//X Face Radius
	//	float Gbl_Y_FaceRadius			//Y Face Radius (Same as x)
	//	float Gbl_Center_Radius			//Radius of HOURS center-circle
	//	float Gbl_MinHubCenterRadius	//Radius of MINUTS center-circle
	//	float Gbl_Hour_HandLength		//Hand Length
	//	float Gbl_Min_HandLength		//Hand Length
	//	float Gbl_Sec_HandLength		//Hand Length
	//
	//	Returns: NOTHING
	//
	//	20181001 Ver 0.0	E.Andrews	Make this a subroutine from P23 demo code
	//
	//	Here is where we draw the fixed, CLOCK FACE parts...
	//Serial.print(" Run: CRT-Clock-Sim Routines...");
	float numTestPoints = 12;	//We want to have 12 text Hour Markers
	float testAngle = 0;
	int patternCount = 0;
	float angleStep = 2 * 3.141526 / numTestPoints;
	testAngle = testAngle - angleStep;
	const int charHt = 450;	//sets the of hour numbers on face.  Usable Range is 300-500
	int HoursIntensity = 0;
	int OldTextIntensity = XYscope.getTextIntensity();
	XYscope.setGraphicsIntensity(100);
	for (patternCount = 0; patternCount < numTestPoints; patternCount++) {
		HoursIntensity = HoursIntensity + 20;	//Just for fun and test purposes, change
												//intensity of every hour value. Start at
												//20% and step brightness for each HOUR.
												//A real CRT clock will probably want to
												// have one intensity for all hour markers.
		HoursIntensity=100;	//Comment this line out if you want to plot each hour at different level
		
		if (HoursIntensity > 255)  HoursIntensity = 255;
		testAngle = testAngle + angleStep;
		int xP, yP;
		xP = int(Gbl_X_Center + Gbl_X_FaceRadius * sin(testAngle));
		yP = int(Gbl_Y_Center + Gbl_Y_FaceRadius * cos(testAngle));

		XYscope.plotCircle(xP, yP, 40);	//Plot small circles at HOUR marks
		// Now Plot Hour numbers 1-12.  Change in text-plot-radius
		// and offsets were needed to get the numbers in the right place...
		int xDelta = -350;
		int yDelta = -350;
		int xOffset = -charHt / 2;
		int yOffset = -charHt / 2;
		xP = int(Gbl_X_Center + xOffset + (Gbl_X_FaceRadius + xDelta) * sin(testAngle));
		yP = int(Gbl_Y_Center + yOffset + (Gbl_Y_FaceRadius + yDelta) * cos(testAngle));

		//Plot HOURS text
		if (patternCount > 0 && patternCount < 7)
			XYscope.printSetup(xP + 150, yP, charHt, HoursIntensity);
		else
			XYscope.printSetup(xP, yP, charHt, HoursIntensity);
		if (patternCount == 0) {
			XYscope.printSetup(xP, yP, charHt, HoursIntensity);
			XYscope.print((char *)"12");
		} else
			XYscope.print(patternCount);
	}
	//Restore prior Text Intensity
	XYscope.setTextIntensity(OldTextIntensity);

	//Draw short dashed lines for each of the SECOND marks
	numTestPoints = 60;
	angleStep = 2 * 3.141526 / numTestPoints;
	for (patternCount = 0; patternCount < numTestPoints; patternCount++) {
		testAngle = testAngle + angleStep;
		int xP1, yP1, xP2, yP2;
		xP1 = int(Gbl_X_Center + (Gbl_X_FaceRadius + 50) * sin(testAngle));
		yP1 = int(Gbl_Y_Center + (Gbl_Y_FaceRadius + 50) * cos(testAngle));
		xP2 = int(Gbl_X_Center + (Gbl_X_FaceRadius - 50) * sin(testAngle));
		yP2 = int(Gbl_Y_Center + (Gbl_Y_FaceRadius - 50) * cos(testAngle));

		//Note: Here we skip plotting the SECONDS-MARK at each of the HOUR marks
		if ((patternCount + 1) % 5 != 0)
			XYscope.plotLine(xP1, yP1, xP2, yP2);
	}
	//Uncomment the following four (4) lines if you want add the Arduino-logo on the screen...
	//int logoHt = 1000;
	//int logoX = 2047 - logoHt * .85;
	//int logoY = 2047 - logoHt;
	//XYscope.plotArduinoLogo(logoX, logoY, logoHt);

	//Draw the center hubs of the HOURS and MINITE hands...
	XYscope.plotCircle(Gbl_X_Center, Gbl_Y_Center, Gbl_Center_Radius-5);		//Hour Hand Center Circle ('-5' just makes it look nicer!)
	XYscope.plotCircle(Gbl_X_Center, Gbl_Y_Center, Gbl_MinHubCenterRadius-5);	//Minute Hand Center Circle
	XYscope.plotCircle(Gbl_X_Center, Gbl_Y_Center, 10);							//Second-Hand Center Circle
	//Post message on how to stop demo...
	XYscope.printSetup(10,10,200,100);
	XYscope.setFontSpacing(XYscope.prop);
	//XYscope.print((char*)"(Key Press=Stop)");
}	//---------------------------END DrawClockFace------------------------------------

void DrawClockHourHand(float CurrentHour){
	//	CLOCK routine that plots the Hour-hand
	//
	//	Passed Parameters:
	//		float Hours		This is the current Hours value, 0<= Hour <= 12
	//						If this value is out of range, 12 will be the Hours value used
	//						Note: An Hours value such as 3.5 will put hour hand at 3:30 position.
	//
	//	This routine uses the following global parameters:
	//		float Gbl_X_Center, Gbl_Y_Center	- This is the XY center of the clock face.
	//		float Gbl_Center_Radius				- Radius of the central clock face 'circle'
	//		float Gbl_Hour_HandLength			- This is the length of the clock arm
	//		float Gbl_Hour_EndRadius			- Radius of the circle plotted at the 'end' of the clock arm
	//		int XYscope.ListEnd					- Standard XY_List array pointer
	//
	//	Returns: NOTING
	//
	//	20171106 Ver 0.0	E.Andrews	First cut
	//	20180710 Ver 0.1	E.Andrews	Accepts Hours in 24 hour format(displays onto 12-hour face)
	//
	int X0, Y0, X1, Y1;
	float Hours;
	double TwoPi=atan(1)*8;

	//Figure out the Theta, the ANGLE (in radians) of the hand based on the passed parameter.FYI: 0=12 o'clock, Pi=6 o'clock, etc...
	Hours=CurrentHour;
	if (Hours<0 or Hours>12) Hours=Hours-12;	//Bound the incoming value
	//double theta = TwoPi * mins/60.;			//This is the angle of the Minute hand
	//double theta = TwoPi * seconds / 60.;		//This is the angle of the Second Hand
	double theta = TwoPi * Hours/ 12.;			//This is the angle of the Hour Hand

	//Calculate the Coordinates for the center point of the END of the hand
	Gbl_Hour_End_X_Center = Gbl_X_Center + Gbl_Hour_HandLength * sin(theta);	//=+(Xc0)+Hand_Len*SIN(Theta)
	Gbl_Hour_End_Y_Center = Gbl_Y_Center + Gbl_Hour_HandLength * cos(theta);	//=+(Yc0)+Hand_Len*COS(Theta)
	//Gbl_Min_End_X_Center = Gbl_X_Center + Gbl_Min_HandLength * sin(theta);	//=+(Xc0)+Hand_Len*SIN(Theta)
	//Gbl_Min_End_Y_Center = Gbl_Y_Center + Gbl_Min_HandLength * cos(theta);	//=+(Yc0)+Hand_Len*COS(Theta)



	//Calculate the Coordinates for LINE#1 of arm and draw this line	(min and hour hand only)
	X0= int(Gbl_X_Center - Gbl_Center_Radius*cos(theta));					//+=Xc0-Radius_1*COS(Theta)
	Y0= int(Gbl_Y_Center + Gbl_Center_Radius*sin(theta));					//=+Yc0-Radius_1*SIN(Theta)
	X1=	int(Gbl_Hour_End_X_Center);			//=+(Xc1_)-Radius_2*COS(Theta)
	Y1=	int(Gbl_Hour_End_Y_Center);			//=Yc1_+Radius_2*SIN(Theta)
	XYscope.plotLine(X0,Y0,X1,Y1);


	//Calculate the Coordinates for LINE#2 of arm and draw this line	(min and hour hand only)
	X0= int(Gbl_X_Center + Gbl_Center_Radius*cos(theta));					//=+Xc0+Radius_1*COS(Theta)
	Y0= int(Gbl_Y_Center - Gbl_Center_Radius*sin(theta));					//=+Yc0-Radius_1*SIN(Theta)
	X1=	int(Gbl_Hour_End_X_Center);			//=+Xc1_+Radius_2*COS(Theta)
	Y1=	int(Gbl_Hour_End_Y_Center);			//=+Yc1_-Radius_2*SIN(Theta)
	XYscope.plotLine(X1,Y1,X0,Y0);	//Reverse order of vector for best plot quality
}
void DrawClockSecHand (int Secs){
	//	Clock Routine that plots the SECONDS-hand
	//
	//	Passed Parameters:
	//		int Secs		This is the current Secs value, 0<= Secs <= 59
	//						If this value is out of range, 0 will be used
	//
	//	This routine uses the following global parameters:
	//		float Gbl_X_Center, Gbl_Y_Center	- This is the XY center of the clock face.
	//		float Gbl_Center_Radius			- Radius of the central clock face 'circle'
	//		float Gbl_Secs_HandLength			- This is the length of the clock arm
	//		int XYscope.ListEnd					- Standard XY_List array pointer
	//
	//	Returns: NOTING
	//
	//	20171106 Ver 0.0	E.Andrews	First cut
	//
	double TwoPi=atan(1)*8;
	float angle= (TwoPi*Secs) / (60);

	int X_end=Gbl_X_Center+Gbl_Sec_HandLength*sin(angle);
	int Y_end=Gbl_Y_Center+Gbl_Sec_HandLength*cos(angle);
	//Plot a simple vector (line) as the second hand
	XYscope.plotLine(Gbl_X_Center, Gbl_Y_Center, X_end, Y_end);


}
//================ END SIMULATED CLOCK ROUTINES ========================//

void P21_SimCoordinateSys(void) {
	//	Routine to plot a sample graphic to show the AGI (X,Y) Coordinate system
	//
	//	Passed Parameters	NONE
	//
	//	Returns: NOTHING
	//
	//	20170811 Ver 0.0	E.Andrews	First cut
	//	20180521			E. Andrws	Checked with TEENSY release
	int xC, yC;
	const boolean UndrLined = true;

	XYscope.plotClear();
	XYscope.plotStart();
	XYscope.setGraphicsIntensity(10);
	XYscope.plotRectangle(0, 0, 4095, 4095);

	XYscope.setGraphicsIntensity(150);
	int textSize = 250, textBright = 150;

	xC = 0;
	yC = 0;
	PlotCrossHair(xC, yC);
	XYscope.printSetup(xC + 100, yC + 40, textSize, textBright);
	XYscope.print((char *)"(");
	XYscope.print(xC);
	XYscope.print((char *)",");
	XYscope.print(yC);
	XYscope.print((char *)")");

	xC = 0;
	yC = 4095;
	PlotCrossHair(xC, yC);
	XYscope.printSetup(xC + 100, yC - 50 - textSize, textSize, textBright);
	XYscope.print((char *)"(");
	XYscope.print(xC);
	XYscope.print((char *)",");
	XYscope.print(yC);
	XYscope.print((char *)")");

	xC = 4095;
	yC = 4095;
	PlotCrossHair(xC, yC);
	XYscope.printSetup(xC - 50 - (11 * (textSize * .65)), yC - 50 - textSize,
			textSize, textBright);
	XYscope.print((char *)"(");
	XYscope.print(xC);
	XYscope.print((char *)",");
	XYscope.print(yC);
	XYscope.print((char *)")");

	xC = 4095;
	yC = 0;
	PlotCrossHair(xC, yC);
	XYscope.printSetup(xC - 50 - (8 * (textSize * .65)), yC + 50, textSize,
			textBright);
	XYscope.print((char *)"(");
	XYscope.print(xC);
	XYscope.print((char *)",");
	XYscope.print(yC);
	XYscope.print((char *)")");

	xC = 2047;
	yC = 2047;
	PlotCrossHair(xC, yC);
	XYscope.printSetup(xC + 50, yC - 50 - textSize, textSize, textBright);
	XYscope.print((char *)" (");
	XYscope.print(xC);
	XYscope.print((char *)",");
	XYscope.print(yC);
	XYscope.print((char *)")");

	xC = 1023;
	yC = 1023;
	PlotCrossHair(xC, yC);
	XYscope.printSetup(xC + 50, yC - 50 - textSize, textSize, textBright);
	XYscope.print((char *)" (");
	XYscope.print(xC);
	XYscope.print((char *)",");
	XYscope.print(yC);
	XYscope.print((char *)")");

	xC = 1650;
	yC = 2047;

	textSize = 400;
	textBright = 150;
	XYscope.printSetup(xC + 50, yC + 50 + 700, textSize, textBright);
	XYscope.print((char *)"AGI", UndrLined);
	xC = 100;
	yC = 2047;
	textSize = 350;
	XYscope.printSetup(xC + 50, yC + 50 + textSize, textSize, textBright);
	if (XYscope.getFontSpacing() != XYscope.mono)
		XYscope.printSetup(xC + 50 + 500, yC + 50 + textSize, textSize,
				textBright);
	XYscope.print((char *)"Coordinate System", UndrLined);

	//PlotCrossHair(2047,2047);

	XYscope.plotEnd();
	XYscope.autoSetRefreshTime();	//Only used when DUE present; Not req'd for TEENSY
	ShowMemory();
	PrintStatsToConsole();
}

void PlotCrossHair(int Xcoord, int Ycoord) {
	//	Routine to draw a cross hair symbol to screen.  Size is fixed by const definitions.
	//            |
	//          - * -
	//            |
	//	Calling parameters (All coordinate parameters are in "pixels")
	//		X_Coord, Ycoord - Defines the XY-Center of the symbol
	//
	//	Returns:
	//		NOTHING
	//
	//	20171022 Ver 0.0	E.Andrews	First cut
	
	const int LineLeng = 100;
	const int LineSpace = 50;
	const int DotRepeatCnt = 10;
	int x0, y0, x1, y1;
	//Xcoord=Xcoord && 4095;	//Make sure coordinates are ON-SCREEN
	//Ycoord=Ycoord && 4095;	//Make sure coordinates are ON-SCREEN

	//Plot the target Point...Make it Bright by plotting multiple times
	for (int i = 0; i < DotRepeatCnt; i++) {
		XYscope.plotPoint(Xcoord, Ycoord);
	}

	//Plot Left Horizontal Cross-hair line
	x0 = Xcoord - LineSpace;
	x1 = Xcoord - (LineSpace + LineLeng);
	if (x1 < 0)
		x1 = 0;
	y0 = Ycoord;
	y1 = Ycoord;
	if (x0 >= 0 && x1 <= 4095)
		XYscope.plotLine(x0, y0, x1, y1);

	//Plot Right Horizontal Cross-hair line
	x0 = Xcoord + LineSpace;
	x1 = Xcoord + (LineSpace + LineLeng);
	if (x1 > 4095)
		x1 = 4095;
	if (x0 >= 0 && x1 <= 4095)
		XYscope.plotLine(x0, y0, x1, y1);

	//Plot Top Vertical Cross-hair line
	x0 = Xcoord;
	x1 = Xcoord;
	if (x1 < 0)
		x1 = 0;
	y0 = Ycoord + LineSpace;
	y1 = Ycoord + (LineSpace + LineLeng);
	if (y1 > 4095)
		y1 = 4095;
	if (y0 >= 0 && y1 <= 4095)
		XYscope.plotLine(x0, y0, x1, y1);

	//Plot Bottom Vertical Cross-hair line
	y0 = Ycoord - LineSpace;
	y1 = Ycoord - (LineSpace + LineLeng);
	if (y1 < 0)
		y1 = 0;
	if (y0 >= 0 && y1 >= 0)
		XYscope.plotLine(x0, y0, x1, y1);

}

void P22_SimGraph_0(void) {
	//	Routine to plot a sample graphic chart display
	//
	//	Passed Parameters	NONE
	//
	//	Returns: NOTHING
	//
	//	20170811 Ver 0.0	E.Andrews	First cut
	//
	//Serial.print(" ERASING Data Table...");
	XYscope.plotClear();
	XYscope.plotStart();
	XYscope.plotRectangle(500, 500, 4000, 3500);

	for (int yLine = 1000; yLine < 3501; yLine = yLine + 500) {
		XYscope.printSetup(20, yLine - 100, 200);
		XYscope.print(yLine / 100);
		XYscope.plotLine(450, yLine, 4000, yLine);
	}

	for (int xLine = 1000; xLine < 4001; xLine = xLine + 500) {
		XYscope.printSetup(xLine - 700, 200, 200);
		XYscope.print(float(xLine) / 1000., 1);
		XYscope.plotLine(xLine, 450, xLine, 3500);
	}
	int yPoint = 2000;
	for (int xPoint = 500; xPoint < 4001; xPoint = xPoint + 2) {
		yPoint = 2000 + 1200 * sin(float(xPoint) / 500.);
		XYscope.plotPoint(xPoint, yPoint);
	}
	XYscope.printSetup(100, 3800, 250);
	XYscope.print((char *)"      Flux-Capacitance");
	XYscope.printSetup(100, 3600, 250);
	XYscope.print((char *)"       Vs Giga-Watts");
	XYscope.plotEnd();
	XYscope.autoSetRefreshTime();
	ShowMemory();
	PrintStatsToConsole();
}

void P4_PeakToPeak_Vert() {
	//	Routine to plot a Vertical edge-to-edge Freq Response Test pattern
	//	   +--+  +--+  +--+  +--+
	//     |  |  |  |  |  |  |  |
	//   ..+  +--+  +--+  +--+  +...
	//	Passed Parameters	NONE
	//
	//	Returns: NOTHING
	//
	//	20170811 Ver 0.0	E.Andrews	First cut
	//
	XYscope.plotStart();
	//Dashed Line, Top and Bottom
	for (int i = 0; i < 4095; i = i + 256) {
		XYscope.plotLine(i, 4095, i + 127, 4095);//Dashed line at Top of Screen
		XYscope.plotLine(i + 128, 0, i + 255, 0);//Dashed Line at Bottom of Screen
	}
	Serial.println(" DONE PeakToPeak_Vert Sq Wave ");
}
void P3_PeakToPeak_Settle_Unblk_Adj() {
	//	Routine to plot a Vertical edge-to-edge Freq Response Test pattern
	//  This pattern increase like a stair case, with each step 256 larger
	//	than the one before it.  It is most useful for evaluating and setting
	//	the LARGE STEP THRESHOLD (nnn L) and LARGE STEP SETTLING TIME (nnn S)
	//	adjustments.
	//	   					 +--+
	//     			   +--+  |  |
	//           +--+  |  |  |  |
	//     +--+  |  |  |  |  |  |
	//     |  |  |  |  |  |  |  |	
	//   ..+  +--+  +--+  +--+  +...
	//	Passed Parameters	NONE
	//
	//	Returns: NOTHING
	//
	//	20170811 Ver 0.0	E.Andrews	First cut. Useful when trying different TEENSY timing setups
	//									particularly when testing Big/Small-Step settling Values
	//
	XYscope.plotStart();
	//Dashed Line, Top and Bottom
	int yTop = 16;
	int yStep =	256;
	for (int i = 0; i < 4095; i = i + 256) {
		XYscope.plotLine(i, yTop, i + 127, yTop);//Dashed line at Top of Screen
		XYscope.plotLine(i + 128, 0, i + 255, 0);//Dashed Line at Bottom of Screen
		yTop=yTop+yStep;
	}
	Serial.println(" DONE Vert_Stair-case Sq Wave ");
}

void P5_PeakToPeak_Horiz() {
	//	Routine to plot Horizontal edge-to-edge Freq Response Test pattern
	//	:
	//  +----+
	//       |
	//	+----+
	//	|
	//  +----+
	//       |
	//	+----+
	//	:
	//
	//	Passed Parameters	NONE
	//
	//	Returns: NOTHING
	//
	//	20170811 Ver 0.0	E.Andrews	First cut
	//
	XYscope.plotStart();
	//Dashed Line, Left & Right
	for (int i = 0; i < 4095; i = i + 256) {
		XYscope.plotLine(0, i, 0, i + 127);	//Dashed line at Left Side of Screen
		XYscope.plotLine(4095, i + 128, 4095, i + 255);	//Dashed Line at Right Side of Screen
	}
	Serial.println(" DONE P5_PeakToPeak_Horiz Sq Wave ");
}

void P2_PeakToPeak_Dots() {
	//	Routine to plot just dots in the corners of the screen.
	//	This pattern was used to vary and optimize the blanking timing
	//	parameters.	
	//	
	//    *            *
	//      
	//	
	//	
	//       
	//    *            *
	//	
	//
	//	Passed Parameters	NONE
	//
	//	Returns: NOTHING
	//
	//	20170811 Ver 0.0	E.Andrews	First cut
	//

	XYscope.plotStart();
	//Dots in lower left hand corner and upper right hand corner of display screen
	//Used to measure frequency response of the DACs

	XYscope.plotPoint(0, 0);
	XYscope.plotPoint(4095, 4095);
	XYscope.plotPoint(0, 0);
	XYscope.plotPoint(4095, 4095);
	XYscope.plotPoint(0, 0);
	XYscope.plotPoint(4095, 4095);
	XYscope.plotPoint(0, 0);
	XYscope.plotPoint(4095, 4095);
	XYscope.plotPoint(0, 0);
	XYscope.plotPoint(4095, 4095);
	XYscope.plotPoint(0, 0);
	XYscope.plotPoint(4095, 4095);
	XYscope.plotPoint(0, 0);
	XYscope.plotPoint(4095, 4095);
	XYscope.plotPoint(0, 0);
	XYscope.plotPoint(4095, 4095);
	XYscope.plotEnd();

	Serial.println(" DONE PeakToPeak Corner_to_Corner Test ");
}

#if defined (TeensyEEPROM_PRESENT)
	//============== EEPROM Read/Write Routines [Only compile these routines if TeensyEEPROM_PRESENT]

	void InitializeTiming_To_EEpromDataSet(){
		//	Routine over-writes Timing Values to EEprom values
		//	but only when EEPROM has a revision 1.00 DataSet code
		//
		//	Passed Parameters	NONE
		//
		//	Returns: 	NOTHING
		//
		//	20180731 Ver 0.0 		 	E.Andrews	Checked for use with TEENSY release (DataStructRev=1.00).
		//	20180731 Ver 0.1 		 	E.Andrews	Updated to used DataStructRev=2.00 (expanded to include clock display parameters)		
		//	
		if(showDueOptions!=true){ //Don't try to read EEprom if we're running on an Arduino DUE!
			GBL_EE_SetupData.DataStructRev = 0.0;		//Init in case we can't actually read EEPROM
			EEPROM.get(eeStartAdr,GBL_EE_SetupData);	//Read data structure from EEpromData
			Serial.print (" InitializeTiming_To_EEpromDataSet -- DataStructRev = ");Serial.println(GBL_EE_SetupData.DataStructRev);
			if (GBL_EE_SetupData.DataStructRev == 1.00){  //Make sure data set rev is valid - If yes, init all timing values
				XYscope.PioSmallSettleCount = GBL_EE_SetupData.PioSmallSettleCount;
				XYscope.PioLargeSettleCount = GBL_EE_SetupData.PioLargeSettleCount;
				XYscope.NoSettlingTimeReqd = GBL_EE_SetupData.PioBreakpointCount;
				XYscope.PioUnblankCount = GBL_EE_SetupData.PioUnblankCount;
				Serial.print ("Initialized from EEPROM Ver: ");Serial.println(GBL_EE_SetupData.DataStructRev);
			}
			if (GBL_EE_SetupData.DataStructRev == 2.00){  //Make sure data set rev is valid - If yes, init all timing values
				XYscope.PioSmallSettleCount = GBL_EE_SetupData.PioSmallSettleCount;
				XYscope.PioLargeSettleCount = GBL_EE_SetupData.PioLargeSettleCount;
				XYscope.NoSettlingTimeReqd = GBL_EE_SetupData.PioBreakpointCount;
				XYscope.PioUnblankCount = GBL_EE_SetupData.PioUnblankCount;
				Gbl_Show_Digital_Time=GBL_EE_SetupData.Show_Digital_Time;
				Gbl_Show_DayOfWeek=GBL_EE_SetupData.Show_DayOfWeek;
				Gbl_Show_Date=GBL_EE_SetupData.Show_Date;
				Gbl_StartUp_In_Clock_Mode=GBL_EE_SetupData.StartUp_In_Clock_Mode;
				Serial.print ("Initialized from EEPROM Ver: ");Serial.println(GBL_EE_SetupData.DataStructRev);
			}
		}
	}
	bool is_ActiveTiming_SameAs_EEPROM(){
		//	Routine which compares current timing parameters to those stored
		//	in the EEPROM powerup values data-set.
		//
		//	Passed Parameters	NONE
		//
		//	Returns: 	TRUE	if EEPROM-Data-Set = Active-Data-Set
		//				FALSE	if EEPROM-Data-Set != Active-Data-Set
		//
		//	20180731 Ver 0.0 		 	E.Andrews	Checked for use with TEENSY release.	
		//	20181019 Ver 0.1 		 	E.Andrews	Update to include Clock-Display & Clock-startup check.
		//	
		bool ReturnValue = true;	//Init return value
		//
		//	Now, compare ACTIVE to EEPROM variable by variable
		//
		//	do NOT check GBL_EE_SetupData.DataStructRev
		if(GBL_EE_SetupData.PioSmallSettleCount != XYscope.PioSmallSettleCount) ReturnValue=false;
		if(GBL_EE_SetupData.PioLargeSettleCount != XYscope.PioLargeSettleCount) ReturnValue=false;
		if(GBL_EE_SetupData.PioBreakpointCount != XYscope.NoSettlingTimeReqd) ReturnValue=false;
		if(GBL_EE_SetupData.PioUnblankCount != XYscope.PioUnblankCount) ReturnValue=false;
		
		if(GBL_EE_SetupData.Show_Digital_Time != Gbl_Show_Digital_Time) ReturnValue=false;
		if(GBL_EE_SetupData.Show_DayOfWeek != Gbl_Show_DayOfWeek) ReturnValue=false;
		if(GBL_EE_SetupData.Show_Date != Gbl_Show_Date) ReturnValue=false;
		if(GBL_EE_SetupData.StartUp_In_Clock_Mode != Gbl_StartUp_In_Clock_Mode) ReturnValue=false;
		
		return ReturnValue;
	}
	
	
	void PrintEEpromDataToConsole(){
		//	Diagnostic and development test routine to dump TEENSY EEPROM
		//	contents out to the serial monitor port.
		//
		//	Passed Parameters	NONE
		//
		//	Returns: 	NOTHING
		//
		//	20180731 Ver 0.0 		 	E.Andrews	Checked for use with TEENSY release (DataStructRev=1.00).
		//	20181005 Ver 0.1 		 	E.Andrews	Updated to use DataStructRev=2.00 (expanded to include clock display parameters)		
		//
		
		Serial.print("     |         Data Structure Rev: ");Serial.println(GBL_EE_SetupData.DataStructRev);
		Serial.println("     |                         EEPROM / ACTIVE_VALUE");
		Serial.print("     |       PIO_Small_Settle_Cnt: ");Serial.print(GBL_EE_SetupData.PioSmallSettleCount);
			Serial.print(" / ");Serial.print(XYscope.PioSmallSettleCount);Serial.println("");
		Serial.print("     |       PIO_Large_Settle_Cnt: ");Serial.print(GBL_EE_SetupData.PioLargeSettleCount);
			Serial.print(" / ");Serial.print(XYscope.PioLargeSettleCount);Serial.println("");			
		Serial.print("     |        PIO_Break_Point_Cnt: ");Serial.print(GBL_EE_SetupData.PioBreakpointCount);
			Serial.print(" / ");Serial.print(XYscope.NoSettlingTimeReqd);Serial.println("");			
		Serial.print("     |PIO_Unblank_Pulse_Width_Cnt: ");Serial.print(GBL_EE_SetupData.PioUnblankCount);
			Serial.print(" / ");Serial.print(XYscope.PioUnblankCount);Serial.println("");
		Serial.print("     |    Show_Digital_Time(0=NO): ");Serial.print(GBL_EE_SetupData.Show_Digital_Time);
			Serial.print(" / ");Serial.print(Gbl_Show_Digital_Time);Serial.println("");									
		Serial.print("     |       Show_DayOfWeek(0=NO): ");Serial.print(GBL_EE_SetupData.Show_DayOfWeek);
			Serial.print(" / ");Serial.print(Gbl_Show_DayOfWeek);Serial.println("");												
		Serial.print("     |            Show_Date(0=NO): ");Serial.print(GBL_EE_SetupData.Show_Date);
			Serial.print(" / ");Serial.print(Gbl_Show_Date);Serial.println("");
		Serial.print("     |StartUp_In_Clock_Mode(0=NO): ");Serial.print(GBL_EE_SetupData.StartUp_In_Clock_Mode);
			Serial.print(" / ");Serial.print(Gbl_StartUp_In_Clock_Mode);Serial.println("");												
	
		//Now check EEPROM
		if(is_ActiveTiming_SameAs_EEPROM()){
			Serial.println("     +-- ALL OKAY: Currently Active data-set is the SAME AS values stored in EEPROM.");
		} else Serial.println("     +-- WARNING: Currently Active data-set is DIFFERNET than values stored in EEPROM!"); 
	}
	//============== END TEENSY TEST AND DEVELOPMENT EEPROM routines
#endif

void PlotToCRT_HwSettings(){
	//	Routine to display Timing Settings to CRT Screen
	//	Prints:	
	//		XYscope.PioSmallSettleCount		Small Step Settling Count Value
	//		XYscope.PioLargeSettleCount		Large Step Settling Count Value

	//		XYscope.NoSettlingTimeReqd		Large Step Threshold Count Value(0-4095)
	//		XYscope.PioUnblankCount			UNBLANK Pulse Width Count Value
	//
	//	Passed Parameters	NONE
	//
	//	Returns: NOTHING
	//
	//	20180719 Ver 0.0 		 	E.Andrews	1st pass effort
	//
	XYscope.printSetup(20,4095-170,150);	//void printSetup(short textX, short textY, short textSize)
	XYscope.print("Small-Step_Settle:");XYscope.print(XYscope.PioSmallSettleCount); XYscope.print(" Large-Step_Settle:");XYscope.print(XYscope.PioLargeSettleCount);
	XYscope.printSetup(20,4095-2*170,150);	//void printSetup(short textX, short textY, short textSize)
	XYscope.print("Large-Step-Threshold:");XYscope.print(XYscope.NoSettlingTimeReqd); XYscope.print(" UNBLANK:");XYscope.print(XYscope.PioUnblankCount);
}

void PlotToCRT_SetTimeDateScreen(int ST_hour, int ST_minute, int ST_second, int ST_month, int ST_day,int ST_year, int ST_State){
	//	Routine to send (&update) "Set Time and Date" screen; Includes ACCEPT and CANCEL display.
	//
	//	Passed Parameters	
	//		ST_hour		Integer, 0-23 Hours (values 0-11 are AM, values 12-23 are PM)
	//		ST_minute	Integer, 0-59 Minutes
	//		ST_second	Integer, 0-59 Seconds
	//		ST_month	Integer, 1-12 Month
	//		ST_day		Integer, 1-31 Day of Month	
	//		ST_year		Integer, 00-99 Year
	//		ST_State	Integer, 0=Hr, 1=Min, 2=Mnth, 3=Day, 4=Yr, 5=ACCEPT, 6=CANCEL, All others=No Underline
	//
	//	Returns: NOTHING
	//
	//	20180927 Ver 0.0 		 	E.Andrews	1st pass effort
	//	
	//	
	short ST_textX = 500;
	short ST_textY = 2000;
	const short ST_textSize=375;
	const short ST_textNormBright=100;
	const short ST_textSuperBright=250;
	//How about a nice boarder for this screen...
	XYscope.setGraphicsIntensity(80);
	XYscope.plotRectangle(0,0,4095,4095);
	//Print Title and Instructions on top of screen
	XYscope.setFontSpacing(10);
	XYscope.printSetup(50, 3700, 300, ST_textNormBright);
	XYscope.print("      SET CLOCK");
	//XYscope.setFontSpacing(prop);
	XYscope.printSetup(50, 3700-250, 200, ST_textNormBright);
	XYscope.print("Touch UP/DOWN to change value.");
	XYscope.printSetup(50, 3700-500, 200, ST_textNormBright);
	XYscope.print("Touch NEXT to move to next item.");
	XYscope.printSetup(50, 3700-750, 200, ST_textNormBright);
	XYscope.print("Touch ENTER to ACCEPT when done.");
	XYscope.setFontSpacing(10);
	
	bool ST_UL_Flag=false;
	
	
	XYscope.printSetup(ST_textX+650, ST_textY, ST_textSize, ST_textNormBright);
	
	//Print Hour
	if(ST_State==0) {
		ST_UL_Flag=true;
		XYscope.setTextIntensity(ST_textSuperBright);
		}
	else{
		ST_UL_Flag=false;	//Underline variable as required
		XYscope.setTextIntensity(ST_textNormBright);
	}
	if (ST_hour<10) XYscope.print("0",ST_UL_Flag);	//Print Leading Zero
	XYscope.print(ST_hour,ST_UL_Flag);
	XYscope.setTextIntensity(ST_textNormBright);
	XYscope.print(":");
	
	//Print Minute
	if(ST_State==1) {
		ST_UL_Flag=true;
		XYscope.setTextIntensity(ST_textSuperBright);
		}
	else{
		ST_UL_Flag=false;	//Underline variable as required
		XYscope.setTextIntensity(ST_textNormBright);
	}
	if (ST_minute<10) XYscope.print("0",ST_UL_Flag);	//Print Leading Zero
	XYscope.print(ST_minute,ST_UL_Flag);
	XYscope.print(":");
	
	//Print Second
	if(ST_State==2) {
		ST_UL_Flag=true;
		XYscope.setTextIntensity(ST_textSuperBright);
		}
	else{
		ST_UL_Flag=false;	//Underline variable as required
		XYscope.setTextIntensity(ST_textNormBright);
	}
	if (ST_second<10) XYscope.print("0",ST_UL_Flag);	//Print Leading Zero
	XYscope.print(ST_second,ST_UL_Flag);
	XYscope.print(" ");
	
	XYscope.printSetup(ST_textX+500, ST_textY-1.5*ST_textSize, ST_textSize, ST_textNormBright);
	
	//Print Month
	if(ST_State==3) {
		ST_UL_Flag=true;
		XYscope.setTextIntensity(ST_textSuperBright);
		}
	else{
		ST_UL_Flag=false;	//Underline variable as required
		XYscope.setTextIntensity(ST_textNormBright);
	}
	if (ST_month<10) XYscope.print("0",ST_UL_Flag);	//Print Leading Zero
	XYscope.print(ST_month,ST_UL_Flag);
	XYscope.setTextIntensity(ST_textNormBright);
	XYscope.print("/");
	
	//Print Date
	if(ST_State==4) {
		ST_UL_Flag=true;
		XYscope.setTextIntensity(ST_textSuperBright);
		}
	else{
		ST_UL_Flag=false;	//Underline variable as required
		XYscope.setTextIntensity(ST_textNormBright);
	}
	if (ST_day<10) XYscope.print("0",ST_UL_Flag);	//Print Leading Zero
	XYscope.print(ST_day,ST_UL_Flag);
	XYscope.setTextIntensity(ST_textNormBright);
	XYscope.print("/");
	
	//Print Year
	if(ST_State==5) {
		ST_UL_Flag=true;
		XYscope.setTextIntensity(ST_textSuperBright);
		}
	else{
		ST_UL_Flag=false;	//Underline variable as required
		XYscope.setTextIntensity(ST_textNormBright);
	}
	XYscope.print(ST_year,ST_UL_Flag);
	XYscope.setTextIntensity(ST_textNormBright);
	ST_UL_Flag=false;
	
	//Reposition for 3rd line of text
	XYscope.printSetup(ST_textX, ST_textY-3*ST_textSize, ST_textSize, ST_textNormBright);
	//Print 'ACCEPT'
	if(ST_State==6) {
		ST_UL_Flag=true;
		XYscope.setTextIntensity(ST_textSuperBright);
		}
	else{
		ST_UL_Flag=false;	//Underline variable as required
		XYscope.setTextIntensity(ST_textNormBright);
	}
	XYscope.print("ACCEPT",ST_UL_Flag);
	XYscope.setTextIntensity(ST_textNormBright);
	ST_UL_Flag=false;
	XYscope.print("  ");
	
	//Print 'CANCEL'Year
	if(ST_State==7) {
		ST_UL_Flag=true;
		XYscope.setTextIntensity(ST_textSuperBright);
		}
	else{
		ST_UL_Flag=false;	//Underline variable as required
		XYscope.setTextIntensity(ST_textNormBright);
	}
	XYscope.print("CANCEL",ST_UL_Flag);
	XYscope.setTextIntensity(ST_textNormBright);
	ST_UL_Flag=false;

	
}
void RunDhrystoneTest(){
	//	Routine to measure and display COMPUTE performance statistics.
	//	Output goes to Serial Monitor where a CSV fragment maybe copied
	//	from Serial MONITOR screen and readily saved into an Excel worksheet.
	//
	//	Passed Parameters	NONE
	//
	//	Returns: NOTHING
	//
	//	20180521 Ver 0.0 		 	E.Andrews	Checked for use with TEENSY release.	
	//
	
	//Run Dhrystone Tests
	Serial.println();
	#if defined(__MK66FX1M0__)	//Here's the #include used when running on a TEENSY 3.6
		Serial.print("Board=TEENSY_3.6 ");Serial.print("(CPU=");Serial.print(F_CPU/1000000);Serial.print(" Mhz, F_BUS="); Serial.print(F_BUS/1000000);Serial.println(" Mhz)");
	#endif
	#if defined(__SAM3X8E__)	//Here's the #include used when running on a TEENSY 3.6
		Serial.print("Board=DUE ");Serial.print("(CPU=");Serial.print("84");Serial.println(" Mhz)");
	#endif
	double Num_Of_Runs=2000000; 
	
	Serial.print("Execution starts, ");	Serial.print(Num_Of_Runs); Serial.print(" runs through Dhrystone...");
	double Dhrystones_Per_Sec = Run_DHRYSTONE_benchmark(Num_Of_Runs);	//Run the actual Drystones Test
	Serial.println("Execution COMPLETED!");

	Serial.print("Microseconds for one run through Dhrystone: ");
	double DhryCycleTime_us=(1000000./Dhrystones_Per_Sec);
	Serial.println(DhryCycleTime_us);
	Serial.print("Dhrystones per Second: ");
	Serial.println(Dhrystones_Per_Sec);
	Serial.print("VAX MIPS rating = ");
	double Vax_Mips = Dhrystones_Per_Sec / 1757.0;
	Serial.print(Vax_Mips);
	Serial.println(", CSV Dump:");
	//CSV Header
	Serial.println("CPU_Mhz,DS_Time_us,DS_Per_Sec,Vax_MIPS,Bus_Mhz,actual_PaintTimeMs,NumPoints,RefreshPeriodMs,AvgPointPeriodUs");
	//------------------------------------------------------------
	//	TEENSY CODE BLOCK 
	//------------------------------------------------------------	
	//
	#if defined(__MK66FX1M0__)
		//CSV Data
		Serial.print(F_CPU/1000000);Serial.print(",");
		Serial.print(DhryCycleTime_us);Serial.print(",");
		Serial.print(Dhrystones_Per_Sec);Serial.print(",");
		Serial.print(Vax_Mips);Serial.print(",");Serial.print(F_BUS/1000000);
		
		Serial.print(",");Serial.print (XYscope.act_PaintTimeDurationUs/1000.);
		Serial.print(",");Serial.print (XYscope.XYlistEnd);
		//Serial.print(",");Serial.print (XYscope.act_RefreshIntervalMs);
		Serial.print(",");Serial.print (XYscope.act_RefreshIntervalUs/1000.);
		float Avg_Point_PeriodUs=float(XYscope.act_PaintTimeDurationUs)/float(XYscope.XYlistEnd);
		Serial.print(",");Serial.print (Avg_Point_PeriodUs);
		Serial.println();
	#endif
	//------------------------------------------------------------
	//	DUE CODE BLOCK 
	//------------------------------------------------------------
	//
	#if defined(__SAM3X8E__) 	
			//CSV Data
		Serial.print(F_CPU/1000000);Serial.print(",");
		Serial.print(DhryCycleTime_us);Serial.print(",");
		Serial.print(Dhrystones_Per_Sec);Serial.print(",");
		Serial.print(Vax_Mips);Serial.print(",");Serial.print("0");
		
		Serial.print(",");Serial.print (XYscope.act_PaintTimeDurationUs/1000.);
		Serial.print(",");Serial.print (XYscope.XYlistEnd);
		//Serial.print(",");Serial.print (XYscope.act_RefreshIntervalMs);
		Serial.print(",");Serial.print (XYscope.act_RefreshIntervalUs/1000.);
		float Avg_Point_PeriodUs=float(XYscope.act_PaintTimeDurationUs)/float(XYscope.XYlistEnd);
		Serial.print(",");Serial.print (Avg_Point_PeriodUs);
		Serial.println();
	#endif
}
void PrintStatsToConsole(void) {
	//	Routine to send memory usage status reports out to the 
	//	serial monitor port.  Use this to observe how various graphics
	//	objects consume the graphics list RAM memory.  Routine also
	//	also displays current clock speeds in use so as you change
	//	them and observe performance differences, you can zero in on
	//	what works best for your scope.
	//
	//	Passed Parameters	NONE
	//
	//	Returns: NOTHING
	//
	//	20170811 Ver 0.0	E.Andrews	First cut
	//	20180510 Ver 1.0	E.Andrews	Added PIO stats (TEENSY) and also inserted 50ms delay before printing
	//
	delay(100);	//Insert delay so stats are valid after last change...
	Serial.print(F("\n STATS..............\n  MaxBuffSize: "));
	Serial.print(XYscope.MaxBuffSize);
	Serial.print(F("  Total Array Used: "));
	Serial.print((XYscope.XYlistEnd - 1));
	Serial.print(F(" ("));
	Serial.print((XYscope.XYlistEnd * 100) / XYscope.MaxBuffSize);
	Serial.print(F(" %)  "));
	#if defined(__MK66FX1M0__)	//Here's the #includes needed when running a DUE processor 
		#if defined(CFG_TNSY_DacRefVolts_LOW)
			Serial.println (F("TNSY Dac_Vref=1.5V"));
		#else
			Serial.println (F("TNSY Dac_Vref=3.3V"));
		#endif
	#endif	
	if (showDueOptions){
		Serial.print(F("     DMA Clock Freq: "));
		Serial.print(XYscope.DmaClkFreq_Hz / 1000);
		Serial.print(F(" Khz"));
		Serial.print(F(" (Period: "));
		Serial.print(XYscope.DmaClkPeriod_us);
		Serial.println(F(" us)"));
		Serial.print(F("   Point Clock Freq: "));
		Serial.print(XYscope.DmaClkFreq_Hz / 2000);
		Serial.print(F(" Khz"));
		Serial.print(F(" (Period: "));
		Serial.print(XYscope.DmaClkPeriod_us * 2);
		Serial.println(F(" us)"));
		Serial.print(F("Actual Refresh Period: "));
		Serial.print(float(XYscope.ActiveRefreshPeriod_us/1000.));
		Serial.print(F(" ms)\n"));
	}
	if (!showDueOptions){

		Serial.print(F("  TEENSY Clock:")); Serial.print(F_CPU/1000000); Serial.print(F(" Mhz"));
		Serial.print(F(", PIO Paint Time: "));Serial.print (XYscope.act_PaintTimeDurationUs/1000.);Serial.print(F(" ms"));
		Serial.print(F(", Refresh Interval: "));Serial.print (XYscope.act_RefreshIntervalUs/1000.);Serial.print(F(" ms"));
		Serial.print(F(", Refresh Rate: ")); Serial.print(float(1000000) / float(XYscope.ActiveRefreshPeriod_us),1); Serial.print(F(" Hz"));
		Serial.println();
		Serial.print(F("  Small-Step Settling: "));
		Serial.print(XYscope.PioSmallSettleCount);
		Serial.print(F("  Large-Step Settling: "));
		Serial.print(XYscope.PioLargeSettleCount);
		Serial.print(F("  Pio Unblank:"));
		Serial.print(XYscope.PioUnblankCount);
		Serial.print(F("  Big-Step Threshold:"));
		Serial.print(XYscope.NoSettlingTimeReqd);
		Serial.println();
		float RefreshLoad = (float(XYscope.act_PaintTimeDurationUs)/float(XYscope.act_RefreshIntervalUs))* 100.;
		Serial.print(F("  Refresh Load: "));Serial.print (float(RefreshLoad),1);Serial.print(F("%"));
		Serial.print (F(", Compute Availability: ")); Serial.print(100.-RefreshLoad,1);Serial.print(F("%"));
		float Avg_Point_PeriodUs=float(XYscope.act_PaintTimeDurationUs)/float(XYscope.XYlistEnd);
		Serial.print(F(", Avg Point Period: "));Serial.print (Avg_Point_PeriodUs);Serial.print(F(" us"));
		Serial.println();
		
	}


	Serial.print(F("  Graphics Int: "));
	Serial.print(XYscope.getGraphicsIntensity(), DEC);
	Serial.print(F(" % ("));	Serial.print(XYscope._graphDensity, DEC); Serial.print(F(")  Text Int: "));
	//Serial.print(F(" %   Text Int: "));
	Serial.print(XYscope.getTextIntensity(), DEC);
	Serial.print(F(" % ("));	Serial.print(XYscope._textDensity, DEC); Serial.print(F(")"));
	Serial.println();
	Serial.print(F("  Font Spcng Mode = "));
	if (XYscope.getFontSpacing() == 0)
		Serial.print(F("PROP"));
	else
		Serial.print(F("MONO"));

	Serial.print(F(", Scrn_Sav_Secs: "));
	Serial.print(XYscope.getScreenSaveSecs());
	ClearOperInputArray();
}
//============================================================================================
//BEGIN Define Bouncing Ball Global variables.................................................
//============================================================================================
float bb_StartHt=5;			//Starting height
float bb_G=9.2;				//Gravity (m/s^2)
float bb_Restitution=.75;	//How bouncy the ball is...
float bb_T=0;				//Current Time (Sec)
float bb_V=0;				//Current Velocity (m/sec)
float bb_H=0;				//Current Height (m)
float bb_Hnew=0;			//intermediate height (m)
float bb_dT=.005;			//Time Step Size (sec)
float bb_ContactTime=0.1;	//Floor Contact Time (sec)
float bb_Hmax=0;			//Maximum Height (m)
float bb_Vmax=0;			//Maximum Velocity Possible (m/s)
float bb_Hstop=0.01;		//Bounce Height when we say we're done with simulation (m)
bool bb_freefall=true;			//Are we in free fall? (true/false)
float bb_Tlast;				//Time we would have launched to get h0 @ t0 (sec)
//============================================================================================
//END Define Bouncing Ball Global Variables...................................................
//============================================================================================
void P25_BouncingBallPlot(float Gravity,float Restitution,float bb_Speed){
	//	Routine to simulate a bouncing ball 
	//	Runs once and plots results to CRT (or Serial Port)
	//	KEY Parameters
	//
	//	Passed Parameters	
	//	   Gravity		Float, Acceleration due to gravity (m/sec^2)
	//					Valid range: 3-25
	//						 Moon 7.66					
	//						Earth 9.2
	//						 Mars 3.71
	//
	//	   Restitution:	Float, "Bounce back" upon hitting floor (%). Range: 0-1.0
	//
	//	   bb_Speed:	Float, "Speed of plotting" (X), range: .1-1000X (1.0= ~Real time, Higher=faster]
	//
	//	Returns: NOTHING
	//
	//	201905 Ver 0.0	E.Andrews	First cut
	//	
	
	if (Gravity<1 || Gravity > 25) Gravity=9.2;	//Error check and bounding; If out of range, set to "EARTH GRAVITY"
	bb_Restitution = constrain(Restitution,0,1.0);	//range limit input (disallow values>1)
	bb_Speed=constrain(bb_Speed,.1,10);	//range limit speed (Note: >5 sets speed to "Fast As Possible!")
	int WaitMs=(bb_dT*(1000.)/(bb_Speed*2) );
	int WaitMicros=(bb_dT*(1000000.)/(bb_Speed*3) );
	if (bb_Speed>5) {
		WaitMs=-1;		//NO DELAY
		WaitMicros=-1;	//No Delay
	}
	Serial.print(" -- Delta T (sec) = ");Serial.print (bb_dT,4);
	Serial.print(" , bb_Speed Factor = ");Serial.print (bb_Speed);
	Serial.print(", Calculated Loop Wait Time (us) = ");Serial.print (WaitMicros);
	Serial.print(", Calculated Loop Wait Time (ms) = ");Serial.println (WaitMs);
	
	//Initialize bb)Variables for this run
	bb_StartHt=10;	//Meters
	bb_V = 0;		//Initial Velocity
	bb_G = Gravity;	//Acceleration due to Gravity (m/s^2). Earth = 9.2 , Moon = 9.2 * (5/6) = 7.66
	bb_T=0;
	bb_dT=.005;
	//bb_Restitution=.75;	//Ball bounce characteristic.  Typical values are: [commentded out as this is now a passed parameter]
							//Super Ball = .90, Basket Ball = .85, Golf Ball = .80, Tennis Ball = .75
							
	bb_ContactTime=0.001;	//Define Contact time with the floor (sec) [how long ball stays at H=0 after contact]
	bb_H = bb_StartHt;		//Initialize Height to "StartHt"
	bb_Hstop=.0005;			//Define Height limit when bouncing has ended and plotting can stop
	bb_freefall=true;		//True means ball is falling down to floor, False means it has hit the flow and is rebounding
	bb_Tlast= (sqrt(2*bb_StartHt/bb_G));	//Time till last point in current arc occurs
	bb_Vmax=sqrt(2*bb_StartHt/bb_G);		//Max Velocity
	bb_Hmax=bb_StartHt;						//Max Height
	bb_T=bb_Tlast+2*sqrt(2*bb_Hmax/bb_G);	//initialize T	
	bb_T=0;
	int bb_X=0;
	int bb_Y=0;	
	int X_offset=450,Y_offset=200;			//Define XY location of the origin of graph
	
	bool diag_mode = false;//true;	//Diagnostic switch...set to true if you want to Printout results to serial port; false = no serial dump 

	//Defensive programming tactics:
	const float LoopMax=5000;		//Backup limit to avoid getting stuck in an infinite loop
	float LoopCount=0;				//Backup counter to prevent infinite loop faults...
	
	//If diag_mode=true, print out column headers ONCE.	
	if (diag_mode){
		Serial.println();
		Serial.println("Loop#,Count,T,H,V,Vmax,Hmax");
	}
	double NextPointTime=0;	//Init variable used for real-time plotting effects
	//Initialize graph title, axis, & plotting grid
	const bool GridOn=true;
	XYscope.plotStart();	//Start with a fresh screen	
	int OldInt=XYscope.getGraphicsIntensity();
	if (GridOn){
		int C_Height=Y_offset-20;
		XYscope.plotRectangle(X_offset,Y_offset,4095,4095);
		//Y-axis Label
		XYscope.printSetup(0,3900,C_Height);
		XYscope.print("(m)");
		//X-axis Label
		XYscope.printSetup(50,12,C_Height);
		XYscope.print("Secs->");
		//Graph Label
		XYscope.printSetup(X_offset+200,4050-C_Height*1.5,C_Height,150);
		XYscope.print(" 5m Drop,G=");XYscope.print(Gravity,2);
		XYscope.print(" R=");XYscope.print(bb_Restitution,2);
		XYscope.print(" S=");XYscope.print(bb_Speed,1);XYscope.print("X");

		int Xwide=4096-X_offset;
		int Yheight=4096-Y_offset;
		
		float LineCount=2.5;
		//Go to starting location for this list
		int ListStartX=2100;
		int LineOffset=C_Height*LineCount;XYscope.printSetup(ListStartX,4050-LineOffset,C_Height); 
		XYscope.print("   G(MARS)= 3.71");
		
		//Go to Next Line
		LineCount++;LineOffset=C_Height*LineCount;
		XYscope.printSetup(ListStartX,4050-LineOffset,C_Height); 
		XYscope.print("   G(MOON)= 7.66");
		
		//Go to Next Line
		LineCount++;LineOffset=C_Height*LineCount;
		XYscope.printSetup(ListStartX,4050-LineOffset,C_Height); 
		XYscope.print("  G(EARTH)= 9.20");
		
		//Go to Next Line
		LineCount++;LineOffset=C_Height*LineCount;
		XYscope.printSetup(ListStartX,4050-LineOffset,C_Height); 
		XYscope.print("G(NEPTUNE)=11.50");
		
		//Go to Next Line
		LineCount++;LineOffset=C_Height*LineCount;XYscope.printSetup(ListStartX,4050-LineOffset,C_Height);
		XYscope.printSetup(ListStartX,4050-LineOffset,C_Height);
		XYscope.print("G(JUPITER)=24.80");
		

		//Go to Next Line
		LineCount=LineCount+2;LineOffset=C_Height*LineCount;
		XYscope.printSetup(ListStartX,4050-LineOffset,C_Height);
		XYscope.print("R(SuperBall)=.90");
		
		//Go to Next Line
		LineCount++;LineOffset=C_Height*LineCount;
		XYscope.printSetup(ListStartX,4050-LineOffset,C_Height);
		XYscope.print("R(Bskt Ball)=.85");

		//Go to Next Line
		LineCount++;LineOffset=C_Height*LineCount;
		XYscope.printSetup(ListStartX,4050-LineOffset,C_Height);
		XYscope.print("R(Golf Ball)=.80");	
		//Go to Next Line
		LineCount++;LineOffset=C_Height*LineCount;
		XYscope.printSetup(ListStartX,4050-LineOffset,C_Height);
		XYscope.print("R(Ten. Ball)=.75");
		//Go to Next Line
		LineCount++;LineOffset=C_Height*LineCount;
		XYscope.printSetup(ListStartX,4050-LineOffset,C_Height);
		XYscope.print("R(No Bounce)=.0");
	
		/* 
		//Plot a cursor to show the current Gravity Setting
		XYscope.setGraphicsIntensity(150);
		XYscope.plotLine(X_Curs,Y_Curs,X_Curs-80,Y_Curs-50);
		XYscope.plotLine(X_Curs,Y_Curs,X_Curs-80,Y_Curs+50);
		XYscope.plotLine(X_Curs-80,Y_Curs-50,X_Curs-80,Y_Curs+50);
		 */
		XYscope.setGraphicsIntensity(10);
		for (int i=1;i<11;i++){
			//Horizontal grid
			XYscope.plotLine(X_offset,Y_offset+i*(Yheight/10),4095,Y_offset+i*(Yheight/10));
			//Vertical grid
			XYscope.plotLine(X_offset+i*(Xwide/10),Y_offset,X_offset+i*(Xwide/10),4095);
			//Label chart

			if(i<10){	//print seconds...
				
				//Print Time Scale Numbers (Secs)
				XYscope.printSetup(X_offset+i*(Xwide/10) - (C_Height/2),0,C_Height);
				XYscope.print(i);
				//Print Height Scale Numbers (meters)
				XYscope.printSetup(0,Y_offset+i*(Yheight/10)- (C_Height/2),C_Height);
				XYscope.print(float(i)/2.,1);
			}
		}
		XYscope.setGraphicsIntensity(OldInt);
	}
	
	//================= begin main calculation loop
	//while ((bb_Hmax>bb_Hstop)&&(LoopCount<LoopMax)) {
	
	while ((bb_T<11)&&(bb_Hmax>bb_Hstop)&&(LoopCount<LoopMax)) {	//Stop when we've plotted a full 10 seconds of data OR hit the end of the graph	
		LoopCount++;
		
		if(bb_freefall){	//Are we in Freefall (still going towards the floor) or in Rebound (off of the floor) state?	
			//FREEFALL!
			bb_Hnew=bb_H + bb_V*bb_dT - 0.5*bb_G*sq(bb_dT);
			
			//is bb_H near or below the floor?  If yes, handle it as a bounce
			
			if (bb_Hnew<0){	//Test the next H location...Have we hit the floor?
				//Yes, we have hit the floor...Handle it as a bounce!
				
				//bb_T=bb_Tlast+2*sqrt(2*bb_Hmax/bb_G);	//Update T
				bb_T=bb_T+bb_dT;
				
				//bb_Tlast=bb_T+bb_ContactTime;			//Update Tlast
				bb_H=0;		//H=floor level				//H is below the floor height; Force H back to zero as we can't go negative
				bb_freefall=false;						//Switch flag to Rebound state.
			}
			else{
				//No, we haven't yet hit the floor; we are still in free fall
				bb_T=bb_T+bb_dT;			//Update T
				bb_V=bb_V - bb_G * bb_dT;	//Update V
				bb_H=bb_Hnew;				//Update H
				
			}
			
		}
		else {	//Not in free fall,we are in a Rebound!
			//REBOUND
			bb_Vmax=-bb_Vmax * bb_Restitution;	//Calc V_max using Restitution value
			bb_T=bb_T+ bb_ContactTime;			//Step ahead T by ContactTime constant
			bb_V=-bb_V*bb_Restitution;						//We're at max velocity
			bb_H=0;								//We're at the floor!
			bb_freefall=true;					//Now that we've reset all the variables, switch back to Freefall state
		}
		
		bb_Hmax=0.5*sq(bb_Vmax)/bb_G;
		
		//Calculate X-scale factor
		float X_ScaleFactor = 3500/10;	//Plot a max of 10 secs?
		//Calc X-coordinate of point to plot
		bb_X=bb_T * X_ScaleFactor+X_offset;
		
		//Calculate Y-scale factor		
		float Y_ScaleFactor = (4096-Y_offset)/bb_StartHt;
		//Calc Y-coordinate of point to plot
		bb_Y=bb_H * Y_ScaleFactor+Y_offset;
		//Plot to screen!
		XYscope.plotPoint(bb_X,bb_Y);
		XYscope.plotPoint(bb_X,bb_Y);	//plot twice to get more brightness!	

		XYscope.autoSetRefreshTime();	//Only required for DUE; Ignored by TEENSY
		
		// We are ready to plot a point...Based on the Ms timer, should we wait a little while?
		// This loop slows things down to approximately Real Time
		//	i.e.: Points actually appear on the screen as they would if you were watching a bouncing ball
		//	Note, use bb_Speed variable if you want to go as fast as possible
		/* 
		while(NextPointTime>millis() ){
			//Serial.print("PlotWait...");
		};	//Wait loop to plot next point...
		 */
		while(NextPointTime>micros() ){
			//Serial.print("PlotWait...");
		};	//Wait loop to plot next point...
		
		//Now calc wait time for next point...
		//	Note:	Set bb_Speed = 1 for REAL TIME
		//			Set bb_Speed < 1 to slow things down
		//			Set bb_Speed > 1 to speed things up
		//			Set bb_Speed >5 to go as fast as possible!

		//NextPointTime=millis()+WaitMs;
		NextPointTime=micros()+WaitMicros;
		
		//Development print out:
		//Serial.print ("Waitms = ");Serial.println (WaitMs);
		
		//In diag mode, send data out to serial port
		if (diag_mode){	
			//Set diag_mode = true and you can output H (bb_H) & V (bb_V) (or other variables) in a CSV format for plotting in Excel.
			//	This was used to debug program and get formulas working
			//Serial.print(Diag_LoopNum);Serial.print(",");
			Serial.print(LoopCount);Serial.print(",");
			Serial.print(bb_T);Serial.print(",");Serial.print(bb_H);
			//Serial.print(",");Serial.print(bb_V);Serial.print(",");Serial.print(bb_Vmax);
			//Serial.print(","); Serial.print(bb_Hmax);
			Serial.println();
		}

		XYscope.plotEnd();						//Only really req'd for DUE
		XYscope.autoSetRefreshTime();	//Only required for DUE; Ignored by TEENSY
	}//================= end main calculation loop
	
}	//====== END P25_BouncingBallPlot ============


void Print_CRT_Scope_Main_Menu() {
	//	Routine to send a menu of all available options to 
	//	the serial monitor port.  This routine is continuously
	//	updated as options are added or changed in XYscope.
	//
	//	Passed Parameters	NONE
	//
	//	Returns: NOTHING
	//
	//	20170725 Ver 0.0	E.Andrews	First cut
	//						(changes made continuously without version num updates)
	//	20180521 Ver 2.0	E.Andrews	TEENSY options, New Options, total rework to make it easier to use.
	//
	Serial.println();Serial.println();
	Serial.print(F("========= "));
	Serial.print(startMsg);
	Serial.println(F(" ========="));
		Serial.println(F(" H/h/? = Show HELP Screen & plotting STATS"));


	if (showDueOptions){
		Serial.println(F("---- DUE Hardware Settings & Control -----"));
		Serial.println(F(" nnn B = Set DMA Back-Porch BLANK count, 0-200"));
		Serial.println(F("nnnn C = Set DMA Freq., nnnn=kHz, 250-1800"));
		Serial.println(F(" nnn F = Set DMA Front-Porch BLANK count, 2-200"));

	}
	if (!showDueOptions){
		Serial.println(F("---- TEENSY Hardware Settings & Control -----"));
		Serial.println(F("  nn S = Set LARGE-step DAC SETTLING (nn=count,0-100)"));
		Serial.println(F("  nn s = Set SMALL-step DAC SETTLING (nn=count,0-100)"));
		Serial.println(F("nnnn L = Set LARGE-step Threshold LIMIT (nnnn=count,0-4095)"));
		Serial.println(F("  nn U = Set UNBLANK Width (nnn=count,0-50)"));
		Serial.println(F("     I = Show current HW settings on Scope Screen"));
		Serial.println(F("   n E = EEPROM(n=0 EEPROM->Screen,n=1 EEPROM->Active Dataset, n=10 SAVE Active_Dataset->EEPROM)"));
	}

	Serial.println(F("---- SCREEN SAVE Test routines"));
	Serial.println(F("     W = Wakeup from SCREEN SAVE"));
	Serial.println(F(" nnn W = Change Screen Save Timeout (nnn=Seconds)"));
	
	Serial.println(F("---- TEXT Test routines"));
	Serial.println(F("     m = Toggle Text Spacing Mode, Mono<-->Prop"));
	#if (CFG_IncludeHersheyFontROM==true)	//Only show this option when both FONTS are enabled
		#if (CFG_IncludeVectorFontROM==true)
			Serial.println(F("     M = Toggle FONT Select, Vector<-->Hershey"));
		#endif
	#endif
	Serial.println(F(" nnn t = Set TEXT Intensity to 1-250%"));

	Serial.println(F("---- GRAPHICS Test Routines ----"));	
	Serial.println(F(" nnn G = Set GRAPHICS Intensity 1-250%"));
	Serial.println(F("     K = CLEAR Display"));
	Serial.println(F("xxxx,yyyy Z = ADD a point at X,Y to Display List"));

	Serial.println("---- TEST Patterns & Demos ----");
	Serial.println("     P = Display 'Show Test Patterns' sub Menu");
	Serial.println("  nn P = Display Test Pattern Number 'nn'");
	
	Serial.println("---- CPU Performance Benchmarks ----");
	Serial.println("     d = Run DHRYSTONE Test (can take >60 Sec)");
	
	Serial.println("---- SET TIME & DATE ----");
	Serial.println(" h,m,s T = Set time values (hours,min,sec)");
	Serial.println(" m,d,y D = Set date values (month,date,year)");
	Serial.println(" t,w,d,s Q = Strt CLOCK(t=ShowTime,w=ShowWkday,d=ShowDate,s=AutoStrtCLK)");
	Serial.println("==========================================");

}
void Print_CRT_Scope_TestPattern_Menu() {
	//	Routine to send a menu of all available options to 
	//	the serial monitor port.  This routine is continuously
	//	updated as options are added or changed in XYscope.
	//
	//	Passed Parameters	NONE
	//
	//	Returns: NOTHING
	//
	//	20180515 Ver 0.0	E.Andrews	First cut.
	//						(changes made continuously without version num updates)
	//
	clearMonitorTerminalScreen();
	//Serial.println();
	Serial.print("========= ");
	Serial.print("TEST PATTERNS Sub-Menu");
	Serial.println(" =========");
	Serial.println("      P = Show Test Pattern Sub Menu");
	Serial.println("    0 P = XY Sine/Cosine OpAmp Gain Setup Pattern");
	Serial.println("    1 P = Centering Test Pattern");
	Serial.println("    2 P = Just Corner Dots Test Pattern");
	Serial.println("    3 P = Vert Stair-Case Test Pattern");
	Serial.println("    4 P = Vert Peak-To-Peak Test Pattern");
	Serial.println("    5 P = Horiz Peak-To-Peak Test Pattern");
	Serial.println();
	Serial.println("   11 P = Show Rand Nums, various formats");
	Serial.println("   12 P = Show Rand Nums w/Underline");
	Serial.println("   13,n P = Show Text Set; n=Switch to Font_0 or Font_1");
	Serial.println("   14,s,a P = Show Character; s=Size,a=Ascii Code");
	Serial.println("              (default s=3000, omit a for ALL chars)");
	Serial.println("   15,n,m P = Show Random Points; n=Num_of_Points,omit=10K,m=loop count");
	Serial.println("   16,n,m P = Show Random Vectors; n=Num_of_Points,omit=10K,m=loop count");
	Serial.println("   17,n,m P = Show Random Rectangles; n=Num_of_Points,omit=10K,m=loop count");
	Serial.println("   18,n,m P = Show Random Circles; n=Num_of_Points,omit=10K,m=loop count");
	Serial.println("   19,n,m P = Show Random Ellipses; n=Num_of_Points,omit=10K,m=loop count");
	Serial.println();	
	Serial.println("   20 P = Demo: Animated Logo Plot");
	Serial.println("   21 P = Demo: AGI Coordinate System");
	Serial.println("   22 P = Demo: Graphics Plot");
	Serial.println("   23,P = BEGIN: Clock Mode (Note: type a key to leave Clock-Mode)");
	Serial.println("   24 P = Demo: PONG ");
	Serial.println("   25,g,r,s P = Demo: 5m Ball Drop(g=gravity m/s/s,r=restitution %,s=speed %");
	Serial.println("   26 P = Demo:Happy Holidays from Nuts & Volts!");
	Serial.println("   27,s,a P = Demo:Fractal Tree, s=size(500-2000),a=branch angle (.2-.7)");
	
	Serial.println("==========================================");

}
void ResetPushButtons(){
	//	Routine to reset/initialize global push button variables.
	//
	//	Passed Parameters	NONE
	//
	//	Returns Via Global Variables:  RESET values for all buttons
	//
	//	20181005 Ver 0.0	E.Andrews	First cut - (Make button update a subroutine that can be called from anywhere)
	//	20181005 Ver 0.1	E.Andrews	Fixed reset logic state
	//
	const bool ResetState=LOW;
	
	PB_IncrBtn_DownEdge=ResetState;
	PB_IncrBtn_UpEdge=ResetState;
	PB_Last_IncrBtn_value=PB_IncrBtn_value=LOW;
	
	PB_DecrBtn_DownEdge=ResetState;	
	PB_DecrBtn_UpEdge=ResetState;
	PB_Last_DecrBtn_value=PB_DecrBtn_value=LOW;
	
	PB_NxtBtn_DownEdge=ResetState;
	PB_NxtBtn_UpEdge=ResetState;
	PB_Last_NxtBtn_value=PB_NxtBtn_value=LOW;
	
	PB_EnterBtn_DownEdge=ResetState;
	PB_EnterBtn_UpEdge=ResetState;
	PB_Last_EnterBtn_value=PB_EnterBtn_value=LOW;
}

void ReadPushButtons() {
	//	Routine to read 4 push buttons and update state variables for each
	//
	//	Passed Parameters	NONE
	//
	//	Returns Via Global Variables: 
	//		PB_IncrBtn_UpEdge		"Increment Button" variables
	//		PB_IncrBtn_DownEdge
	//		PB_IncrBtn_value
	//		
	//		PB_DecrBtn_UpEdge		"Decrement Button" variables
	//		PB_DecrBtn_DownEdge
	//		PB_DecrBtn_value
	
	//		PB_NxtBtn_UpEdge		"Increment Button" variables
	//		PB_NxtBtn_DownEdge
	//		PB_NxtBtn_value
	//
	//		PB_EnterBtn_UpEdge		"Enter Button" variables
	//		PB_EnterBtn_DownEdge
	//		PB_EnterBtn_value
	//
	//	20180928 Ver 0.0	E.Andrews	First cut - Make button update a subroutine that can be called from anywhere)
	//	20180930 Ver 0.1	E.Andrews	Expanded from 2 to 4 buttons. Moved button REPEAT code to beginning of routine
	//									to improve initial-button-push responsiveness.
	//	20181018 Ver 0.2	E.Andrews	Updated auto-repeat code to avoid "millis() rollover issues"
	//
	
	// BEGIN Push Button  repeat logic vvvvvvvvvvvvvv
	//	Note: Only PB1 (Incr) and PB2 (Decr) are repeating keys
	if (PB_IncrBtn_DownEdge==HIGH) {					//PB1: If we are just seeing the down edge, reset repeat counter and reset DOWN_Edge variable
		PB_RepeatMs=PB_RepeatRatePeriodMs;				//Use RepeatRatePeriodMs for next auto-repeat period
		PB_LastPushedMs = millis();						//Update LastPushed TimeStamp
		PB_IncrBtn_DownEdge=LOW;
	}	
	if (PB_IncrBtn_value==LOW && millis()-PB_LastPushedMs>PB_RepeatMs ) {
		PB_IncrBtn_DownEdge=HIGH;						//Repeat the button press
		PB_RepeatMs=PB_RepeatRatePeriodMs;				//Use RepeatRatePeriodMs for next auto-repeat period
		PB_LastPushedMs = millis();						//Update LastPushed TimeStamp
	}
	if (PB_DecrBtn_DownEdge==HIGH){		//PB2: If we are just seeing the down edge, reset repeat counter and reset DOWN_Edge variable
		PB_RepeatMs=PB_RepeatRatePeriodMs;				//Use RepeatRatePeriodMs for next auto-repeat period
		PB_LastPushedMs = millis();						//Update LastPushed TimeStamp
		PB_DecrBtn_DownEdge=LOW;
	}		
	if (PB_DecrBtn_value==LOW && millis()-PB_LastPushedMs>PB_RepeatMs ) {
		PB_DecrBtn_DownEdge=HIGH;						//Repeat the button press
		PB_RepeatMs=PB_RepeatRatePeriodMs;				//Use RepeatRatePeriodMs for next auto-repeat period
		PB_LastPushedMs = millis();						//Update LastPushed TimeStamp
	}
	// END Push Button repeat logic ^^^^^^^^^^^^^^^^ 
	
	// Update the Bounce instances :
	#if defined (IncrPB_Pin)	//Update Increment Button state...
		debounce_IncrBtn.update();
		PB_IncrBtn_value = debounce_IncrBtn.read();	
		
		if (PB_IncrBtn_value==HIGH && PB_Last_IncrBtn_value==HIGH) {
			//No button push is in process...
			//PB_IncrBtn_DownEdge=LOW;	//Comment out to let application code reset these values
			//PB_IncrBtn_UpEdge=LOW;
		}		
		if (PB_IncrBtn_value==LOW && PB_Last_IncrBtn_value==HIGH) {
			//Detect a DOWN Edge...(Button just being depressed)
			PB_IncrBtn_DownEdge=HIGH;
			PB_IncrBtn_UpEdge=LOW;
			PB_LastPushedMs = millis();						//Update LastPushed TimeStamp
			PB_RepeatMs=PB_RepeatInitialDelayMs;			//Use InitialRepeatDelayMs for next auto-repeat period
			//Serial.println ("==== PB1 DOWN Edge");
		}
		if (PB_IncrBtn_value==HIGH && PB_Last_IncrBtn_value==LOW) {
			//Detect an UP Edge...(Button being released)
			PB_IncrBtn_DownEdge=LOW;
			PB_IncrBtn_UpEdge=HIGH;
			//Serial.println ("==== PB1 UP Edge");
		}		
		PB_Last_IncrBtn_value=PB_IncrBtn_value;
	#endif
	
	#if defined (DecrPB_Pin)	
		debounce_DecrBtn.update();		//Update Decrement Button state...
		PB_DecrBtn_value = debounce_DecrBtn.read();	
		
		if (PB_DecrBtn_value==HIGH && PB_Last_DecrBtn_value==HIGH) {
			//No button push is in process...
			//PB_DecrBtn_DownEdge=LOW;	//Comment out to let application code reset these values
			//PB_DecrBtn_UpEdge=LOW;
		}		
		if (PB_DecrBtn_value==LOW && PB_Last_DecrBtn_value==HIGH) {
			//Detect a DOWN Edge...(Button just being depressed)
			PB_DecrBtn_DownEdge=HIGH;
			PB_DecrBtn_UpEdge=LOW;
			PB_LastPushedMs = millis();						//Update LastPushed TimeStamp
			PB_RepeatMs=PB_RepeatInitialDelayMs;			//Use InitialRepeatDelayMs for next auto-repeat period
			//Serial.println ("==== PB2 DOWN Edge");
		}
		if (PB_DecrBtn_value==HIGH && PB_Last_DecrBtn_value==LOW) {
			//Detect an UP Edge...(Button being released)
			PB_DecrBtn_DownEdge=LOW;
			PB_DecrBtn_UpEdge=HIGH;
			//Serial.println ("==== PB2 UP Edge");
		}		
		PB_Last_DecrBtn_value=PB_DecrBtn_value;

	#endif
	
	#if defined (NxtPB_Pin)	
		debounce_NxtBtn.update();
		PB_NxtBtn_value = debounce_NxtBtn.read();	
		
		if (PB_NxtBtn_value==HIGH && PB_Last_NxtBtn_value==HIGH) {
			//No button push is in process...
			//PB_NxtBtn_DownEdge=LOW;	//Comment out to let application code reset these values
			//PB_NxtBtn_UpEdge=LOW;
		}		
		if (PB_NxtBtn_value==LOW && PB_Last_NxtBtn_value==HIGH) {
			//Detect a DOWN Edge...(Button being depressed)
			PB_NxtBtn_DownEdge=HIGH;
			PB_NxtBtn_UpEdge=LOW;
			PB_RepeatMs=millis()+PB_RepeatInitialDelayMs;
			//Serial.println ("==== PB2 DOWN Edge");
		}
		if (PB_NxtBtn_value==HIGH && PB_Last_NxtBtn_value==LOW) {
			//Detect an UP Edge...(Button being released)
			PB_NxtBtn_DownEdge=LOW;
			PB_NxtBtn_UpEdge=HIGH;
			//Serial.println ("==== PB2 UP Edge");
		}		
		PB_Last_NxtBtn_value=PB_NxtBtn_value;

	#endif
	
	#if defined (EnterPB_Pin)	
		debounce_EnterBtn.update();
		PB_EnterBtn_value = debounce_EnterBtn.read();	
		
		if (PB_EnterBtn_value==HIGH && PB_Last_EnterBtn_value==HIGH) {
			//No button push is in process...
			//PB_EnterBtn_DownEdge=LOW;	//Comment out to let application code reset these values
			//PB_EnterBtn_UpEdge=LOW;
		}		
		if (PB_EnterBtn_value==LOW && PB_Last_EnterBtn_value==HIGH) {
			//Detect a DOWN Edge...(Button being depressed)
			PB_EnterBtn_DownEdge=HIGH;
			PB_EnterBtn_UpEdge=LOW;
			PB_RepeatMs=millis()+PB_RepeatInitialDelayMs;
			//Serial.println ("==== PB2 DOWN Edge");
		}
		if (PB_EnterBtn_value==HIGH && PB_Last_EnterBtn_value==LOW) {
			//Detect an UP Edge...(Button being released)
			PB_EnterBtn_DownEdge=LOW;
			PB_EnterBtn_UpEdge=HIGH;
			//Serial.println ("==== PB2 UP Edge");
		}		
		PB_Last_EnterBtn_value=PB_EnterBtn_value;

	#endif
	
	// Turn on the LED if any button is pressed :
	if ( PB_IncrBtn_value == LOW || PB_DecrBtn_value == LOW || PB_NxtBtn_value== LOW ||PB_EnterBtn_value==LOW) {	
		digitalWrite(LED_PIN, HIGH );
		
		//Wakeup the display with any button push...(Example of How To Wakeup from a screen saver event)
		//To wakeup the screen, just call the setScreenSaverSecs routine with the Current Setting...
		// 1) First, retrieve the Current TIMEOUT Setting (in seconds)
		long curScreenSaverSetting = XYscope.getScreenSaveSecs();
		// 2) Then just Call the setScreenSave...routine with Current TIMEOUT value
		XYscope.setScreenSaveSecs(curScreenSaverSetting);	//This sequence wakes up screen and resets the time 
		
	} 
	else {
		digitalWrite(LED_PIN, LOW );
	}
}

//=======================================================
// DIAGNOSTIC ROUTINES
//=======================================================
void clearMonitorTerminalScreen(){
	//	Routine to clear serial monitor screen
	//
	//	Passed Parameters	NONE
	//
	//	Returns: NOTHING
	//
	//	20180526 Ver 0.0	E.Andrews	First cut.
	//	
	//	
	//If using a term emulator, here's the clear screen command...
	Serial.write(27);       // ESC command
	Serial.print("[2J");    // clear screen command
	Serial.write(27);
	Serial.print("[H");     // cursor to home command
	
	//If not, just put lots of line feeds in the data flow...
	for (int i=0;i<20;i++){
		Serial.println();
	}
}

void WaitForAnyKey(String msg) {
	// Routine outputs 'msg' to serial monitor port using Serial.print then halts execution and 
	// waits for Opr to hit any key.  Then it will return so execution can continue.
	// Used as a diagnostic tool to provide a 'halt execution and wait' function for
	// debugging use
	//
	//	Passed Parameters	
	//		msg - 	This string is output to the Serial Monitor port, then
	//				execution halts until the operator touches any key.
	//
	//	Returns: NOTHING
	//
	//	20170708 Ver 0.0	E.Andrews	First cut
	//

	boolean wait;
	Serial.print(msg);
	Serial.println(F(" <-WAITING! (...Hit any Key to CONTINUE...)"));
	wait = true;
	while (wait) {
		if (Serial.available()) {
			int d=0;
			while (d>=0) {
				d=Serial.read();	//clean out the buffer (Retrieved data is NOT USED)
			}						//...keep reading until buffer is empty; note, Serial.read will return as -1 when no data is available
			wait = false;
			Serial.println(F("_KeyPressed_"));
			delay(100);
		}

	}
}
void ShowMemory() {
	// Routine outputs an estimate of the total RAM in active use to the serial monitor port.
	// Used during development to observe RAM consumption.
	//
	//	Passed Parameters	NONE
	//
	//	Returns: NOTHING
	//
	//	20170725 Ver 0.0	E.Andrews	First cut
	//	20180425 Ver 1.0	E.Andrews	Now handles DUE and TEENSY processors	
	//
	#if defined(__SAM3X8E__)	//Only compile this code block for Arduino DUE
		//----------------------------------------------------    
		//  DUE CODE BLOCK
		//----------------------------------------------------
		char *heapend = sbrk(0);
		register char * stack_ptr asm ("sp");
		struct mallinfo mi = mallinfo();
		printf("\n DUE Dynamic ram used: %d\n", mi.uordblks);
		printf("  DUE Program static ram used %d\n", &_end - ramstart);
		printf("  DUE Stack ram used %d\n\n", ramend - stack_ptr);
		printf("     ... My guess at free mem: %d\n",
				stack_ptr - heapend + mi.fordblks);
	#else
		//----------------------------------------------------    
		//  TEENSY CODE BLOCK
		//----------------------------------------------------		
	
		//Serial.println("\n No memory usage report available for TEENSY 3.6.");	
	#endif
}
