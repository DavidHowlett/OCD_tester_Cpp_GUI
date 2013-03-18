#ifndef OCD_FlowMeterManagerH
#define OCD_FlowMeterManagerH
class SettingsFileManager;
class AbstractFlowMeter;
//class AlicatFlowmeterV2; // I chose to forward declare this rather then include the whole header file
//class TsiFlowmeter;    // I chose to forward declare this rather then include the whole header file
//---------------------------------------------------------------------------
enum FlowmeterTypeEnum {None, Alicat, Tsi};
class FlowMeterManager{
	public:
		FlowMeterManager(SettingsFileManager*, TListBox*);
		void CallMeRegularly();
		bool IsThereNewData();
		float MassFlow(); // should be SCCM
		float Temperature(); // should be celcius
		float Pressure(); // should be bar
		~FlowMeterManager();
	public:
		bool FlowMeterIsReady(); // this should be true if the flowmeter has been setup and the caller of this class can safely get data from it
		FlowmeterTypeEnum FlowmeterType; // this will be None until a flowmeter is setup
	private:
		void Setup(); // this method should be repetitively called until there is a valid flowmeter setup
		bool TestPortExistence(int Port); // this checks if a port is a valid com port
		bool AttemptTsiSetup(int Port);
		bool AttemptAlicatSetup(int Port);

		#define MAX_PORT_NUMBER 200 // this is the largest port number the program can cope with
		//bool PortExistence[MAX_PORT_NUMBER];
		AbstractFlowMeter* FlowMeterPointer;
		SettingsFileManager* SettingsPointer;
		TListBox* FlowmeterStatus;
		HANDLE aComFile;
		char tmp[100];
		int PortNum;
};
#endif

