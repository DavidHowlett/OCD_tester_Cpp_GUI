object Form1: TForm1
  Left = 0
  Top = 0
  Caption = 'OCD test program'
  ClientHeight = 344
  ClientWidth = 345
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  DesignSize = (
    345
    344)
  PixelsPerInch = 96
  TextHeight = 13
  object Label2: TLabel
    Left = 8
    Top = 267
    Width = 87
    Height = 13
    Caption = 'Running averages'
  end
  object Label3: TLabel
    Left = 8
    Top = 141
    Width = 80
    Height = 13
    Caption = 'Record of pulses'
  end
  object Label1: TLabel
    Left = 8
    Top = 8
    Width = 45
    Height = 13
    Caption = 'Event log'
  end
  object Label4: TLabel
    Left = 8
    Top = 84
    Width = 86
    Height = 13
    Caption = 'Mass flow (SCCM)'
  end
  object Label5: TLabel
    Left = 208
    Top = 84
    Width = 104
    Height = 13
    Caption = 'Temperature (celcius)'
  end
  object Label6: TLabel
    Left = 108
    Top = 84
    Width = 69
    Height = 13
    Caption = 'Pressure (bar)'
  end
  object Label7: TLabel
    Left = 8
    Top = 286
    Width = 55
    Height = 13
    Caption = 'Volume (ml)'
  end
  object Label8: TLabel
    Left = 80
    Top = 286
    Width = 37
    Height = 13
    Caption = 'On time'
  end
  object Label9: TLabel
    Left = 224
    Top = 286
    Width = 49
    Height = 13
    Caption = 'Cycle time'
  end
  object Label10: TLabel
    Left = 152
    Top = 286
    Width = 39
    Height = 13
    Caption = 'Off time'
  end
  object Pause: TButton
    Left = 232
    Top = 141
    Width = 105
    Height = 34
    Caption = 'Start'
    TabOrder = 0
    OnClick = PauseClick
  end
  object MassFlowMonitor: TEdit
    Left = 8
    Top = 103
    Width = 80
    Height = 21
    TabOrder = 1
    Text = 'MassFlowMonitor'
  end
  object TemperatureMonitor: TEdit
    Left = 208
    Top = 103
    Width = 81
    Height = 21
    TabOrder = 2
    Text = 'TemperatureMonitor'
  end
  object PressureMonitor: TEdit
    Left = 108
    Top = 103
    Width = 83
    Height = 21
    TabOrder = 3
    Text = 'PressureMonitor'
  end
  object Button3: TButton
    Left = 231
    Top = 221
    Width = 105
    Height = 34
    Caption = 'Output data to file'
    TabOrder = 4
  end
  object PulseHistory: TListBox
    Left = 8
    Top = 160
    Width = 217
    Height = 101
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
    Left = 8
    Top = 308
    Width = 66
    Height = 21
    TabOrder = 6
    Text = 'AvgVolume'
  end
  object AvgOnTime: TEdit
    Left = 80
    Top = 308
    Width = 66
    Height = 21
    TabOrder = 7
    Text = 'AvgOnTime'
  end
  object AvgOffTime: TEdit
    Left = 152
    Top = 308
    Width = 66
    Height = 21
    TabOrder = 8
    Text = 'AvgOffTime'
  end
  object AvgCycleTime: TEdit
    Left = 224
    Top = 308
    Width = 65
    Height = 21
    TabOrder = 9
    Text = 'AvgCycleTime'
  end
  object EventLog: TListBox
    Left = 8
    Top = 27
    Width = 324
    Height = 49
    Anchors = [akLeft, akTop, akRight]
    ItemHeight = 13
    TabOrder = 10
  end
  object Button2: TButton
    Left = 231
    Top = 181
    Width = 105
    Height = 34
    Caption = 'Reset'
    TabOrder = 11
  end
  object FastTimer: TTimer
    Interval = 1
    OnTimer = FastTimerTimer
    Left = 296
    Top = 268
  end
end
