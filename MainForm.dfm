object Form1: TForm1
  Left = 0
  Top = 0
  Caption = 'OCD test program'
  ClientHeight = 493
  ClientWidth = 709
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  DesignSize = (
    709
    493)
  PixelsPerInch = 96
  TextHeight = 13
  object Label2: TLabel
    Left = 25
    Top = 187
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
    Top = 150
    Width = 45
    Height = 13
    Caption = 'Event log'
  end
  object Label4: TLabel
    Left = 8
    Top = 8
    Width = 86
    Height = 13
    Caption = 'Mass flow (SCCM)'
  end
  object Label5: TLabel
    Left = 8
    Top = 65
    Width = 104
    Height = 13
    Caption = 'Temperature (celcius)'
  end
  object Label6: TLabel
    Left = 8
    Top = 38
    Width = 69
    Height = 13
    Caption = 'Pressure (bar)'
  end
  object Label7: TLabel
    Left = 25
    Top = 214
    Width = 55
    Height = 13
    Caption = 'Volume (ml)'
  end
  object Label8: TLabel
    Left = 25
    Top = 233
    Width = 37
    Height = 13
    Caption = 'On time'
  end
  object Label9: TLabel
    Left = 25
    Top = 310
    Width = 49
    Height = 13
    Caption = 'Cycle time'
  end
  object Label10: TLabel
    Left = 25
    Top = 266
    Width = 39
    Height = 13
    Caption = 'Off time'
  end
  object Label11: TLabel
    Left = 8
    Top = 96
    Width = 77
    Height = 13
    Caption = 'Pulses to record'
  end
  object Start: TButton
    Left = 7
    Top = 123
    Width = 105
    Height = 34
    Anchors = [akTop, akRight]
    Caption = 'Start'
    TabOrder = 0
    OnClick = StartClick
  end
  object MassFlowMonitor: TEdit
    Left = 100
    Top = 8
    Width = 83
    Height = 21
    TabOrder = 1
    Text = 'MassFlowMonitor'
  end
  object TemperatureMonitor: TEdit
    Left = 118
    Top = 65
    Width = 84
    Height = 21
    TabOrder = 2
    Text = 'TemperatureMonitor'
  end
  object PressureMonitor: TEdit
    Left = 100
    Top = 38
    Width = 83
    Height = 21
    TabOrder = 3
    Text = 'PressureMonitor'
  end
  object OutputData: TButton
    Left = 118
    Top = 123
    Width = 105
    Height = 34
    Anchors = [akTop, akRight]
    Caption = 'Output data to file'
    TabOrder = 4
    OnClick = OutputDataClick
  end
  object PulseHistory: TListBox
    Left = 250
    Top = 38
    Width = 419
    Height = 101
    Anchors = [akLeft, akTop, akRight]
    ItemHeight = 13
    Items.Strings = (
      'volume, on time, off time, cycle time'
      'volume, on time, off time, cycle time'
      'volume, on time, off time, cycle time'
      'volume, on time, off time, cycle time'
      '')
    TabOrder = 5
  end
  object AvgVolume: TEdit
    Left = 117
    Top = 206
    Width = 66
    Height = 21
    TabOrder = 6
    Text = 'AvgVolume'
  end
  object AvgOnTime: TEdit
    Left = 117
    Top = 233
    Width = 66
    Height = 21
    TabOrder = 7
    Text = 'AvgOnTime'
  end
  object AvgOffTime: TEdit
    Left = 117
    Top = 263
    Width = 66
    Height = 21
    TabOrder = 8
    Text = 'AvgOffTime'
  end
  object AvgCycleTime: TEdit
    Left = 118
    Top = 307
    Width = 65
    Height = 21
    TabOrder = 9
    Text = 'AvgCycleTime'
  end
  object Log: TListBox
    Left = 253
    Top = 169
    Width = 416
    Height = 77
    Anchors = [akLeft, akTop, akRight]
    ItemHeight = 13
    TabOrder = 10
  end
  object PulsesToRecord: TEdit
    Left = 112
    Top = 96
    Width = 97
    Height = 21
    TabOrder = 11
    Text = 'PulsesToRecord'
  end
  object FastTimer: TTimer
    Interval = 1
    OnTimer = FastTimerTimer
    Left = 312
    Top = 292
  end
end
