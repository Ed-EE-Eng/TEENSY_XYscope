
#ifndef XYs		//Include-Guard to prevent multiple includes...
	#define XYs
	
	#if (ARDUINO >=100)
		#include "Arduino.h"
	#else
		#include "WProgram.h"
	#endif
	#include "XYscopeConfig.h"
	
	//Make sure we are compiling for an Arduino DUE or Teensy 3.6 processor
	#if defined(__SAM3X8E__)		//This will dump a diag message to screen during compilation - Comment-out next line to suppress message
		//#pragma message "XYscope Lib: Compiling for Arduino DUE"	
	#elif defined(__MK66FX1M0__)	//This will dump a diag message to screen during compilation - Comment-out next line to suppress message
		//#pragma message "XYscope Lib: Compiling for TEENSY 3.6"		
	#else							//This will stop compilation and give a 'wrong processor' message as needed
		#error WRONG PROCESSOR SELECTED - XYscope Library only works with Arduino DUE & Teensy 3.6 Processors
	#endif

	const uint8_t _VectorFont=0;
	const uint8_t _HersheyFont=1;	



	extern void ChangeTeensyRefreshInterval(long duration);

	//void paintCrt2_ISR();						//This is the Refresh CRT ISR declaration
	//void XY_TimerTestISR();					//TODO this is a test routine...Delete ASAP
	//void initiateDacDma(void);				//Fires off a DacDMA transfer
	//void initiateDacDma(short *ArrayPtr,int NumPoints,int blankCount,int crtBlankingPin); //Another version of the InitialDacDMA routine

	class RefreshTimer{
		public:
		RefreshTimer();
		void update(long duration);
	};

	class XYscope{
	  public:
		// Constructor
		XYscope();
		
		//Methods

		//Setup and Control Routines
		float getLibRev(void);								//Retrieves the revision level of the active library
		void begin(uint32_t dmaFreqHz=800000);				//Initialize & enab DAC, DAC Ctr-Tmr(DUE), DMA cntrlr(DUE),& Refresh Counter Timer
															//	Specify default dmaFreqHz value in case caller doesn't supply it
		void initiateDacDma(void);							//NOT A USER ROUTINE: DUE ONLY - Fires off DMA Screen Refresh
		void initiatePioScreenPaint(void);					//NOT A USER ROUTINE: TEENSY 3.6 ONLY - Fires off PIO Screen Refresh
		void initiateDuePioScreenPaint(void);					//NOT A USER ROUTINE: TEENSY 3.6 ONLY - Fires off PIO Screen Refresh
		void initiatePioScreenPaint_TODO(void);					//NOT A USER ROUTINE: TEENSY 3.6 ONLY - Fires off PIO Screen Refresh
		void dacHandler(void);								//NOT A USER ROUTINE: This is link to DAC_TRANSFER_COMPLETED interrupt

		void setScreenSaveSecs(long ScreenOnTime_sec);	//Set the screen saver time. Set to Zero to disable screen saver.
		
		long screenOnTimeDefaultSec=600;				//Define power-up default setting.

		long getScreenSaveSecs(void);					//Retrieve current screen saver time. Zero=screen saver disabled.

		void setDmaClockRate(uint32_t New_XfrRateHz);	//Allows user to change output Clk rate during run-time

		void setRefreshPeriodUs(uint32_t refresh_us);	//Can be used to manually change the refresh period (us) during run-time.

		void autoSetRefreshTime();						//Automatically sets the best refresh time based on the number points being plotted
		long getRefreshPeriodUs(void);					//Retrieves the currently active refresh period (us).

		//Buffer Management Routines
		void plotStart();				//Reset current buffer pointer to zero, effectively erasing the existing XY_List array
		void plotClear();				//Reset current buffer pointer to zero, effectively erasing the existing XY_List array and turning the display OFF
		void plotEnd();					//Makes sure last points in XYlist are actually actually visualized

		//Graphics Plotting Routines

		void setGraphicsIntensity(short GraphBright=100);	//Set (Get) brightness of Lines, Circles, Ellipse, Rectangles (in percent)
		short getGraphicsIntensity();							//Nominal setting is 100. Usable range is 50-200.

		short getGraphDensity();								//OBSOLETE-DO NOT USE. Function returns dot-to-dot spacing value in use (driven by intensity setting)

		void setTextIntensity(short TextBright=100);		//Set (Get) brightness of Text (in percent)
		short getTextIntensity();								//Nominal setting is 100. Usable range is 50-200.
		short getTextDensity();								//OBSOLETE-DO NOT USE. Function returns dot-to-dot spacing value in use (driven by intensity setting)

		void plotPoint(int x0, int y0);											// Plots a POINT
		void plotLine(int x0, int y0, int x1, int y1);							// Plots Lines (aka: a Vector)
		void plotRectangle(int x0, int y0, int x1, int y1);						// Plots a rectangle	
		void plotCircle(int xc, int yc, int r);									// Plots a circle centered at (xc,yc) of radius "r"	
		void plotCircle(int xc, int yc, int r, uint8_t arcSegment);				// Plots a circle centered at (xc,yc) of radius "r" BUT just specified arcSegment(s) of the circle.
		void plotCircleBres(int xc, int yc, int r, uint8_t arcSegment);			// Bresenham Algorithm:Plots a circle centered at (xc,yc) of radius "r" BUT just specified arcSegment(s) of the circle.
		void plotEllipse(int xc, int yc, int xr, int yr);						// Plots an ellipse
		void plotEllipse(int xc, int yc, int xr, int yr,uint8_t arcSegment);	// Plots an elliptical arc BUT just specified arcSegment(s)of the ellipse.
		void plotEllipseBres(int xc, int yc, int xr, int yr,uint8_t arcSegment=255);	//Bresenham Algorithm:Plots an ellipse (does Not use Bressham!)
		
		void plotChar(char c, int& x0, int& y0, int& charHt);					//Plot a single Character
		void plotChar_V(char c, int& x0, int& y0, int& charHt);					//Plot a single Character using VectorFont
		void plotChar_H(char c, int& x0, int& y0, int& charHt);					//Plot a single Character using HersheyFont
		
		void plotCharUL(char c, int& x0, int& y0, int& charHt);					//Plot a single character WITH UNDERLINE using VectorFont
		
		short getActiveFont();													//Retrieve currently active font
		void setActiveFont(int FontSelect);										//Sets active font
		
		void plotHersheyChar(char c, int& x0, int& y0, int& charHt);			//Plot a single Character using HersheyFont

		void plotArduinoLogo(int& charX, int& charY, int& charHt);				//Plot the Arduino LOGO to screen

		const short prop=0, monoTight=8, mono=10, monoNorm=10, monoWide=12;	//Define FontSpacing  Constants
		void setFontSpacing(short spacingMode=0);	//Set Font Proportional or mono spaced mode...
		short getFontSpacing();						//retrieve current font spacing

		void printSetup(short textX, short textY);									//Set X-Y location for start of text
		void printSetup(short textX, short textY, short textSize);					//Set X-Y location for start of text, set textSixe (pixels)
		void printSetup(short textX, short textY, short textSize, short textDensity);	//Set X-Y location for start of text, set textSixe (pixels), textDensity(Brightness)
		void printSetup(short textX, short textY, short textSize, short textDensity, short Justification);	//Set X-Y location for start of text, set textSixe (pixels), textDensity(Brightness), Left(0),Right(1),Center(1) justification mode	
		
		void printUnderline(int nPlaces);			//Print UNDERLINE character(s)
		void print(char const * text);				//Print text string (starting at current X-Y location, size, & density, no underline)
		void print(char const * text,bool UL_flag);	//Print text string (starting at current X-Y location, size, & density, with/without UNDERLINE)
		void print(int number);						//Print integer to screen (starting at current X-Y location, size, & density, no underline)
		void print(int number,bool UL_Flag);		//Print integer to screen (starting at current X-Y location, size, & density, with/without UNDERLINE)
		void print(float number);					//Print floating point number to screen (starting at current X-Y location, size, & density), nPlaces to right of DP
		void print(float number,int placesToPrint);	//Print floating point number to screen (starting at current X-Y location, size, & density), nPlaces to right of DP, without UNDERLINE
		void print(float number,int placesToPrint,bool UL_Flag);	//Print floating point number to screen (starta at current X-Y location, size, & density), nPlaces to right of DP, with/without UNDERLINE

		

		void disableDac(void); 						//NOT A USER ROUTINE - Turns dac Off...May not need this

		//========================================
		//Global Variables & Constants
		//========================================
		
		//Global Variables used for Performance Metrics Variables
		long act_PaintTimeDurationMs,act_RefreshIntervalMs,act_PriorRefreshTimeStampMs;
		long measured_PaintTimeUs, act_PaintTimeDurationUs,act_RefreshIntervalUs,act_PriorRefreshTimeStampUs;	
		int act_NumPointsPlotted;

		
		//Arduino DUE ONLY (DMA related) Variables & Constants
		static const uint8_t crtBlankingPin=3;		//CRT_Blanking pin (1=CRT_OFF, 0=CRT_ON)
		uint16_t frontPorchBlankCount=100;			//Calculated and set by setDmaClockRate(int dmaClkFreq); Used to define the duration before CRT unblanks & displays starts at START of DMA transfer
		uint16_t backPorchBlankCount=100;			//Calculated and set by setDmaClockRate(int dmaClkFreq); Used to define the duration before CRT blanking starts and display ENDS after the completion of a DMA transfer
		uint32_t DmaClkFreq_Hz;						//Currently Active DMA Clock FREQUENCY (Hz)
		float DmaClkPeriod_us;						//Currently Active DMA Clock PERIOD (us)


		//TEENSY 3.6 ONLY (PIO related) Variables & Constants
		//CONFIGURATION	- Use this time to define the minimum compute time that will be allocated.  It only comes into play required plot time approaches the refresh period
		const long TNSY_3_6_MinComputeTimeUs = CFG_TNSY_3_6_MinimumComputeTimeUs;	




		//General use variables 
		uint32_t ActiveRefreshPeriod_us;			//Currently Active REFRESH Clock PERIOD (us)

		uint8_t ActiveFont=0;						//Defines the Active Font (0=VectorFontROM, 1=HersheyFontROM)
													//This gets re-initalized in XYscope.Begin()
													//Note, use CFG file to INCLUDE fontROM files as needed
													
		//CONFIGURATION - Set FONT available bits...
		bool VectorFontAvailable = CFG_IncludeVectorFontROM;
		bool HersheyFontAvailable = CFG_IncludeHersheyFontROM;
		
		//CONFIGURATION	- Mainline can change the setting of this value to vary the blanking for PIO TEENSY DAC transfers
		int16_t PioDelaySetting=240;				//Test value to set PIO blanking delays - FOR DEVELOPMENT ONLY
													// Value is two digit number 'ab' where 'a' - num of extra blanking steps preceed unblank command
													//                                      'b' - num if 'unblank' steps (duration of unblank-pulse)
													// Example: 73 means 7 extra preceeding delays in place (total = 8) and 3 unblank steps
													// See void XYscope::initiatePioScreenPaint(void) within XYscope.cpp for valid combinations
		
		//CONFIGURATION	- Set initial values for DacSettling and Unblank time, a.k.a. the "Default" settings
		uint8_t PioLargeSettleCount=CFG_PioLargeSettleCount;		//If more settling time is req'd, add this many steps of delay
		uint8_t PioSmallSettleCount=CFG_PioSmallSettleCount;		//If more settling time is req'd, add this many steps of delay
		uint8_t	PioUnblankCount=CFG_PioUnblankCount;				//This sets the width of the dot unblank pulse
		uint16_t NoSettlingTimeReqd=CFG_NoSettlingTimeReqd;			//Don't add settling time if points are close to one another
													
		//CONFIGURATION	- Use line to set the size of the XYlist buffer.  Note, Actual bytes used = 4 X MaxArraySize
		static const uint32_t MaxArraySize=CFG_MaxArraySize;	//This sets the max Array size reserved for XY list.  MaxBufferSize must always be <= MaxArraySize!

		int MaxBuffSize=MaxArraySize-100;			//This sets the size of the XY point plotting buffer (in XY-Point-Pairs). (There is a 100 extra elements defined as an overrun guard)
													// NOTE: Each XY point pair consumes 4 bytes of RAM. This means that a buffer size of 15000
													// actually uses 15K X 4 = 60K bytes of RAM.  To leave room for other variables, MaxBufferSize should
													// NEVER exceed MaxArraySize (about 20000 for DUE or 50000 for TEENSY).  Also note that a lots of XY points 
													// can require longer refresh times which will lead to display flicker!
		
		int XYlistEnd;								//This value points to the last element loaded into XYlist[] array
													//and is automatically maintained by Driver Routine "plotPoint"
													//By using the plotPoint routine, new points are automatically added at the END of the current list.
													//If you want to add point pairs directly into XYlist yourself, you must watch & maintain this value
													//and you must FOLLOW THESE RULES!
													//  1. Always use plotStart routine to setup a new list.  This places the needed snychronization pattern into the
													//     very start of a new list.  The hardware interface needs this pattern to run properly.
													//	2. When adding data into the XY_List array, you MUST include the X & Y flag values
													//     that are set into the high order bits of the X and Y integers. Look inside of plotPoint for details!
													//	3. XYlistEnd must stay BELOW MaxBufferSize or else you'll clobber other variables!
													//  4. XYlistEnd must be updated every time a point is added to the lsit.
													//	5. XYlistEnd always points the the NEXT AVAILABLE LOCATION in the XYList[] array.
													//	6. If the total number of points in the list gets too big, they may not all be displayed!
													//     If this happens, either reduce the the number of points in the list, or increase the refresh period.
													//	   You increase the the regresh period by changing the value of crtRefreshMs found else where in this file.						//


		// Define Structure of XY Point List.  This is the actual data array that holds the points to be plotted to the CRT.
		//  Use plotPoint or the other graphic draw routines to add points into this array.
		//  Use plotStart to initialize and/or start over with a new list

		struct pointList{
			short X;	//X-coordinate value of a point. Valid range: 0-4095 (See also the "rules" in XYlistEnd comment above!)
			short Y;	//Y-coordinate value of a point. Valid range: 0-4095 (See also the "rules" in XYlistEnd comment above!)
		};
		pointList XY_List[MaxArraySize];	//This reserves & defines the RAM allocated for the XY_List.  Actual value of usable space is set by variable MaxBuffSize

		//Define Justification Flags
		static const uint8_t LtJustify=0;	//Left Justified Text	(default)
		static const uint8_t RtJustify=1;	//Right Justified Text
		static const uint8_t CtrJustify=2;	//Center justified Text

		//Define partial-circle/partial-ellipse "Arc-Codes"
		static const uint8_t arc0=1;
		static const uint8_t arc1=2;
		static const uint8_t arc2=4;
		static const uint8_t arc3=8;
		static const uint8_t arc4=16;
		static const uint8_t arc5=32;
		static const uint8_t arc6=64;
		static const uint8_t arc7=128;
		
		
			
		uint8_t plotErr;						//plotPoint routine sets this variable when ever we attempt to plot too many points



		static const long CrtMinRefresh_ms=20;	//in milliseconds, Sets minimum refresh period (ms) for the display
												//  Example, 20 ms = 50Hz CRT refresh rate
												//  Note: Although not recommended, you can set value=0 for fastest possible refresh.  
												//	When set to zero, actual refresh rate WILL GREATLY VARY depending on the number of point-pairs in XYlist
												
		static const long CrtMinRefresh_us=20000;//in Microseconds, Sets minimum refresh period (us) for the display
														
		
		//DUE ONLY constants
		static const uint16_t X_flag=0x0000;	//These are routing codes used by the DMA hardware to route each integer the right DAC
		static const uint16_t Y_flag=0x1000;

		// Define & initialize global text X-Y coordinates and text size variables
		int charX=0,charY=4095, charSize=50;
		


		#if CFG_IncludeVectorFontROM == true
			#include "VectorFontROM.h"		//Include Font ROM file
		#endif
		
		#if CFG_IncludeHersheyFontROM == true
			#include "HersheyFontROM.h"
		#endif
		
		//_graphDensity & _textDensity are really private variables; they were made public for diagnostic printout purposes.
		int _graphDensity;		//value calculated by/set by call to SetGraphicsIntensity(int brightness)	
		int _textDensity;		//value calculated by/set by call to SetTextIntensity(int brightness)
		
	  private:
		//Methods
		//void paintCrt_isr();			//This routine Calls for a SCREEN REFRESH every crtRefreshMs milliseconds...this routine to refresh CRT as needed
		void dacSetup (void);				//Called within begin(). Initializes and enables dac peripherals.
		void tcSetup (uint32_t XfrRateHz);	//Called within begin().  Used to initialize Timer Counter TC0 (Drive DAC_DMA channel) at target transfer rate
		uint32_t FreqToTimerTicks(uint32_t freqHz);	//DUE ONLY - Used within tcSetup to set DMA_Clock Rate


		//Private Variables


		//int _graphDensity;		//value calculated by/set by call to SetGraphicsIntensity(int brightness)
		int _graphBrightness;	//value that is set by call to SetGraphicsIntensity(int graphbrightness)

		//int _textDensity;		//value calculated by/set by call to SetTextIntensity(int brightness)
		int _textBrightness;	//value that is set by call to SetGraphicsIntensity(int graphbrightness)

		unsigned long _crtOffTOD_ms;	//Screen save time in ms; Zero means screen saver disabled.
		unsigned long _screenOnTime_ms;	//TOD (ms) when Screen should next be blanked;

		//Define Font Spacing variable
		short _fontSpacing=8;
		uint8_t	_fontJustifyFlag;	//Global variable to define justification
		uint8_t	_fontJustifyEnab;	//0=disable justification (default to LEFT); !0=enable justification as set by _fontJustifyFlag


		//Define XYscope Library Version using Semantic scheme (Major_Rev, Minor_Rev, Patch_Rev)
		float _libMajorRev=2;	//Valid range: 0-99
		float _libMinorRev=0;	//Valid range: .00-.99
		short _libPatchRev=0;	//Valid range: 0-99
		float _libRev=0.0;

	};
#endif // XYscope


