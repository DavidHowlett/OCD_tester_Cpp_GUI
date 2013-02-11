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
	DataCount 			= 10000; // this just causes the first request for data to be sent
	WaitCount				= 0;
	BadDataCount		= 0;
	Ring = new RingBuffer(RingSize);
	// fill the ring buffer with null chars
	for (RingPosition=0;RingPosition<RingSize;RingPosition++)
		Ring->Write(RingPosition,0);
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

		GetNewData();
		RingPosition-=2; // the TSI terminates data with 2 bytes this is not useful so it is deleted
	}
	LastBytesRead=BytesRead;
	ReadFile(		TsiPortHandle,	//HANDLE        hFile,
							TmpBuffer,      //LPVOID        lpBuffer,
							TmpBufferSize,   //DWORD         nNumberOfBytesToRead,
							&BytesRead,     //LPDWORD       lpNumberOfBytesRead,
							FALSE);

	Ring->WriteString(RingPosition+1,BytesRead,TmpBuffer);
	DataCount+=BytesRead;
	RingPosition+=BytesRead;
	if (BytesRead>0)
		WaitCount=0;
	else
		WaitCount++;
	for (int i = 0; i < RingSize; i++)
		DiagnosticBuffer[i] = Ring->Read(i);
	LastByteOfGoodDataset =RingPosition-DataCount%6;
	if (LastByteOfGoodDataset>=5){
		TmpMassFlow		=(Ring->Read(LastByteOfGoodDataset-5)*265.0+Ring->Read(LastByteOfGoodDataset-4))/1000.0;
		TmpTemperature=(Ring->Read(LastByteOfGoodDataset-3)*265.0+Ring->Read(LastByteOfGoodDataset-2))/100.0;
		TmpPressure		=(Ring->Read(LastByteOfGoodDataset-1)*265.0+Ring->Read(LastByteOfGoodDataset))/10000.0;
		if(	(200<TmpMassFlow)		||(TmpMassFlow<-0.1)
			||(60<TmpTemperature)	||(TmpTemperature<10)
			||(1.5<TmpPressure)		||(TmpPressure<0.5)){
			BadDataCount++;
		}else{ // I only want to update the data visible to the rest of the program if it is good.
			LastMassFlow		= TmpMassFlow;
			LastTemperature	= TmpTemperature;
			LastPressure		= TmpPressure;

    }
	}
	if(BadDataCount>1)
		Sleep(1); // this is used to provide a source breakpoint
}
int TsiFlowmeter::CheckAndSyncData(){

}
void TsiFlowmeter::GetNewData(){
	ClearBuffer();// this clears window's internal buffer
	DataCount = 0;
	Write("DBFTP1000");
	//SetCommTimeouts(TsiPortHandle,&WaitForData);
	Sleep(10); // experimentaly it was found that sleeping for 7 or less was unreliable.
	// The TSI sends a single null char to start communication, it has no value
	// to me and it causes synchronization issues so I dump it
	ReadFile(		TsiPortHandle,	//HANDLE        hFile,
							TmpBuffer,      //LPVOID        lpBuffer,
							1,              //DWORD         nNumberOfBytesToRead,
							&BytesRead,     //LPDWORD       lpNumberOfBytesRead,
							FALSE);
	//SetCommTimeouts(TsiPortHandle,&StdTimeouts);
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
	ClearBuffer();// this clears window's internal buffer
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
void TsiFlowmeter::ClearBuffer(){
	do
		ReadFile(TsiPortHandle,BigBuffer,BigBufferSize,&BytesRead,FALSE);
	while(BytesRead!=0);
}
TsiFlowmeter::~TsiFlowmeter(){
}
