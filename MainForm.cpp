// Environment Resources ----------------------------------------------------
#include <vcl>
#include <stdio>
#include <iostream>
#include <windows>
#include <cassert>
#include <new>
#include <time.h> // this is not used to give the time but it is used to process the time provided by the API
#pragma hdrstop
#pragma package(smart_init)
#pragma resource "*.dfm"
// My Header Files ----------------------------------------------------------
#include "MainForm.h"
#include "SettingsFileManager.h"
#include "FlowmeterManager.h"
// My Classes ---------------------------------------------------------------
TForm1 *Form1;
SettingsFileManager * Settings;
FlowmeterManager * GenericFlowmeter;
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner) : TForm(Owner)
{
	Settings = new SettingsFileManager;
	Settings->ReadFile();
	GenericFlowmeter = new FlowmeterManager(Settings,Log);//I want this class to always have acsess to these pointers
	GenericFlowmeter->Setup();
	PulsesToRecord->Text = "3";
	ZeroRawDataArrays();
	ZeroProcessedDataArrays();
	GroupIsRecording = false;
	QueryPerformanceFrequency(&Frequency);  // get ticks per second
	ReadingsInRawDataArray = 0;
}
void __fastcall TForm1::FastTimerTimer(TObject *Sender)
{
	GenericFlowmeter->CallMeRegularly();
	if (GenericFlowmeter->IsThereNewData()) {// there are some things that should only be done when there is new data
		char tmpString[100];
		sprintf(tmpString,"%f",GenericFlowmeter->LastMassFlow);
		MassFlowMonitor->Text = tmpString;
		sprintf(tmpString,"%f",GenericFlowmeter->LastTemperature);
		TemperatureMonitor->Text = tmpString;
		sprintf(tmpString,"%f",GenericFlowmeter->LastPressure);
		PressureMonitor->Text = tmpString;
		if(GroupIsRecording){
			GetDataPoint();
			ProcessRecentData();
		}
	}
}
void __fastcall TForm1::OutputDataClick(TObject *Sender)
{
	SaveProcessedData();
}
void __fastcall TForm1::StartClick(TObject *Sender)
{
	if (!GenericFlowmeter->FlowmeterReady) {
		Log->Items->Add("You can't do this operation because the flowmeter is not setup yet");
		return;
	}
	if (TargetPulsesInGroup > MaxPulsesInAGroup){
		char tmp[100];
		sprintf(tmp,"You can't have more then %d pulses in a group",MaxPulsesInAGroup);
		Log->Items->Add(tmp);
		return;
	}
	if(GroupIsRecording){
		//Log->Items->Add("Recording already in progress");
		FinishGroup();
		return;
	}
	StartGroup();
	//idea: add code here to stop the recording if the button is pressed again.
}
void TForm1::ZeroRawDataArrays(){
	for (int i = 0; i < RawDataArraySize; i++) {
		TimeOfReading[i]=0;
		FlowReading[i]=0;
	}
}
void TForm1::ZeroProcessedDataArrays(){
	for(int i = 0; i < MaxGroupsBeforeOutputtingAnalysisFile; i++) {
		AveragePeakFlow[i] = 0;
		AverageCycleTime[i] = 0;
		AveragePulseOnTime[i] = 0;
		AveragePulseVolume[i] = 0;
		PulsesInGroup[i] = 0;
		TooManyPulsesInGroup	 [i] = false;
		for (int j = 0; j < MaxPulsesInAGroup; j++) {
			PulsePeakFlow[j][i]=0;
			PulseCycleTime[j][i]=0;
			PulseOnTime[j][i]=0;
			PulseVolume[j][i]=0;
		}
	}
}
void TForm1::StartGroup(){
	ZeroRawDataArrays();
	TargetPulsesInGroup=StrToInt(PulsesToRecord->Text);
	QueryPerformanceCounter(&TicksAtStartOfReading);     // record the time that the reading started
	PulsesInGroup[GroupsStoredInArrays] = 0;
	MostRecentUp = 0;
	GroupIsRecording = true;
	Log->Items->Add("Pulse recording started");
}
void TForm1::GetDataPoint(){
	LARGE_INTEGER CurrentTicks;
	QueryPerformanceCounter(&CurrentTicks);
	if(ReadingsInRawDataArray>=RawDataArraySize){
		Log->Items->Add("The reading was aborted early because the raw data array was full. Contact a programmer.");
		FinishGroup();
		return;
	}
	TimeOfReading[ReadingsInRawDataArray] = double(CurrentTicks.QuadPart - TicksAtStartOfReading.QuadPart)/double(Frequency.QuadPart); // this calculates the time in seconds from the initiation of this function
	FlowReading[ReadingsInRawDataArray] = GenericFlowmeter->LastMassFlow;
	ReadingsInRawDataArray++;
	char tmp[100];
	itoa(PulsesInGroup[GroupsStoredInArrays],tmp,10);
	PulsesRecorded->Text=tmp;

	if (PulsesInGroup[GroupsStoredInArrays] >= TargetPulsesInGroup){
		assert(PulsesInGroup[GroupsStoredInArrays] == TargetPulsesInGroup);
		FinishGroup();
	}
	return;
}
void  TForm1::ProcessRecentData(){ // I need to find the pulse duration, the cycle time and the volume in this function
	//execute on pulse start
	if (ReadingsInRawDataArray>1&&FlowReading[ReadingsInRawDataArray-1] >= Settings->TriggerFlow && FlowReading[ReadingsInRawDataArray-2] < Settings->TriggerFlow){
		//finish processing for previous pulse
		if(MostRecentUp>0.1){// this stops the first up triggering the compleation of a pulse
			PulseVolume[PulsesInGroup[GroupsStoredInArrays]] [GroupsStoredInArrays]=IntegratedVolume;
			PulseCycleTime [PulsesInGroup[GroupsStoredInArrays]] [GroupsStoredInArrays] =  TimeOfReading[ReadingsInRawDataArray] - TimeOfReading[MostRecentUp];
			PulsePeakFlow [PulsesInGroup[GroupsStoredInArrays]] [GroupsStoredInArrays] = CurrentBiggestFlow;
			PulsesInGroup[GroupsStoredInArrays]++;
			UpdateAverages();
			PutProcessedDataOnScreen();
		}
		//processing for new pulse
		PulseHistory->Items->Add("Pulse Started");// remove me later
		IntegratedVolume = 0;
		CurrentBiggestFlow = -1000;
		MostRecentUp = ReadingsInRawDataArray;// this is setting the position in the array of the most recent pulse start equal to the current position in the array
	}
	//execute on pulse end
	if (ReadingsInRawDataArray>1&&FlowReading[ReadingsInRawDataArray-2] >= Settings->TriggerFlow && FlowReading[ReadingsInRawDataArray-1] < Settings->TriggerFlow){
		PulseHistory->Items->Add("Pulse Ended");// remove me later
		PulseOnTime [PulsesInGroup[GroupsStoredInArrays]] [GroupsStoredInArrays] = TimeOfReading[ReadingsInRawDataArray] - TimeOfReading[MostRecentUp];
		PulsePeakFlow [PulsesInGroup[GroupsStoredInArrays]] [GroupsStoredInArrays] = CurrentBiggestFlow;
	}
	if (FlowReading[ReadingsInRawDataArray-1] > CurrentBiggestFlow){
		CurrentBiggestFlow = FlowReading[ReadingsInRawDataArray-1];
	}
	// The below logic does the integration.
	// The "1000/60" converts from litres per minute to millilitres per second
	// The "(TimeOfReading[i+1]-TimeOfReading[i-1])/2" gives the best estimate that I know of for the length of time associated with a reading
	// For easy understanding I want to write the algebraically correct expression:
	// IntegratedVolume = IntegratedVolume + ((TimeOfReading[i+1]-TimeOfReading[i-1])/2)*FlowReading[j]*1000/60;
	// To make the above line actually function in code and provide and accurate answer I rearrange it to be the algebraically equivalent:
	// IntegratedVolume = IntegratedVolume + (TimeOfReading[i+1]-TimeOfReading[i-1])*FlowReading[j]*1000/60/2;
	// and then:
	IntegratedVolume = IntegratedVolume + (TimeOfReading[ReadingsInRawDataArray-1]-TimeOfReading[ReadingsInRawDataArray-1-2])*FlowReading[ReadingsInRawDataArray-1-1]*(double)25/(double)3;

	// idea: run a running total of pulse volume
//-----------------------------------------------------------
/*
	double IntegratedVolume = 0; // this variable records the volume that passed through the OCD in the part of the pulse processed so far
	float CurrentBiggestFlow = -1000;
	int MostRecentUp = -1000;
	bool Recording = false;
	for (int i=2;(i<(RawDataArraySize-1)) && TimeOfReading[i] > 0;i++){// the unusual fence post conditions are to stop the integration line reading outside the area that it should
		assert(TimeOfReading[i] > 0);
		if (FlowReading[i] > CurrentBiggestFlow){
			CurrentBiggestFlow = FlowReading[i];
		}
		// The below logic does the integration.
		// The "1000/60" converts from litres per minute to millilitres per second
		// The "(TimeOfReading[i+1]-TimeOfReading[i-1])/2" gives the best estimate that I know of for the length of time associated with a reading
		// For easy understanding I want to write the algebraically correct expression:
		// IntegratedVolume = IntegratedVolume + ((TimeOfReading[i+1]-TimeOfReading[i-1])/2)*FlowReading[j]*1000/60;
		// To make the above line actually function in code and provide and accurate answer I rearrange it to be the algebraically equivalent:
		// IntegratedVolume = IntegratedVolume + (TimeOfReading[i+1]-TimeOfReading[i-1])*FlowReading[j]*1000/60/2;
		// and then:
		IntegratedVolume = IntegratedVolume + (TimeOfReading[i]-TimeOfReading[i-2])*FlowReading[i-1]*(double)25/(double)3;
		if (FlowReading[i] >= Settings->TriggerFlow && FlowReading[i-1] < Settings->TriggerFlow){
			if (Recording){
				PulseVolume[RecordedPulses] [ReadingsStoredInArrays]=IntegratedVolume;
				PulseCycleTime [RecordedPulses] [ReadingsStoredInArrays] =  TimeOfReading[i] - TimeOfReading[MostRecentUp];
				PulsePeakFlow [RecordedPulses] [ReadingsStoredInArrays] = CurrentBiggestFlow;
				CurrentBiggestFlow = -1000;
				RecordedPulses++;
			}
			IntegratedVolume = 0;
			MostRecentUp = i;
			Recording = true;
		}
		if ((FlowReading[i] <= Settings->TriggerFlow) && (FlowReading[i-1] > Settings->TriggerFlow) && Recording){
			PulseOnTime [RecordedPulses] [ReadingsStoredInArrays] = TimeOfReading[i] - TimeOfReading[MostRecentUp];
		}
	}
	PulsesInReading [ReadingsStoredInArrays] = RecordedPulses;
	return(RecordedPulses);
	*/
}
void TForm1::UpdateAverages(){// averages all the pulses in the current reading
	AveragePulseVolume[GroupsStoredInArrays]=0;
	AveragePulseOnTime[GroupsStoredInArrays]=0;
	AverageCycleTime[GroupsStoredInArrays]=0;
	AveragePeakFlow[GroupsStoredInArrays]=0;
	for(int i=0 ; i<PulsesInGroup[GroupsStoredInArrays];i++){
		AveragePulseVolume[GroupsStoredInArrays]=AveragePulseVolume[GroupsStoredInArrays]+PulseVolume [i][GroupsStoredInArrays];
		AveragePulseOnTime[GroupsStoredInArrays]=AveragePulseOnTime[GroupsStoredInArrays]+PulseOnTime [i][GroupsStoredInArrays];
		AverageCycleTime[GroupsStoredInArrays]=AverageCycleTime[GroupsStoredInArrays]+PulseCycleTime [i][GroupsStoredInArrays];
		AveragePeakFlow[GroupsStoredInArrays]=AveragePeakFlow[GroupsStoredInArrays]+PulsePeakFlow [i][GroupsStoredInArrays];
	}
	if(PulsesInGroup[GroupsStoredInArrays] != 0){
		AveragePulseVolume[GroupsStoredInArrays]=AveragePulseVolume[GroupsStoredInArrays]/PulsesInGroup[GroupsStoredInArrays];
		AveragePulseOnTime[GroupsStoredInArrays]=AveragePulseOnTime[GroupsStoredInArrays]/PulsesInGroup[GroupsStoredInArrays];
		AverageCycleTime[GroupsStoredInArrays]=AverageCycleTime[GroupsStoredInArrays]/PulsesInGroup[GroupsStoredInArrays];
		AveragePeakFlow[GroupsStoredInArrays]=AveragePeakFlow[GroupsStoredInArrays]/PulsesInGroup[GroupsStoredInArrays];
	}
}
void TForm1::PutProcessedDataOnScreen(){
	char tmp[100];
	// put code here to update PulseHistory
	sprintf(tmp,"%f",AveragePulseVolume[GroupsStoredInArrays]);
	AvgVolume		->Text=tmp;
	sprintf(tmp,"%f",AveragePulseOnTime[GroupsStoredInArrays]);
	AvgOnTime		->Text=tmp;
	sprintf(tmp,"%f",(AverageCycleTime[GroupsStoredInArrays]-AveragePulseOnTime[GroupsStoredInArrays]));
	AvgOffTime 	->Text=tmp;
	sprintf(tmp,"%f",AverageCycleTime[GroupsStoredInArrays]);
	AvgCycleTime->Text=tmp;
}
void TForm1::FinishGroup(){
	GroupsStoredInArrays++;
	GroupIsRecording = false;
	Log->Items->Add("Pulse recording ended");
	SaveRawData();
	if(GroupsStoredInArrays>=MaxGroupsBeforeOutputtingAnalysisFile)// this is included to handle the overflow caused by the user taking too many readings before telling the system to output an analyis file
		SaveProcessedData();
}
void TForm1::SaveRawData(){ // this function needs to know the current reading number and it needs to know the most recently written raw Settings file number
	char FileName[50];
	sprintf(FileName,"raw data %d.txt",Settings->LastRawDataFileNumber+1);
	Settings->WriteFile(); // this record the change in the most_recently_used_raw_data_file_number
	RawDataFileAssosiatedWithGroup [GroupsStoredInArrays-1]=Settings->LastRawDataFileNumber+1;
	FILE * pFile;
	pFile = fopen (FileName,"w+");
	if( pFile==NULL){
		Log->Items->Add("Error: could not access the raw data file");
	}else{
		time_t rawtime;
		struct tm * timeinfo;
		char buffer [80];
		time ( &rawtime );
		timeinfo = localtime ( &rawtime );
		fprintf(pFile,"The local date and time when the below data was recorded to file:\n");
		strftime (buffer,80,"Date: %Y-%m-%d Time: %H:%M:%S",timeinfo);
		fprintf(pFile,"%s\n",buffer);
		fprintf(pFile,"The left column shows the number of seconds since data logging began and the right column shows the mass flow reading at that time\n\n");
		for (int i = 0; i < RawDataArraySize; i++) {
			if(i>5 && TimeOfReading[i]<0.01) // the array is intialised to contain only zeros exept in the region in which there is data. it is expected that once this loop is 5 readings into the data the time will no longer be 0
				break;                                // this means that the text file is truncated to the size of the actual data
			fprintf(pFile,"%f\t%f\n",TimeOfReading[i],FlowReading[i]);
		}
		char tmp[100];
		sprintf(tmp,"Raw data file %d created\n",Settings->LastRawDataFileNumber+1);
		Log->Items->Add(tmp);
		Settings->LastRawDataFileNumber++;
		Settings->WriteFile();
	}
	fclose (pFile);
}
void TForm1::SaveProcessedData(){
	FILE * pFile;
	char FileName[50];
	sprintf(FileName,"processed data %d.txt",(Settings->LastProcessedDataFileNumber+1)); // the idea is that this causes the analysed data file numbers to be numbered sequentialy
	pFile = fopen (FileName,"w");
	if(NULL==pFile){
		Log->Items->Add("Could not open processed data file\n");
	}else{
		time_t rawtime;
		struct tm * timeinfo;
		char buffer [80];
		time ( &rawtime );
		timeinfo = localtime ( &rawtime );
		fprintf(pFile,"The local date and time when the below data was recorded to file:\n");
		strftime (buffer,80,"Date: %Y-%m-%d Time: %H:%M:%S",timeinfo);
		fprintf(pFile,"%s\n\n",buffer);

		if(GroupsStoredInArrays>0){
			for (int i = 0; i < GroupsStoredInArrays; i++) {
				fprintf(pFile,"Below is reading %d assosiated with raw data file %d\n",(i+1),RawDataFileAssosiatedWithGroup [i]);
				if(PulsesInGroup[i]>0){
					fprintf(pFile,"The pulse peak flow, duration, cycle time and volume are:\n");
					for(int j=0; j < PulsesInGroup[i] ; j++){
						fprintf(pFile,"%f\t%f\t%f\t%f\n",PulsePeakFlow[j][i],PulseOnTime[j][i],PulseCycleTime[j][i],PulseVolume[j][i]);
					}
					fprintf(pFile,"The average pulse peak flow, duration, cycle time and volume are:\n");
					fprintf(pFile,"%f\t%f\t%f\t%f\n",AveragePeakFlow[i],AveragePulseOnTime[i],AverageCycleTime[i],AveragePulseVolume[i]);
					if(TooManyPulsesInGroup	[i]){
						fprintf(pFile,"There were too many pulses in this reading meaning that\nsome of them were not recorded properly and are are not displayed above\n");
					}
				}else{
					fprintf(pFile,"There are no pulses available for analysis in this reading\n");
				}
				fprintf(pFile,"\n"); // this aids readablitiy for humans by visualy seperateing each reading
			}
		}else{
			fprintf(pFile,"There were no readings\n");
		}
		Settings->LastProcessedDataFileNumber++;
		Settings->WriteFile(); // this records that the most_recently_used_analysed_data_file_number has increaced in value
		GroupsStoredInArrays = 0;
		char tmp[100];
		sprintf(tmp,"Processed data file %d created\n",Settings->LastProcessedDataFileNumber);
		Log->Items->Add(tmp);
	}
	fclose (pFile);
	ZeroProcessedDataArrays();
}

