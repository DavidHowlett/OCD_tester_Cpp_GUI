#include <iostream.h>
#include "SettingsFileManager.h"
#include "TsiFlowmeter.h"
//#include "AlicatFlowmeter.h"
#include "AlicatFlowmeterV2.h"
#include "OCD_FlowMeterManager.h"

//---------------------------------------------------------------------------
FlowMeterManager::FlowMeterManager(SettingsFileManager* GivenSettingsPointer,TListBox* GivenTEditPointer){
	SettingsPointer=GivenSettingsPointer;
	FlowmeterStatus=GivenTEditPointer;
	PortNum = 0; // a port number of 0 is a special value
	FlowmeterType = None;
}
void FlowMeterManager::CallMeRegularly(){
	if(FlowmeterType!=None)
		FlowMeterPointer->CallMeRegularly();
	if(FlowmeterType==None||FlowMeterPointer->DataAge()>1)// if no flowmeter is ready and the data is older then a second
		Setup();
}
void FlowMeterManager::Setup(){ // this method should finish quickly to allow processing of the message queue.
	// the flowmeter port does not change often so first try the one that worked last time
	// and then systematicly try them all
	FlowmeterType=None;
	int TmpPortNum;
	if(PortNum==0)// COM0 does not exist on windows
		TmpPortNum = SettingsPointer->LatestGoodFlowmeterPort;
	else
		TmpPortNum = PortNum;
	if(TestPortExistence(TmpPortNum)){
		if (AttemptTsiSetup(TmpPortNum))
			return;
		if (AttemptAlicatSetup(TmpPortNum))
			return;
	}
	PortNum++;
	if(PortNum>MAX_PORT_NUMBER){// if no flowmeter is found then tell the user and restart the search
		PortNum=1;
		FlowmeterStatus->Items->Add("Warning: no flowmeter found");
	}
}
bool FlowMeterManager::TestPortExistence(int GivenPort){// it would save the time of the user if the most recent sucsessfull flowmeter port detection was recorded in the settings file and then checked first
	const size_t NewSize = 100;
	wchar_t MyWideString[NewSize];
	swprintf(MyWideString,NewSize, L"\\\\.\\COM%d",GivenPort); // note that this is the wide char equivelent of sprintf, the capital L tells the program that input string should be wide char
	aComFile = CreateFile(					   	MyWideString,  	            //LPCTSTR lpFileName,
												GENERIC_READ | GENERIC_WRITE, //DWORD dwDesiredAccess ,
												0,	                  //DWORD dwShareMode (0 for COM port access),
												SECURITY_ANONYMOUS,	  //LPSECURITY_ATTRIBUTES lpSecurityAttributes,
												OPEN_EXISTING,	      //DWORD dwCreationDisposition (necessary to be OPEN_EXISTING for COM ports),
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
bool FlowMeterManager::AttemptTsiSetup(int GivenPort){
	sprintf(tmp,"Searching for TSI flowmeter on port %d",GivenPort);
	FlowmeterStatus->Items->Add(tmp);
	FlowMeterPointer = new TsiFlowmeter(GivenPort);
	if (FlowMeterPointer->CheckPresence()) {
		FlowmeterType = Tsi;
		SettingsPointer->LatestGoodFlowmeterPort = GivenPort;
		SettingsPointer->WriteFile();
		sprintf(tmp,"TSI flowmeter found on port %d",GivenPort);
		FlowmeterStatus->Items->Add(tmp);
		return true;
	}else{
		delete FlowMeterPointer;
		return false;
	}
}
bool FlowMeterManager::AttemptAlicatSetup(int GivenPort){
	sprintf(tmp,"Searching for Alicat flowmeter on port %d",GivenPort);
	FlowmeterStatus->Items->Add(tmp);
	FlowMeterPointer = new AlicatFlowmeterV2(GivenPort);
	if (FlowMeterPointer->CheckPresence()) {
		FlowmeterType = Alicat;
		SettingsPointer->LatestGoodFlowmeterPort = GivenPort;
		SettingsPointer->WriteFile();
		sprintf(tmp,"Alicat flowmeter found on port %d",GivenPort);
		FlowmeterStatus->Items->Add(tmp);
		return true;
	}else{
		delete FlowMeterPointer;
		return false;
	}
}

bool FlowMeterManager::IsThereNewData(){
	if (FlowmeterType!=None)
		return FlowMeterPointer->IsThereNewData();
	return false; // if there is no flowmeter there is no new data
}
float FlowMeterManager::MassFlow(){ // should be SCCM
	if (FlowmeterType!=None)
		return FlowMeterPointer->MassFlow();
	return -1;
}
float FlowMeterManager::Temperature(){ // should be celcius
	if (FlowmeterType!=None)
		return FlowMeterPointer->Temperature();
	return -1;
}
float FlowMeterManager::Pressure(){ // should be bar
	if (FlowmeterType!=None)
		return FlowMeterPointer->Pressure();
	return -1;
}
FlowMeterManager::~FlowMeterManager(){
	delete FlowMeterPointer;
}

