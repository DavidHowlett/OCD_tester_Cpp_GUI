#include <iostream.h>
#include <cstring.h>
#include <windows.h>
#pragma hdrstop
#include "TsiFlowmeter.h"
#include "RingBuffer.h"


TsiFlowmeter::TsiFlowmeter(int Port){
	LastMassFlow 		= -2;
	LastTemperature = -2;
	LastPressure 		= -2;
	DataCount 			= 10000; // this causes the first request for data to be sent
	WaitCount				= 0;
	BadDataCount		= 0;
	PreviousGoodPosition = -1; // this is deliberately initialised to a bogus position
	Ring = new RingBuffer(RingSize);
	for (RingPosition=0;RingPosition<RingSize;RingPosition++)
		Ring->Write(RingPosition,0); // fill the ring buffer with null chars
	RingPosition=-1;// as there have been no chars written to the array at this point
									// it is as though the most recent char to have been written to
									// the array is -1
	const size_t Newsize = 100;
	wchar_t MyWideString[Newsize];
	swprintf(MyWideString,Newsize, L"\\\\.\\COM%d",Port); // note that this is the wide char equivelent of sprintf, the capital L tells the program that input string should be wide char
	TsiPortHandle = CreateFile(	MyWideString,  	            //LPCTSTR lpFileName,
															GENERIC_READ | GENERIC_WRITE, //DWORD dwDesiredAccess ,
															0,	                  //DWORD dwShareMode (0 for COM port access),
															SECURITY_ANONYMOUS,	  //LPSECURITY_ATTRIBUTES lpSecurityAttributes,
															OPEN_EXISTING,	      //DWORD dwCreationDisposition (necessary to be OPEN_EXISTING for COM ports),
															FILE_ATTRIBUTE_NORMAL,//DWORD dwFlagsAndAttributes,
															0);	                  //HANDLE hTemplateFile
	assert((TsiPortHandle!=INVALID_HANDLE_VALUE)&&(TsiPortHandle!=NULL));
	DCB dcbCommPort;   //This structure is used to hold the COM port parameters
	dcbCommPort.DCBlength = sizeof(DCB); //This never needs to be changed, so set it on startup
	GetCommState(TsiPortHandle, &dcbCommPort);  //Read whatever state is to avoid hComPort's random contents corrupting defaults

	dcbCommPort.BaudRate=38400;
	dcbCommPort.fParity =1;
	dcbCommPort.Parity  =0;
	dcbCommPort.ByteSize=8;
	dcbCommPort.StopBits=0; // this means 1 stop bit

	SetCommState(TsiPortHandle, &dcbCommPort); //Write the modified result back.

	StdTimeouts.ReadIntervalTimeout         =MAXDWORD;
	StdTimeouts.ReadTotalTimeoutConstant    =0;
	StdTimeouts.ReadTotalTimeoutMultiplier  =0;
	StdTimeouts.WriteTotalTimeoutConstant   =100;
	StdTimeouts.WriteTotalTimeoutMultiplier =10;

	WaitForData.ReadIntervalTimeout 				=5;
	WaitForData.ReadTotalTimeoutConstant 		=10;
	WaitForData.ReadTotalTimeoutMultiplier 	=2;
	WaitForData.WriteTotalTimeoutConstant  	=100;
	WaitForData.WriteTotalTimeoutMultiplier =10;

	SetCommTimeouts(TsiPortHandle,&StdTimeouts);

}
void TsiFlowmeter::CallMeRegularly(){
	if(	(DataCount>=6002&&WaitCount >0)
		||(DataCount>=6000)&&(WaitCount>10)){
		//there are 6000 bytes  of data and 2 stop bytes in a batch of readings from the tsi.
		AskForData();
		ReadFile(TsiPortHandle,TmpBuffer,1,&BytesRead,FALSE);
		// The TSI sends a single null char to start communication, it has no value
		// to me and it causes synchronization issues so I dump it
		RingPosition-=2; // the TSI terminates data with 2 bytes this is not useful so it is deleted
	}
	ReadFile(		TsiPortHandle,	//HANDLE        hFile,
							TmpBuffer,      //LPVOID        lpBuffer,
							TmpBufferSize,   //DWORD         nNumberOfBytesToRead,
							&BytesRead,     //LPDWORD       lpNumberOfBytesRead,
							FALSE);

	Ring->WriteString(RingPosition+1,BytesRead,TmpBuffer);
	DataCount+=BytesRead;
	RingPosition+=BytesRead;
	if (BytesRead>0) // I keep track of the length of time since I got incoming data
		WaitCount=0;
	else
		WaitCount++;
	for(int i=0; i < RingSize; i++)
		DiagnosticBuffer[i] = Ring->Read(i);
	for(int i=0;i>(0-RingSize);i--)
		if(TestDataWithOffset(i)){
			LastMassFlow		= TmpMassFlow;
			LastTemperature	= TmpTemperature;
			LastPressure		= TmpPressure;
			break;
		}
}
bool TsiFlowmeter::TestDataWithOffset(int Offset){
		PositionToTry =RingPosition+Offset-DataCount%6; // in an ideal world this should be the last byte of a good data set
		if (PositionToTry<5)
			return false;// negetive positions in the ring should not be acsessed
		TmpMassFlow		=(Ring->Read(PositionToTry-5)*265.0+Ring->Read(PositionToTry-4))/1000.0;
		TmpTemperature=(Ring->Read(PositionToTry-3)*265.0+Ring->Read(PositionToTry-2))/100.0;
		TmpPressure		=(Ring->Read(PositionToTry-1)*265.0+Ring->Read(PositionToTry))/10000.0;
		if(	(200<TmpMassFlow)		||(TmpMassFlow<-0.1)
			||(50<TmpTemperature)	||(TmpTemperature<10)
			||(1.5<TmpPressure)		||(TmpPressure<0.7)){
			BadDataCount++;
			return false;
		}else{ // I only want to update the data visible to the rest of the program if it is good.
			if (PreviousGoodPosition!=PositionToTry){
				ThereIsNewData = true;
				PreviousGoodPosition = PositionToTry;
			}
			return true;
		}
}
void TsiFlowmeter::AskForData(){
	PurgeComm(TsiPortHandle,PURGE_RXCLEAR&PURGE_TXCLEAR);
	DataCount = 0;
	Write("DBFTP1000");
	Sleep(10); // experimentaly it was found that sleeping for 7 or less was unreliable.
}
bool TsiFlowmeter::IsThereNewData(){
	// the over all intention is that the other classes can detect if there is
	// new data avalible
	bool tmp=ThereIsNewData;
	ThereIsNewData = false;
	return tmp;
}

