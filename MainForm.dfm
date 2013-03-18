object Form1: TForm1
  Left = 0
  Top = 0
  Caption = 'OCD test program'
  ClientHeight = 458
  ClientWidth = 795
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  DesignSize = (
    795
    458)
  PixelsPerInch = 96
  TextHeight = 13
  object Label2: TLabel
    Left = 256
    Top = 400
    Width = 87
    Height = 13
    Anchors = [akLeft, akBottom]
    Caption = 'Running averages'
    ExplicitTop = 375
  end
  object Label3: TLabel
    Left = 256
    Top = 9
    Width = 80
    Height = 13
    Caption = 'Record of pulses'
  end
  object Label1: TLabel
    Left = 8
    Top = 204
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
    Left = 256
    Top = 37
    Width = 187
    Height = 13
    Caption = 'Peak flow, On time, Cycle time, Volume'
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
  object Log: TListBox
    Left = 8
    Top = 231
    Width = 233
    Height = 219
    Anchors = [akLeft, akTop, akBottom]
    ItemHeight = 13
    TabOrder = 5
  end
  object PulsesToRecord: TEdit
    Left = 120
    Top = 143
    Width = 83
    Height = 21
    TabOrder = 6
    Text = 'PulsesToRecord'
  end
  object PulsesRecorded: TEdit
    Left = 120
    Top = 173
    Width = 83
    Height = 21
    TabOrder = 7
    Text = 'PulsesRecorded'
  end
  object ClearPulseLog: TButton
    Left = 352
    Top = 5
    Width = 75
    Height = 25
    Caption = 'Clear'
    TabOrder = 8
    OnClick = ClearPulseLogClick
  end
  object ClearEventLog: TButton
    Left = 104
    Top = 200
    Width = 75
    Height = 25
    Caption = 'Clear'
    TabOrder = 9
    OnClick = ClearEventLogClick
  end
  object PulseHistory: TMemo
    Left = 256
    Top = 56
    Width = 519
    Height = 330
    Anchors = [akLeft, akTop, akRight, akBottom]
    TabOrder = 10
  end
  object RunningAvgs: TEdit
    Left = 256
    Top = 425
    Width = 519
    Height = 21
    Anchors = [akLeft, akRight, akBottom]
    TabOrder = 11
  end
  object FastTimer: TTimer
    Interval = 1
    OnTimer = FastTimerTimer
    Left = 214
    Top = 164
  end
end
