//---------------------------------------------------------------------------
#ifndef MainFormH
#define MainFormH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Menus.hpp>
#include <ExtCtrls.hpp>
//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// IDE-managed Components
	TLabel *Label2;
	TLabel *Label3;
	TButton *Start;
	TEdit *MassFlowMonitor;
	TLabel *Label1;
	TLabel *Label4;
	TLabel *Label5;
	TLabel *Label6;
	TEdit *TemperatureMonitor;
	TEdit *PressureMonitor;
	TButton *OutputData;
	TListBox *PulseHistory;
	TLabel *Label7;
	TLabel *Label8;
	TLabel *Label9;
	TLabel *Label10;
	TEdit *AvgVolume;
	TEdit *AvgOnTime;
	TEdit *AvgOffTime;
	TEdit *AvgCycleTime;
	TTimer *FastTimer;
	TListBox *Log;
	TLabel *Label11;
	TEdit *PulsesToRecord;
	TLabel *Label12;
	TEdit *PulsesRecorded;
	TButton *ClearPulseLog;
	TButton *ClearEventLog;
	void __fastcall FastTimerTimer(TObject *Sender);
	void __fastcall OutputDataClick(TObject *Sender);
	void __fastcall StartClick(TObject *Sender);
	void __fastcall ClearPulseLogClick(TObject *Sender);
	void __fastcall ClearEventLogClick(TObject *Sender);
private:	// User declarations
// I define the word "group" to mean a set of pulses and their assosiated data
// I expect that 1 group will only contain data about 1 OCD unit on 1 setting
	void ZeroRawDataArrays();
	void ZeroProcessedDataArrays();
	void StartGroup();
	void GetDataPoint();      // this records data from the flowmeter for a user specified number of seconds, calling it will create one reading
	void ProcessRecentData(); 	// this includes finding all the pulse data other then avarages
	void UpdateAverages();
	void PutProcessedDataOnScreen();
	void FinishGroup();

	void SaveRawData();     // this saves the raw experemental data to a text file. the int passed to it indicates the file number that it should be saved to
	void SaveProcessedData();
private:
	LARGE_INTEGER Frequency;        // ticks per second
	LARGE_INTEGER TicksAtStartOfReading;           // ticks
	bool GroupIsRecording;
	int  TargetPulsesInGroup;
	static const int MaxPulsesInAGroup = 100;
	static const int MaxGroupsBeforeOutputtingAnalysisFile = 100;
	static const int RawDataArraySize = 100000; // it may be a good idea to make the raw data a struct and then make a linked list of structs
	int 			ReadingsInRawDataArray;
	int    		GroupsStoredInArrays;      // I choose to define a reading to be a set of pulses that are grouped together and avaraged to give a result
	double 		TimeOfReading			  	[RawDataArraySize]; // this is the time in milliseconds
	float 		FlowReading						[RawDataArraySize]; // the raw data arrays have been declared here to avoid putting the data on the stack
	int				RawDataFileAssosiatedWithGroup [MaxGroupsBeforeOutputtingAnalysisFile];
	int 			PulsesInGroup				[MaxGroupsBeforeOutputtingAnalysisFile];// each reading will be made up of many pulses (3 is expected)
	bool  		TooManyPulsesInGroup[MaxGroupsBeforeOutputtingAnalysisFile];// this notes if there was more data then was recorded
	float			PulsePeakFlow 				[MaxPulsesInAGroup] [MaxGroupsBeforeOutputtingAnalysisFile];
	float			PulseCycleTime 				[MaxPulsesInAGroup] [MaxGroupsBeforeOutputtingAnalysisFile];
	float			PulseOnTime 					[MaxPulsesInAGroup] [MaxGroupsBeforeOutputtingAnalysisFile];
	float			PulseOffTime         	[MaxPulsesInAGroup] [MaxGroupsBeforeOutputtingAnalysisFile];
	float			PulseVolume 					[MaxPulsesInAGroup] [MaxGroupsBeforeOutputtingAnalysisFile];
	float			AveragePeakFlow 			[MaxGroupsBeforeOutputtingAnalysisFile];
	float			AverageCycleTime 			[MaxGroupsBeforeOutputtingAnalysisFile];
	float			AveragePulseOnTime 		[MaxGroupsBeforeOutputtingAnalysisFile];
	float			AveragePulseOffTime   [MaxGroupsBeforeOutputtingAnalysisFile];
	float			AveragePulseVolume 		[MaxGroupsBeforeOutputtingAnalysisFile];
	// variables exclusively for data processing function
	double IntegratedVolume;
	float CurrentBiggestFlow;
	int MostRecentUp;
	int MostRecentDown;



public:		// User declarations
	__fastcall TForm1(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
