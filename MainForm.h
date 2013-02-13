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
	void __fastcall FastTimerTimer(TObject *Sender);
	void __fastcall OutputDataClick(TObject *Sender);
	void __fastcall StartClick(TObject *Sender);
private:	// User declarations
	void	ZeroRawDataArrays();
	void	ZeroProcessedDataArrays();
	void	StartReading();
	void	GetDataPoint();      // this records data from the flowmeter for a user specified number of seconds, calling it will create one reading
	void	FinishReading();
	int		ProcessData();  // I don't like this function, remove later
	void	FindPulseAverages(int reading_of_interest);
	void	PutProcessedDataOnConsole();
	void	SaveRawData();     // this saves the raw experemental data to a text file. the int passed to it indicates the file number that it should be saved to
	void	SaveProcessedData();
private:
	bool RecordingInProgress;
	int NumberOfPulsesToRecord;
	LARGE_INTEGER Frequency;        // ticks per second
	LARGE_INTEGER TicksAtStartOfReading;           // ticks
	static const int MaxPulsesInAReading = 100;
	static const int MaxReadingsBeforeOutputtingAnalysisFile = 100;
	static const int RawDataArraySize = 100000;
	int 			ReadingsInRawDataArray;
	int    		ReadingsStoredInArrays;      // I choose to define a reading to be a set of pulses that are grouped together and avaraged to give a result
	double 		TimeOfReading			  	[RawDataArraySize]; // this is the time in milliseconds
	float 		FlowReading						[RawDataArraySize]; // the raw data arrays have been declared here to avoid putting the data on the stack
	int				RawDataFileAssosiatedWithReading [MaxReadingsBeforeOutputtingAnalysisFile];
	int 			PulsesInReading				[MaxReadingsBeforeOutputtingAnalysisFile];// each reading will be made up of many pulses (3 is expected)
	bool  		TooManyPulsesInReading[MaxReadingsBeforeOutputtingAnalysisFile];// this notes if there was more data then was recorded
	float			PulsePeakFlow 				[MaxPulsesInAReading] [MaxReadingsBeforeOutputtingAnalysisFile];
	float			PulseCycleTime 				[MaxPulsesInAReading] [MaxReadingsBeforeOutputtingAnalysisFile];
	float			PulseOnTime 					[MaxPulsesInAReading] [MaxReadingsBeforeOutputtingAnalysisFile];
	float			PulseVolume 					[MaxPulsesInAReading] [MaxReadingsBeforeOutputtingAnalysisFile];
	float			AveragePeakFlow 			[MaxReadingsBeforeOutputtingAnalysisFile];
	float			AverageCycleTime 			[MaxReadingsBeforeOutputtingAnalysisFile];
	float			AveragePulseOnTime 		[MaxReadingsBeforeOutputtingAnalysisFile];
	float			AveragePulseVolume 		[MaxReadingsBeforeOutputtingAnalysisFile];
public:		// User declarations
	__fastcall TForm1(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
