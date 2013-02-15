#ifndef TsiFlowmeterH
#define TsiFlowmeterH
#include <ExtCtrls.hpp>
class RingBuffer;

class TsiFlowmeter{
	public:
		TsiFlowmeter(int Port);
		bool CheckPresence();		  	// checks if the flowmeter answers to sent data
		void CallMeRegularly();
		bool IsThereNewData();
		float MassFlow();
		float Temperature();
		float Pressure();
		~TsiFlowmeter();
	private:
		void AskForData();
		bool TestDataWithOffset(int Offset);
		int Write(char*);       // sends a c string to the flowmeter
	private:
		// variables used exclusively by ClearBuffer
		static const int BigBufferSize = 10000;
		char BigBuffer[BigBufferSize];

		long long RingPosition; // this is the position of the most recently written char in the buffer
		int DataCount; 					// this is the number of valid chars of data put into the buffer since the data request
														//the last request of the TSI for data. This does not include the initial null char.
		int WaitCount; 					// this is the number of times in a row no data has been recieved
		int BadDataCount;
		long long PositionToTry;
		long long PreviousGoodPosition;
		bool ThereIsNewData;

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
		float	TmpPressure;
		float LastMassFlow;
		float LastTemperature;
		float LastPressure;
};

#endif

