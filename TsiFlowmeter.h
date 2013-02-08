#ifndef TsiFlowmeterH
#define TsiFlowmeterH
#include <ExtCtrls.hpp>
class RingBuffer;

class TsiFlowmeter{
	public:
		TsiFlowmeter(int Port);
		bool CheckPresence();		  	// checks if the flowmeter answers to sent data
		void CallMeRegularly();
		float MassFlow();
		float Temperature();
		float Pressure();
		~TsiFlowmeter();
	private:
		void GetNewData();
		int Write(char*);       // sends a c string to the flowmeter
		void ClearBuffer();			// clears window's internal buffer
		// variables used exclusively by ClearBuffer
		static const int BigBufferSize = 10000;
		char BigBuffer[BigBufferSize];

		// variables used exclusively by CallMeRegularly
		long long RingPosition; // this is the position of the most recently written char in the buffer
		int DataCount; 					// this is the number of valid chars of data put into the buffer since the data request
														//the last request of the TSI for data. This does not include the initial null char.

		// other private variables
		HANDLE TsiPortHandle;
		static const int TmpBufferSize=200;
		unsigned char TmpBuffer[TmpBufferSize];
		unsigned long BytesRead;
		unsigned long LastBytesRead;
		RingBuffer * Ring;
		static const int RingSize = 300;
		COMMTIMEOUTS StdTimeouts;
		COMMTIMEOUTS WaitForData;
		float LastMassFlow;
		float LastTemperature;
		float LastPressure;
};

#endif

