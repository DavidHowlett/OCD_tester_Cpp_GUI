#ifndef RingBufferH
#define RingBufferH

class RingBuffer{
	public:
		RingBuffer(int GivenRingSize);
		int SizeOfRealBuffer();
		void Write(long long GivenRingPosition, unsigned char ToWrite);
		void WriteString(long long GivenRingPosition,
										int NumberOfCharsToWrite, unsigned char * CharsToWrite);
		int Read(long long GivenRingPosition); //this should return a char, fix tommorow
	private:
		int * TheBuffer;
		int RealRingSize;
};

#endif
