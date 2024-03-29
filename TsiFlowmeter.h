#ifndef TsiFlowmeterH
#define TsiFlowmeterH
#include <ExtCtrls.hpp>
class RingBuffer;
#include "AbstractFlowMeter.h"

class TsiFlowmeter: public AbstractFlowMeter{
	public:
		TsiFlowmeter(int Port);
		bool CheckPresence();		  	// checks if the flowmeter answers to sent data
		void CallMeRegularly();
		~TsiFlowmeter();
	private:
		void AskForData();
		bool TestDataWithOffset(int Offset);
		int Write(char*);       // sends a c string to the flowmeter
	private:
		long long RingPosition; // this is the position of the most recently written char in the buffer
		int DataCount; 					// this is the number of valid chars of data put into the buffer since the data request
		int WaitCount; 					// this is the number of times in a row no data has been recieved
		int BadDataCount;
		long long PositionToTry;
		long long PreviousGoodPosition;


		HANDLE TsiPortHandle;
		static const int TmpBufferSize=4000;
		unsigned char TmpBuffer[TmpBufferSize];
		unsigned long BytesRead;
		RingBuffer * Ring;
		static const int RingSize = 1000;
		int DiagnosticBuffer[RingSize];
		COMMTIMEOUTS StdTimeouts;
		COMMTIMEOUTS WaitForData;
		float TmpMassFlow;
		float TmpTemperature;
		float TmpPressure;
};

#endif