float TsiFlowmeter::MassFlow(){
	return LastMassFlow;
}
float TsiFlowmeter::Temperature(){
	return LastTemperature;
}
float TsiFlowmeter::Pressure(){
	return LastPressure;
}
int TsiFlowmeter::Write(char* ToSend){             	// sends a c string to the flowmeter
	DWORD BytesWritten;					   //Number of bytes written
	AnsiString AStringSendMe(ToSend), CarrageReturn('\r'); // a carrage return is needed for the tsi flowmeter to recognise a command.
	AStringSendMe += CarrageReturn;
	WriteFile(		TsiPortHandle,                  //HANDLE hComFile
								AStringSendMe.c_str(),                  //LPCVOID lpBuffer,
								AStringSendMe.Length(),                  //DWORD nNumberOfBytesToWrite,
								&BytesWritten ,                  //LPDWORD lpNumberOfBytesWritten,
								FALSE);                  //LPOVERLAPPED lpOverlapped
	return BytesWritten;
}
bool TsiFlowmeter::CheckPresence(){
	PurgeComm(TsiPortHandle,PURGE_RXCLEAR&PURGE_TXCLEAR);// this clears window's internal buffer
	Write("SSR0001"); // the flowmeter should reply "OK" to this.// this also sets the reading rate to max
	SetCommTimeouts(TsiPortHandle,&WaitForData);
	ReadFile(		TsiPortHandle,                        //HANDLE        hFile,
							TmpBuffer,                          //LPVOID        lpBuffer,
							TmpBufferSize,                    //DWORD         nNumberOfBytesToRead,
							&BytesRead,                       //LPDWORD       lpNumberOfBytesRead,
							FALSE);
	SetCommTimeouts(TsiPortHandle,&StdTimeouts);
	if('O'==TmpBuffer[0] && 'K'==TmpBuffer[1])
		return true;
	else
		return false;
}
TsiFlowmeter::~TsiFlowmeter(){
	PurgeComm(TsiPortHandle,PURGE_RXCLEAR&PURGE_TXCLEAR);// this ditches the data in the port
	CloseHandle(TsiPortHandle);
}
