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
	SetupProgressFlag = 0;
	FlowmeterType = None;
}
FlowmeterManager::CallMeRegularly(){
	// idea: insert code here to check if the flowmeter is working properly and if not then set FlowmeterReady = false;
	if(!FlowmeterReady)			Setup();
	if(Tsi == FlowmeterType)	TsiPointer->CallMeRegularly();
	return 0;
}
void FlowmeterManager::Setup(){ // this function returns 0 when it sucsessfuly sets a flowmeter up
	if(0==SetupProgressFlag++){
	// the flowmeter port does not change often so first try the one that worked last time
		PortNum = SettingsPointer->LatestGoodFlowmeterPort;
		if (TestPortExistence(PortNum)){
			if(AttemptTsiSetup(PortNum))
				return;
			AttemptAlicatSetup(PortNum);
		}
		return;
	}
	if(1==SetupProgressFlag++){
	// now systematically try all the ports
		for (PortNum = 0;(PortNum < MAX_PORT_NUMBER) && (!FlowmeterReady); PortNum++) {
			if(TestPortExistence(PortNum)){
				PortExistence[PortNum] =true;
				if(AttemptTsiSetup(PortNum))
					return ;
			}else{
				PortExistence[PortNum] =false;
			}
		}
		return;
	}
	if(2==SetupProgressFlag++){
		for (PortNum = 0;(PortNum < MAX_PORT_NUMBER) && (!FlowmeterReady); PortNum++) { // I seperate the alicat setup attempts from the tsi setup attempts because windows takes a little bit ro relace a COM port
			if(PortExistence[PortNum]){

				if(AttemptAlicatSetup(PortNum))
					return ;
			}
		}
		return;
	}
	if(3==SetupProgressFlag){
		SetupProgressFlag = 0;
		if(!FlowmeterReady)
			FlowmeterStatus->Items->Add("Warning: no flowmeter found");
		return;
	}
	return;
}
bool FlowmeterManager::TestPortExistence(int Port){// it would save the time of the user if the most recent sucsessfull flowmeter port detection was recorded in the settings file and then checked first
	const size_t NewSize = 100;
	wchar_t MyWideString[NewSize];
	swprintf(MyWideString,NewSize, L"\\\\.\\COM%d",Port); // note that this is the wide char equivelent of sprintf, the capital L tells the program that input string should be wide char
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
bool FlowmeterManager::AttemptTsiSetup(int Port){
	sprintf(tmp,"Searching for TSI flowmeter on port %d",PortNum);
	FlowmeterStatus->Items->Add(tmp);
	TsiPointer = new TsiFlowmeter(Port);
	if (TsiPointer->CheckPresence()) {
		FlowmeterReady = true;
		FlowmeterType = Tsi;
		SettingsPointer->LatestGoodFlowmeterPort = Port;
		SettingsPointer->WriteFile();
		sprintf(tmp,"TSI flowmeter found on port %d",Port);
		FlowmeterStatus->Items->Add(tmp);
		return true;
	}else{
		delete TsiPointer;
		return false;
	}
}
bool FlowmeterManager::AttemptAlicatSetup(int Port){// this function is evil because it starts a class and does not close it
	sprintf(tmp,"Searching for Alicat flowmeter on port %d",PortNum);
	FlowmeterStatus->Items->Add(tmp);
	char Name[10];
	sprintf(Name,"COM%d",Port);
	AlicatPointer = new AlicatFlowmeter(True);
	AlicatPointer->Setup(Name,38400,'N',8,1);             //COM port, baud rate, parity, bytes per bit, stop bits
	AlicatPointer->Open();                                   //Enable port access
	AlicatPointer->Resume();
	Sleep(100); // this alows the flowmeter class to get some data so it can be used safely
	if (AlicatPointer->MassFlow()!= -2){                                // this never seems to be null
		FlowmeterReady = true;
		FlowmeterType = Alicat;
		SettingsPointer->LatestGoodFlowmeterPort = Port;
		SettingsPointer->WriteFile();
		sprintf(tmp,"Alicat flowmeter found on found on port %d",Port);
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

