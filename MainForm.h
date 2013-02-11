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
	TButton *Pause;
	TButton *Button2;
	TEdit *MassFlowMonitor;
	TLabel *Label1;
	TLabel *Label4;
	TLabel *Label5;
	TLabel *Label6;
	TEdit *TemperatureMonitor;
	TEdit *PressureMonitor;
	TButton *Button3;
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
	TListBox *EventLog;
	void __fastcall FastTimerTimer(TObject *Sender);
	void __fastcall PauseClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TForm1(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
