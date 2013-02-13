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
TListBox * MainLog;
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
	MainLog = Log;
	ZeroRawDataArrays();
	ZeroProcessedDataArrays();
	ReadingsStoredInArrays=0;
	RecordingInProgress = false;
	QueryPerformanceFrequency(&Frequency);  // get ticks per second
	ReadingsInRawDataArray = 0;
}
//---------------------------------------------------------------------------
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
		if(RecordingInProgress){
			GetDataPoint();
			FindPulseAverages(ReadingsStoredInArrays);
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
		MainLog->Items->Add("You can't do this operation because the flowmeter is not setup yet");
		return;
	}
	if (NumberOfPulsesToRecord > MaxPulsesInAReading){
		char tmp[100];
		sprintf(tmp,"You can't have more then %d pulses in a reading",MaxPulsesInAReading);
		MainLog->Items->Add(tmp);
		return;
	}
	if(RecordingInProgress){
		MainLog->Items->Add("Recording already in progress");
		return;
	}
	if(ReadingsStoredInArrays>=MaxReadingsBeforeOutputtingAnalysisFile)// this is included to handle the overflow caused by the user taking too many readings before telling the system to output an analyis file
		SaveProcessedData();
	StartReading();
	//idea: add code here to stop the recording if the button is pressed again.
	//GetData(NumberOfPulsesToRecord);
	//PutProcessedDataOnConsole();
	//SaveRawData();
}
void TForm1::ZeroRawDataArrays(){
	for (int i = 0; i < RawDataArraySize; i++) {
		TimeOfReading[i]=0;
		FlowReading[i]=0;
	}
}
void TForm1::ZeroProcessedDataArrays(){
	for(int i = 0; i < MaxReadingsBeforeOutputtingAnalysisFile; i++) {
		AveragePeakFlow[i] = 0;
		AverageCycleTime[i] = 0;
		AveragePulseOnTime[i] = 0;
		AveragePulseVolume[i] = 0;
		PulsesInReading[i] = 0;
		TooManyPulsesInReading	 [i] = false;
		for (int j = 0; j < MaxPulsesInAReading; j++) {
			PulsePeakFlow[j][i]=0;
			PulseCycleTime[j][i]=0;
			PulseOnTime[j][i]=0;
			PulseVolume[j][i]=0;
		}
	}
}
void TForm1::StartReading(){
	ZeroRawDataArrays();
	NumberOfPulsesToRecord=StrToInt(PulsesToRecord->Text);
	QueryPerformanceCounter(&TicksAtStartOfReading);     // record the time that the reading started
	RecordingInProgress = true;
}
void TForm1::GetDataPoint(){
	int RecordedPulses = 0;
	//while(-2==GenericFlowmeter->LastMassFlow){
	//	Sleep(1);// the flowmeter returns -2 when it does not have data, this code waits until the flowmeter has data, idealy this code should never need to run as the flowmeter should have data by this point in the program. If the flowmeter is connected backwards it will give negetive flow values but this will not cause  problems unless it consistantly returns a value of -2
	//}
	LARGE_INTEGER CurrentTicks;           // ticks
	QueryPerformanceCounter(&CurrentTicks);
	if(ReadingsInRawDataArray>=RawDataArraySize){
		MainLog->Items->Add("The reading was aborted early because the raw data array was full. Contact a programmer.");
		ProcessData();
		FinishReading();
		return;
	}
	TimeOfReading[ReadingsInRawDataArray] = double(CurrentTicks.QuadPart - TicksAtStartOfReading.QuadPart)/double(Frequency.QuadPart); // this calculates the time in seconds from the initiation of this function
	FlowReading[ReadingsInRawDataArray] = GenericFlowmeter->LastMassFlow;
	RecordedPulses = ProcessData();
	if (RecordedPulses >= NumberOfPulsesToRecord){
		assert(RecordedPulses == NumberOfPulsesToRecord);
		PulsesInReading[ReadingsStoredInArrays] = RecordedPulses;
		ReadingsStoredInArrays++;    // this records that a reading has happened
		FinishReading();
	}
	char tmp[100];
	itoa(RecordedPulses,tmp,10);
	PulsesRecorded->Text=tmp;
	return;
}
void TForm1::FinishReading(){
	RecordingInProgress = false;
}
int  TForm1::ProcessData(){ // I need to find the pulse duration, the cycle time and the volume in this function
	double IntegratedVolume = 0; // this variable records the volume that passed through the OCD in the part of the pulse processed so far
	float CurrentBiggestFlow = -1000;
	int MostRecentUp = -1000;
	int RecordedPulses = 0;
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
}
void TForm1::FindPulseAverages(int reading_of_intrest){// this take the avarage of the quantitiys derived though analysis over all the pulses in the reading passed to it
	AveragePeakFlow[reading_of_intrest]=0;
	AverageCycleTime[reading_of_intrest]=0;
	AveragePulseOnTime[reading_of_intrest]=0;
	AveragePulseVolume[reading_of_intrest]=0;
	for(int i=0 ; i<PulsesInReading[reading_of_intrest];i++){
		AveragePeakFlow[reading_of_intrest]=AveragePeakFlow[reading_of_intrest]+PulsePeakFlow [i][reading_of_intrest];
		AverageCycleTime[reading_of_intrest]=AverageCycleTime[reading_of_intrest]+PulseCycleTime [i][reading_of_intrest];
		AveragePulseOnTime[reading_of_intrest]=AveragePulseOnTime[reading_of_intrest]+PulseOnTime [i][reading_of_intrest];
		AveragePulseVolume[reading_of_intrest]=AveragePulseVolume[reading_of_intrest]+PulseVolume [i][reading_of_intrest];
	}
	if(PulsesInReading[reading_of_intrest] != 0){
		AveragePeakFlow[reading_of_intrest]=AveragePeakFlow[reading_of_intrest]/PulsesInReading[reading_of_intrest];
		AverageCycleTime[reading_of_intrest]=AverageCycleTime[reading_of_intrest]/PulsesInReading[reading_of_intrest];
		AveragePulseOnTime[reading_of_intrest]=AveragePulseOnTime[reading_of_intrest]/PulsesInReading[reading_of_intrest];
		AveragePulseVolume[reading_of_intrest]=AveragePulseVolume[reading_of_intrest]/PulsesInReading[reading_of_intrest];
	}
}
void TForm1::PutProcessedDataOnConsole(){
	/*
	if (0 != PulsesInReading[ReadingsStoredInArrays-1]){
		printf("The pulse peak flow, duration, cycle time and volume are:\n");
		for(int i = 0 ;i<PulsesInReading[ReadingsStoredInArrays-1];i++){
			printf("%f\t%f\t%f\t%f\n",PulsePeakFlow[i][ReadingsStoredInArrays-1],PulseOnTime[i][ReadingsStoredInArrays-1],PulseCycleTime[i][ReadingsStoredInArrays-1],PulseVolume[i][ReadingsStoredInArrays-1]);
		}
		printf("The average pulse peak flow, duration, cycle time and volume are:\n");
		printf("%f\t%f\t%f\t%f\n",AveragePeakFlow[ReadingsStoredInArrays-1],AveragePulseOnTime[ReadingsStoredInArrays-1],AverageCycleTime[ReadingsStoredInArrays-1],AveragePulseVolume[ReadingsStoredInArrays-1]);
	}else{
		printf("No pulses available for analysis\n");
	}
	if(TooManyPulsesInReading	[ReadingsStoredInArrays-1]){
		printf("There were too many pulses in this reading meaning that\nsome of them were not recorded properly and are are not displayed above\n");
	}
	*/
}
void TForm1::SaveRawData(){ // this function needs to know the current reading number and it needs to know the most recently written raw Settings file number
	char FileName[50];
	sprintf(FileName,"raw data %d.txt",Settings->LastRawDataFileNumber+1);
	Settings->WriteFile(); // this record the change in the most_recently_used_raw_data_file_number
	RawDataFileAssosiatedWithReading [ReadingsStoredInArrays-1]=Settings->LastRawDataFileNumber+1;
	FILE * pFile;
	pFile = fopen (FileName,"w+");
	if( pFile==NULL){
		MainLog->Items->Add("Error: could not access the raw data file");
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
		MainLog->Items->Add(tmp);
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
		MainLog->Items->Add("Could not open processed data file\n");
	}else{
		time_t rawtime;
		struct tm * timeinfo;
		char buffer [80];
		time ( &rawtime );
		timeinfo = localtime ( &rawtime );
		fprintf(pFile,"The local date and time when the below data was recorded to file:\n");
		strftime (buffer,80,"Date: %Y-%m-%d Time: %H:%M:%S",timeinfo);
		fprintf(pFile,"%s\n\n",buffer);

		if(ReadingsStoredInArrays>0){
			for (int i = 0; i < ReadingsStoredInArrays; i++) {
				fprintf(pFile,"Below is reading %d assosiated with raw data file %d\n",(i+1),RawDataFileAssosiatedWithReading [i]);
				if(PulsesInReading[i]>0){
					fprintf(pFile,"The pulse peak flow, duration, cycle time and volume are:\n");
					for(int j=0; j < PulsesInReading[i] ; j++){
						fprintf(pFile,"%f\t%f\t%f\t%f\n",PulsePeakFlow[j][i],PulseOnTime[j][i],PulseCycleTime[j][i],PulseVolume[j][i]);
					}
					fprintf(pFile,"The average pulse peak flow, duration, cycle time and volume are:\n");
					fprintf(pFile,"%f\t%f\t%f\t%f\n",AveragePeakFlow[i],AveragePulseOnTime[i],AverageCycleTime[i],AveragePulseVolume[i]);
					if(TooManyPulsesInReading	[i]){
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
		ReadingsStoredInArrays = 0;
		char tmp[100];
		sprintf(tmp,"Processed data file %d created\n",Settings->LastProcessedDataFileNumber);
		MainLog->Items->Add(tmp);
	}
	fclose (pFile);
	ZeroProcessedDataArrays();
}
