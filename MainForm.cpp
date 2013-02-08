// Environment Resources ----------------------------------------------------
#include <vcl>
#include <stdio>
#include <iostream>
#include <windows>
#include <cassert>
#include <new>
#pragma hdrstop
#pragma package(smart_init)
#pragma resource "*.dfm"
// My Header Files ----------------------------------------------------------
#include "MainForm.h"
#include "SettingsFileManager.h"
#include "FlowmeterManager.h"
#include "RingBuffer.h"
// My Global Variables-------------------------------------------------------
TForm1 *Form1;
char LastMassFlowString[100];
char LastTemperatureString[100];
char LastPressureString[100];
// My Classes ---------------------------------------------------------------
SettingsFileManager * Settings;
FlowmeterManager * GenericFlowmeter;

//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner) : TForm(Owner)
{
	Settings = new SettingsFileManager;
	Settings->ReadFile();
	GenericFlowmeter = new FlowmeterManager(Settings,FlowmeterStatus);//I want this class to always have acsess to these pointers
	GenericFlowmeter->Setup();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::FastTimerTimer(TObject *Sender)
{
	GenericFlowmeter->CallMeRegularly();

	sprintf(LastMassFlowString,"%f",GenericFlowmeter->LastMassFlow);
	MassFlowMonitor->Text = LastMassFlowString;

	sprintf(LastTemperatureString,"%f",GenericFlowmeter->LastTemperature);
	TemperatureMonitor->Text = LastTemperatureString;

	sprintf(LastPressureString,"%f",GenericFlowmeter->LastPressure);
	PressureMonitor->Text = LastPressureString;
}
//---------------------------------------------------------------------------
void __fastcall TForm1::PauseClick(TObject *Sender)
{
	assert(true);
}
//---------------------------------------------------------------------------

