#include "RingBuffer.h"

RingBuffer::RingBuffer(int GivenRingSize){
	RealRingSize=GivenRingSize;
	TheBuffer = new int[RealRingSize];
}

int RingBuffer::SizeOfRealBuffer(){
	return RealRingSize;
}
void RingBuffer::Write(long long GivenRingPosition, unsigned char ToWrite){
	TheBuffer[GivenRingPosition%RealRingSize] = ToWrite;
}
void RingBuffer::WriteString(	long long GivenRingPosition,
															int NumberOfCharsToWrite,
															unsigned char * GivenBuffer){
	for(int i=0;i<NumberOfCharsToWrite;i++)
		this->Write(GivenRingPosition+i,GivenBuffer[i]);
}
RingBuffer::Read(long long GivenRingPosition){
	return TheBuffer[GivenRingPosition%RealRingSize];
}

