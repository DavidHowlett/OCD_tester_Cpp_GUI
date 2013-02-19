#include <iostream.h>
#include "SettingsFileManager.h"
#include "TsiFlowmeter.h"
#include "AlicatFlowmeter.h"
#include "FlowmeterManager.h"

//---------------------------------------------------------------------------
FlowmeterManager::FlowmeterManager(SettingsFileManager* GivenSettingsPointer,TListBox* GivenTEditPointer){
	SettingsPointer=GivenSettingsPointer;
	FlowmeterStatus=GivenTEditPointer;
	FlowmeterReady = false;
	PortNum = 0; // a port number of 0 is a special value
	FlowmeterType = None;
}
FlowmeterManager::CallMeRegularly(){
	// idea: insert code here to check if the flowmeter is working properly and if not then set FlowmeterReady = false;
	if(!FlowmeterReady)			Setup();
	if(Tsi == FlowmeterType)	TsiPointer->CallMeRegularly();
	return 0;
}
void FlowmeterManager::Setup(){ // this method should finish quickly to allow processing of the message queue.
	// the flowmeter port does not change often so first try the one that worked last time
	// and then systematicly try them all
	int TmpPortNum;
	if(PortNum==0)
		TmpPortNum = SettingsPointer->LatestGoodFlowmeterPort;
	else
		TmpPortNum = PortNum;
	if(TestPortExistence(TmpPortNum)){
		if (AttemptTsiSetup(TmpPortNum))
			return ;
		if (AttemptAlicatSetup(TmpPortNum))
			return ;
	}
	PortNum++;

	// if no flowmeter is found then tell the user and restart the search
	if(PortNum>MAX_PORT_NUMBER){
		PortNum=1;
		FlowmeterStatus->Items->Add("Warning: no flowmeter found");
	}
	return;
}
bool FlowmeterManager::TestPortExistence(int GivenPort){// it would save the time of the user if the most recent sucsessfull flowmeter port detection was recorded in the settings file and then checked first
	const size_t NewSize = 100;
	wchar_t MyWideString[NewSize];
	swprintf(MyWideString,NewSize, L"\\\\.\\COM%d",GivenPort); // note that this is the wide char equivelent of sprintf, the capital L tells the program that input string should be wide char
	aComFile = CreateFile(					   	MyWideString,  	            //LPCTSTR lpFileName,
												GENERIC_READ | GENERIC_WRITE, //DWORD dwDesiredAccess ,
												0,	                  //DWORD dwShareMode (0 for COM port access),
												SECURITY_ANONYMOUS,	  //LPSECURITY_ATTRIBUTES lpSecurityAttributes,
												OPEN_EXISTING,	      //DWORD dwCreationDisposition (necessary to be OPEN_EXISTING for COM ports),
//												FILE_FLAG_OVERLAPPED,  //DWORD dwFlagsAndAttributes,
												FILE_ATTRIBUTE_NORMAL,//DWORD dwFlagsAndAttributes,
												0);	                  //HANDLE hTemplateFile
	if ((aComFile==INVALID_HANDLE_VALUE)||(aComFile==NULL)){
		CloseHandle(aComFile);
		return false;
	}else{
		CloseHandle(aComFile);
		return true;
	}
}
bool FlowmeterManager::AttemptTsiSetup(int GivenPort){
	sprintf(tmp,"Searching for TSI flowmeter on port %d",GivenPort);
	FlowmeterStatus->Items->Add(tmp);
	TsiPointer = new TsiFlowmeter(GivenPort);
	if (TsiPointer->CheckPresence()) {
		FlowmeterReady = true;
		FlowmeterType = Tsi;
		SettingsPointer->LatestGoodFlowmeterPort = GivenPort;
		SettingsPointer->WriteFile();
		sprintf(tmp,"TSI flowmeter found on port %d",GivenPort);
		FlowmeterStatus->Items->Add(tmp);
		return true;
	}else{
		delete TsiPointer;
		return false;
	}
}
bool FlowmeterManager::AttemptAlicatSetup(int GivenPort){// this function is evil because it starts a class and does not close it
	sprintf(tmp,"Searching for Alicat flowmeter on port %d",GivenPort);
	FlowmeterStatus->Items->Add(tmp);
	char Name[10];
	sprintf(Name,"COM%d",GivenPort);
	AlicatPointer = new AlicatFlowmeter(True);
	AlicatPointer->Setup(Name,38400,'N',8,1);             //COM port, baud rate, parity, bytes per bit, stop bits
	AlicatPointer->Open();                                   //Enable port access
	AlicatPointer->Resume();
	Sleep(100); // this alows the flowmeter class to get some data so it can be used safely
	if (AlicatPointer->MassFlow()!= -2){                                // this never seems to be null
		FlowmeterReady = true;
		FlowmeterType = Alicat;
		SettingsPointer->LatestGoodFlowmeterPort = GivenPort;
		SettingsPointer->WriteFile();
		sprintf(tmp,"Alicat flowmeter found on found on port %d",GivenPort);
		FlowmeterStatus->Items->Add(tmp);
		return true;
	}else{
		AlicatPointer->Close();
		return false;
	}
}

bool FlowmeterManager::IsThereNewData(){
	if (Alicat == FlowmeterType)
		return true;// this should probably be fixed later, the alicat class lacks the desired functionality
	if (Tsi == FlowmeterType)
		return TsiPointer->IsThereNewData();
	return false; // if there is no flowmeter there is no new data
}
float FlowmeterManager::MassFlow(){ // should be SCCM
	if (Alicat == FlowmeterType)
		return AlicatPointer->MassFlow();
	if (Tsi == FlowmeterType)
		return TsiPointer->MassFlow();
}
float FlowmeterManager::Temperature(){ // should be celcius
	if (Alicat == FlowmeterType)
		return AlicatPointer->Temperature();
	if (Tsi == FlowmeterType)
		return TsiPointer->Temperature();
}
float FlowmeterManager::Pressure(){ // should be bar
	if (Alicat == FlowmeterType)
		return AlicatPointer->Pressure();
	if (Tsi == FlowmeterType)
		return TsiPointer->Pressure();
}
FlowmeterManager::~FlowmeterManager(){
	delete AlicatPointer;
	delete TsiPointer;
}

