/*
	SIMPLE_BOXES
	
	This program plots just a simple series of boxes on the screen.
	
	20181019 Ver  0.00	E.Andrews	Simple routines provided with initial TEENSY release

*/
// 	+-----Begin Includes, Timer creation, & XYscope declaration-----+
//	|																|
#include <Arduino.h>			//Provided as part of the Arduino IDE
#include <XYscope.h>			//Add XYscope library into program.
IntervalTimer RefreshTimer;		//Create an instance of the interval timer to control refresh events
								//This must be done in MAIN CODE block
XYscope XYscope;
//	|																|
// 	+---------Begin Critical Interrupt Service Routines ------------+
//	|  These routines MUST be declared as shown at the top of the	|
//	|      user's main line code for all XYscope Projects!			|
// 	+---------------------------------------------------------------+
//	|																|
void paintCrt_ISR(void) {	//Used for both DUE an TEENSY implementation
	//	paintCrtISR  Interrupt Service Routine. This routine
	//	provides a 'wrapper' to link Timer3.AttachedInterrupt()
	//	function to the 'XYscope-class' ISR routine.
	XYscope.initiatePioScreenPaint();
}
void ChangeTeensyRefreshInterval(long duration_us){	//Req'd for TEENSY ONLY
	//This line links the TEENSY RefreshTimer to the XYscope library
	RefreshTimer.update(duration_us);
}
//	|																|
// 	+---------- END Critical Includes & Declarations ---------------+


 void setup() {
	// Mainline program SETUP routine.
	
	//------------------------------------------------------------
	//	TEENSY STARTUP CODE BLOCK 
	//------------------------------------------------------------	
	//
	
	XYscope.begin();											//TEENSY startup
	RefreshTimer.begin(paintCrt_ISR,XYscope.CrtMinRefresh_us);	//CrtMinRefresh_us defined in XYscope.h
 }

void loop() {
	
	XYscope.plotStart();
	int x0=0,y0=0,x1=4095,y1=4095;
	int NumOfBoxes = 20;
	int dx=4096/NumOfBoxes;
	int dy=4096/NumOfBoxes;
	for (int i=0;i<20;i++){
		XYscope.plotRectangle(x0,y0,x1,y1);
		x0=x0+dx;	x1=x1-dx;
		y0=y0+dy;	y1=y1-dy;
	
	}
	XYscope.plotEnd();
	XYscope.setScreenSaveSecs(0);		//Turn screen save function off 
										//You probably don't want to do this for most apps
										//as this this can burn patterns into the CRT screen!
	while(true){
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
	XYscope.plotStart();
	XYscope.plotPoint(0, 4095);		//Plot point to upper LH corner
	int xCtr = 2047;
	int yCtr = 2047;
	int xRadius = 2047;
	int yRadius = 2047;

	float numTestPoints = 160;		//Plot a point every 2 degrees
	float testAngle = 0;
	int patternCount = 0;
	float angleStep = 2 * 3.141526 / numTestPoints;
	testAngle = testAngle - angleStep;
	for (patternCount = 0; patternCount < numTestPoints; patternCount++) {
		testAngle = testAngle + angleStep;
		XYscope.plotPoint(int(xCtr + xRadius * sin(testAngle)),int(yCtr + yRadius * cos(testAngle)));
	}
	XYscope.plotPoint(4095, 4095);	//Plot point to upper RH corner
	XYscope.plotEnd();
}