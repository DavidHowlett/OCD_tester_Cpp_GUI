#include "tsi_flowmeter.h"
#include "alicat_flowmeter.h"
#include "flowmeter_manager.h"
#include <stdio.h>
#include <cwchar.h>
#include <iostream.h>
HANDLE aComFile;
//---------------------------------------------------------------------------
flowmeter_manager::flowmeter_manager(){
	flowmeter_ready = false;
	flowmeter_type = none;
}
int flowmeter_manager::setup(settings_file_manager * given_settings_pointer){ // this function returns 0 when it sucsessfuly sets a flowmeter up
	settings_pointer=given_settings_pointer;
	int portnum;
// the flowmeter port does not change often so first try the one that worked last time
	portnum = settings_pointer->most_recent_flowmeter_port;
	if (test_port_existence(portnum)){
		if(attempt_tsi_setup(portnum))
			return 0;
		if(attempt_alicat_setup(portnum))
			return 0;

	}
// now systematically try all the ports
	cout << "Searching ports for TSI flowmeter" << endl;
	for (portnum = 0;(portnum < MAX_PORT_NUMBER) && (!flowmeter_ready); portnum++) {
		if(test_port_existence(portnum)){
			port_existence[portnum] =true;
			cout << portnum << endl;
			if(attempt_tsi_setup(portnum))
				return 0;
		}else{
			port_existence[portnum] =false;
		}
	}
	cout << "Searching ports for Alicat flowmeter" << endl;
	for (portnum = 0;(portnum < MAX_PORT_NUMBER) && (!flowmeter_ready); portnum++) { // I seperate the alicat setup attempts from the tsi setup attempts because windows takes a little bit ro relace a COM port
		if(port_existence[portnum]){
			cout << portnum << endl;
			if(attempt_alicat_setup(portnum))
				return 0;
		}
	}
	if(!flowmeter_ready)
		cout << "Warning: no flowmeter found" << endl;
	return 1;
}
bool flowmeter_manager::test_port_existence(int port){// it would save the time of the user if the most recent sucsessfull flowmeter port detection was recorded in the settings file and then checked first
	const size_t newsize = 100;
	wchar_t my_wide_string[newsize];
	swprintf(my_wide_string,newsize, L"\\\\.\\COM%d",port); // note that this is the wide char equivelent of sprintf, the capital L tells the program that input string should be wide char
	aComFile = CreateFile(					   	my_wide_string,  	            //LPCTSTR lpFileName,
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
bool flowmeter_manager::attempt_tsi_setup(int port){
	tsi_pointer = new tsi_flowmeter(port);
	if (tsi_pointer->check_presence()) {
		flowmeter_ready = true;
		flowmeter_type = tsi;
		settings_pointer->most_recent_flowmeter_port = port;
		settings_pointer->write_file();
		cout << "TSI flowmeter found on port " << port << endl;
		return true;
	}else{
		//cout << "TSI flowmeter not found on port " << port << endl; // remove me later
		delete tsi_pointer;
		return false;
	}
}
bool flowmeter_manager::attempt_alicat_setup(int port){// this function is evil because it starts a class and does not close it
	char name[10];
	sprintf(name,"COM%d",port);
	alicat_pointer = new alicat_flowmeter(True);
	alicat_pointer->Setup(name,38400,'N',8,1);             //COM port, baud rate, parity, bytes per bit, stop bits
	alicat_pointer->Open();                                   //Enable port access
	alicat_pointer->Resume();
	Sleep(100); // this alows the flowmeter class to get some data so it can be used safely
	if (alicat_pointer->MassFlow()!= -2){                                // this never seems to be null
		flowmeter_ready = true;
		flowmeter_type = alicat;
		settings_pointer->most_recent_flowmeter_port = port;
		settings_pointer->write_file();
		cout << "Alicat flowmeter found on port " << port << endl;
		return true;
	}else{
		//cout << "Alicat flowmeter not found on port " << port << endl; // remove me later
		alicat_pointer->Close();
		//delete alicat_pointer;  // this causes a crash so it is not used.
		return false;
	}
}
float flowmeter_manager::mass_flow(){
	if (alicat == flowmeter_type) {
		return(alicat_pointer->MassFlow());
	}
	if (tsi == flowmeter_type) {
		return(tsi_pointer->mass_flow());
	}
	cout << "Flowmeter should be setup before readings are taken" << endl;
	return 0;
}
flowmeter_manager::~flowmeter_manager(){
	delete alicat_pointer;
	delete tsi_pointer;
}

