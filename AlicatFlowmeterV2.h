#ifndef AlicatFlowmeterV2H
#define AlicatFlowmeterV2H
#include <ExtCtrls.hpp>
class RingBuffer;

class AlicatFlowmeterV2{ // this class is based on the TsiFlowmeter class
	public:
		AlicatFlowmeterV2(int Port);
		bool CheckPresence();		  	// checks if the flowmeter answers to sent data
		void CallMeRegularly();
		bool IsThereNewData();
		double DataAge();   		// the age of the most recent data measured in seconds.
		float MassFlow(); 		// should be SCCM
		float Temperature(); 	// should be celcius
		float Pressure(); 		// should be bar
		~AlicatFlowmeterV2();
	private:
		bool ProcessData();
		int Write(char*);       // sends a c string to the flowmeter
	private:
		long long RingPosition; // this is the position of the most recently written char in the buffer




		long long PreviousGoodPosition;
		bool ThereIsNewData;
		LARGE_INTEGER TicksAssosiatedWithLastGoodData;
		LARGE_INTEGER Frequency;        // ticks per second

		HANDLE AlicatPortHandle;
		static const int TmpBufferSize=4000;
		unsigned char TmpBuffer[TmpBufferSize];
		unsigned long BytesRead;
		RingBuffer * Ring;
		static const int RingSize = 1000;
		int DiagnosticBuffer[RingSize];
		COMMTIMEOUTS StdTimeouts;
		float TmpMassFlow;
		float TmpVolumetricFlow;
		float TmpTemperature;
		float TmpPressure;
		float LastMassFlow;
		float LastTemperature;
		float LastPressure;
};

#endif
