object Form1: TForm1
  Left = 0
  Top = 0
  Caption = 'OCD test program'
  ClientHeight = 453
  ClientWidth = 758
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  DesignSize = (
    758
    453)
  PixelsPerInch = 96
  TextHeight = 13
  object Label2: TLabel
    Left = 67
    Top = 200
    Width = 87
    Height = 13
    Caption = 'Running averages'
  end
  object Label3: TLabel
    Left = 248
    Top = 12
    Width = 80
    Height = 13
    Caption = 'Record of pulses'
  end
  object Label1: TLabel
    Left = 250
    Top = 235
    Width = 45
    Height = 13
    Caption = 'Event log'
  end
  object Label4: TLabel
    Left = 10
    Top = 10
    Width = 86
    Height = 13
    Caption = 'Mass flow (SCCM)'
  end
  object Label5: TLabel
    Left = 10
    Top = 69
    Width = 104
    Height = 13
    Caption = 'Temperature (celcius)'
  end
  object Label6: TLabel
    Left = 10
    Top = 40
    Width = 69
    Height = 13
    Caption = 'Pressure (bar)'
  end
  object Label7: TLabel
    Left = 10
    Top = 320
    Width = 55
    Height = 13
    Caption = 'Volume (ml)'
  end
  object Label8: TLabel
    Left = 10
    Top = 260
    Width = 87
    Height = 13
    Caption = 'On time (seconds)'
  end
  object Label9: TLabel
    Left = 10
    Top = 230
    Width = 99
    Height = 13
    Caption = 'Cycle time (seconds)'
  end
  object Label10: TLabel
    Left = 10
    Top = 290
    Width = 89
    Height = 13
    Caption = 'Off time (seconds)'
  end
  object Label11: TLabel
    Left = 10
    Top = 147
    Width = 77
    Height = 13
    Caption = 'Pulses to record'
  end
  object Label12: TLabel
    Left = 10
    Top = 176
    Width = 76
    Height = 13
    Caption = 'Pulses recorded'
  end
  object Label13: TLabel
    Left = 248
    Top = 40
    Width = 233
    Height = 13
    Caption = 'Peak flow, Cycle time, On time, Off time, Volume'
  end
  object Start: TButton
    Left = 8
    Top = 95
    Width = 105
    Height = 34
    Caption = 'Start'
    TabOrder = 0
    OnClick = StartClick
  end
  object MassFlowMonitor: TEdit
    Left = 120
    Top = 8
    Width = 83
    Height = 21
    TabOrder = 1
    Text = 'MassFlowMonitor'
  end
  object TemperatureMonitor: TEdit
    Left = 120
    Top = 63
    Width = 83
    Height = 21
    TabOrder = 2
    Text = 'TemperatureMonitor'
  end
  object PressureMonitor: TEdit
    Left = 120
    Top = 36
    Width = 83
    Height = 21
    TabOrder = 3
    Text = 'PressureMonitor'
  end
  object OutputData: TButton
    Left = 119
    Top = 95
    Width = 105
    Height = 34
    Caption = 'Output data to file'
    TabOrder = 4
    OnClick = OutputDataClick
  end
  object PulseHistory: TListBox
    Left = 250
    Top = 63
    Width = 480
    Height = 162
    Anchors = [akLeft, akTop, akRight]
    ItemHeight = 13
    TabOrder = 5
  end
  object AvgVolume: TEdit
    Left = 120
    Top = 316
    Width = 83
    Height = 21
    TabOrder = 6
    Text = 'AvgVolume'
  end
  object AvgOnTime: TEdit
    Left = 120
    Top = 256
    Width = 83
    Height = 21
    TabOrder = 7
    Text = 'AvgOnTime'
  end
  object AvgOffTime: TEdit
    Left = 120
    Top = 286
    Width = 83
    Height = 21
    TabOrder = 8
    Text = 'AvgOffTime'
  end
  object AvgCycleTime: TEdit
    Left = 120
    Top = 226
    Width = 83
    Height = 21
    TabOrder = 9
    Text = 'AvgCycleTime'
  end
  object Log: TListBox
    Left = 253
    Top = 262
    Width = 477
    Height = 167
    Anchors = [akLeft, akTop, akRight, akBottom]
    ItemHeight = 13
    TabOrder = 10
  end
  object PulsesToRecord: TEdit
    Left = 120
    Top = 143
    Width = 83
    Height = 21
    TabOrder = 11
    Text = 'PulsesToRecord'
  end
  object PulsesRecorded: TEdit
    Left = 120
    Top = 173
    Width = 83
    Height = 21
    TabOrder = 12
    Text = 'PulsesRecorded'
  end
  object ClearPulseLog: TButton
    Left = 344
    Top = 8
    Width = 75
    Height = 25
    Caption = 'Clear'
    TabOrder = 13
    OnClick = ClearPulseLogClick
  end
  object ClearEventLog: TButton
    Left = 344
    Top = 231
    Width = 75
    Height = 25
    Caption = 'Clear'
    TabOrder = 14
    OnClick = ClearEventLogClick
  end
  object FastTimer: TTimer
    Interval = 1
    OnTimer = FastTimerTimer
    Left = 102
    Top = 364
  end
end
