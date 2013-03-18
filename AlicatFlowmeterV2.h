#ifndef AlicatFlowmeterV2H
#define AlicatFlowmeterV2H
#include <ExtCtrls.hpp>
class RingBuffer;
#include "AbstractFlowMeter.h"

class AlicatFlowmeterV2: public AbstractFlowMeter{ // this class is based on the TsiFlowmeter class
	public:
		AlicatFlowmeterV2(int Port);
		bool CheckPresence();		  	// checks if the flowmeter answers to sent data
		void CallMeRegularly();
		~AlicatFlowmeterV2();
	private:
		bool ProcessData();
		int Write(char*);       // sends a c string to the flowmeter
	private:
		long long RingPosition; // this is the position of the most recently written char in the buffer
		long long PreviousGoodPosition;
		HANDLE AlicatPortHandle;
		static const int TmpBufferSize=4000;
		unsigned char TmpBuffer[TmpBufferSize];
		unsigned long BytesRead;
		RingBuffer * Ring;
		static const int RingSize = 1000;
		//int DiagnosticBuffer[RingSize];
		COMMTIMEOUTS StdTimeouts;
		float TmpMassFlow;
		float TmpVolumetricFlow;
		float TmpTemperature;
		float TmpPressure;
};

#endif
