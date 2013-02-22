#include <iostream.h>
#include <cstring.h>
#include <windows.h>
#pragma hdrstop
#include "AlicatFlowmeterV2.h"
#include "RingBuffer.h"

AlicatFlowmeterV2::AlicatFlowmeterV2(int Port){
	LastMassFlow 		= -2;
	TmpVolumetricFlow 	= -2;
	LastTemperature 	= -2;
	LastPressure 		= -2;
	PreviousGoodPosition= -1; // this is deliberately initialised to a bogus position
	Ring = new RingBuffer(RingSize);
	for (RingPosition=0;RingPosition<RingSize;RingPosition++)
		Ring->Write(RingPosition,0); // fill the ring buffer with null chars
	RingPosition=-1;// as there have been no chars written to the array at this point
									// it is as though the most recent char to have been written to
									// the array is -1
	const size_t Newsize = 100;
	wchar_t MyWideString[Newsize];
	swprintf(MyWideString,Newsize, L"\\\\.\\COM%d",Port); // note that this is the wide char equivelent of sprintf, the capital L tells the program that input string should be wide char
	AlicatPortHandle = CreateFile(	MyWideString,  	            //LPCTSTR lpFileName,
									GENERIC_READ | GENERIC_WRITE, //DWORD dwDesiredAccess ,
									0,	                  //DWORD dwShareMode (0 for COM port access),
									SECURITY_ANONYMOUS,	  //LPSECURITY_ATTRIBUTES lpSecurityAttributes,
									OPEN_EXISTING,	      //DWORD dwCreationDisposition (necessary to be OPEN_EXISTING for COM ports),
									FILE_ATTRIBUTE_NORMAL,//DWORD dwFlagsAndAttributes,
									0);	                  //HANDLE hTemplateFile
	assert((AlicatPortHandle!=INVALID_HANDLE_VALUE)&&(AlicatPortHandle!=NULL));
	DCB dcbCommPort;   //This structure is used to hold the COM port parameters
	dcbCommPort.DCBlength = sizeof(DCB); //This never needs to be changed, so set it on startup
	GetCommState(AlicatPortHandle, &dcbCommPort);  //Read whatever state is to avoid hComPort's random contents corrupting defaults

	dcbCommPort.BaudRate=38400;
	dcbCommPort.fParity =1;
	dcbCommPort.Parity  =0;
	dcbCommPort.ByteSize=8;
	dcbCommPort.StopBits=0; // this means 1 stop bit

	SetCommState(AlicatPortHandle, &dcbCommPort); //Write the modified result back.

	StdTimeouts.ReadIntervalTimeout         =MAXDWORD;
	StdTimeouts.ReadTotalTimeoutConstant    =0;
	StdTimeouts.ReadTotalTimeoutMultiplier  =0;
	StdTimeouts.WriteTotalTimeoutConstant   =100;
	StdTimeouts.WriteTotalTimeoutMultiplier =10;

	SetCommTimeouts(AlicatPortHandle,&StdTimeouts);

}
bool AlicatFlowmeterV2::CheckPresence(){
	return true; // improve this later
}
void AlicatFlowmeterV2::CallMeRegularly(){
	ReadFile(	AlicatPortHandle,	//HANDLE        hFile,
				TmpBuffer,      //LPVOID        lpBuffer,
				TmpBufferSize,   //DWORD         nNumberOfBytesToRead,
				&BytesRead,     //LPDWORD       lpNumberOfBytesRead,
				FALSE);
	Ring->WriteString(RingPosition+1,BytesRead,TmpBuffer);
	RingPosition+=BytesRead;
	for(int i=0; i<RingSize; i++)
		DiagnosticBuffer[i] = Ring->Read(i);
	ProcessData();
}
bool AlicatFlowmeterV2::ProcessData(){
	int Offset=0;
	for (int CR_Found=0;CR_Found<=2; Offset++) {
		if(13==Ring->Read(RingPosition-Offset)){ //13 is a carage return
			CR_Found++;
		}
		if(Offset>=RingSize)
			return false; // this causes ProcessData to fail early if there is no data avalible
	}
	Offset-=2; // this is a fudge to make sure that the data given to the scanf below is good
	const int SmallBufferSize = 100;
	char SmallBuffer[SmallBufferSize];
	Ring->ReadString(RingPosition-Offset,SmallBufferSize,SmallBuffer);
	sscanf(SmallBuffer,"%f %f %f %f",&TmpPressure,&TmpTemperature,&TmpVolumetricFlow,&TmpMassFlow);
	TmpPressure	  		=TmpPressure* 0.0689475729;// this converts from Psi to Bar
	if((200000<TmpMassFlow)		||(TmpMassFlow<-200000)
		||(50<TmpTemperature)	||(TmpTemperature<0)
		||(20<TmpPressure)		||(TmpPressure<0)){
		return false; // this checks that the data is sensible
	}else{ // I only want to update the data visible to the rest of the program if it is good.
		if ((RingPosition-Offset)!=PreviousGoodPosition){
			ThereIsNewData = true;
			PreviousGoodPosition = (RingPosition-Offset);
		}
		LastMassFlow		= TmpMassFlow;
		LastTemperature		= TmpTemperature;
		LastPressure		= TmpPressure;
		return true;
	}
}
bool AlicatFlowmeterV2::IsThereNewData(){
	// the over all intention is that the other classes can detect if there is
	// new data avalible
	bool tmp=ThereIsNewData;
	ThereIsNewData = false;
	return tmp;
}
float AlicatFlowmeterV2::MassFlow(){
	return LastMassFlow;
}
float AlicatFlowmeterV2::Temperature(){
	return LastTemperature;
}
float AlicatFlowmeterV2::Pressure(){
	return LastPressure;
}
AlicatFlowmeterV2::~AlicatFlowmeterV2(){
	PurgeComm(AlicatPortHandle,PURGE_RXCLEAR&PURGE_TXCLEAR);// this ditches the data in the port
	CloseHandle(AlicatPortHandle);
}
