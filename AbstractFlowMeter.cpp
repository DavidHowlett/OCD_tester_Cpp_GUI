#include <windows.h>
#pragma hdrstop
#include "AbstractFlowMeter.h"
#pragma package(smart_init)
 AbstractFlowMeter::AbstractFlowMeter(){
	QueryPerformanceFrequency(&Frequency);  // get ticks per second
	QueryPerformanceCounter(&TicksAssosiatedWithLastGoodData); // this should be over written once data has been recived
	ThereIsNewData=false;
	LastMassFlow 		= -2;
	LastVolumetricFlow 	= -2;
	LastTemperature 	= -2;
	LastPressure 		= -2;
 }
double AbstractFlowMeter::DataAge(){
	LARGE_INTEGER CurrentTicks;
	QueryPerformanceCounter(&CurrentTicks);
	return double(CurrentTicks.QuadPart - TicksAssosiatedWithLastGoodData.QuadPart)/double(Frequency.QuadPart); // this calculates the time in seconds from the last time there was good data
}
bool AbstractFlowMeter::IsThereNewData(){
	// the overall intention is that the other classes can detect if there is
	// new data avalible
	bool tmp=ThereIsNewData;
	ThereIsNewData = false;
	return tmp;
}
float AbstractFlowMeter::MassFlow(){
	return LastMassFlow;
}
float AbstractFlowMeter::Temperature(){
	return LastTemperature;
}
float AbstractFlowMeter::Pressure(){
	return LastPressure;
}
