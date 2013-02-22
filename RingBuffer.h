#ifndef RingBufferH
#define RingBufferH

class RingBuffer{
	public:
		RingBuffer(int GivenRingSize);
		int SizeOfRealBuffer();
		void Write			(long long GivenRingPosition, unsigned char ToWrite);
		void WriteString	(long long GivenRingPosition,int NumberOfCharsToWrite, 	unsigned char * CharsToWrite);
		char Read			(long long GivenRingPosition);
		void ReadString		(long long GivenRingPosition,int NumberOfCharsToRead,	unsigned char * BufferToWriteTo);
	private:
		int * TheBuffer;
		int RealRingSize;
};

#endif
