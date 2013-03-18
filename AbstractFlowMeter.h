#ifndef AbstractFlowMeterH
#define AbstractFlowMeterH

class AbstractFlowMeter{
	public:
		AbstractFlowMeter();
		virtual bool CheckPresence()=0;	// checks there is a flow meter of the correct type on the given port
		virtual void CallMeRegularly()=0;// this should be called at least once every second, it handles the streaming data
		virtual bool IsThereNewData();// this checks if there has been new data recieved since the last call to IsThereNewData
		virtual double DataAge();   	// the age of the most recent good data measured in seconds. (this might be used later to detect that the flowmeter was unplugged)
		virtual float MassFlow(); 	// should return a value in SCCM
		virtual float Temperature(); 	// should return a value in Celcius
		virtual float Pressure(); 	// should return a value in Bar
	protected:
		LARGE_INTEGER TicksAssosiatedWithLastGoodData;
		LARGE_INTEGER Frequency;        // ticks per second
		bool ThereIsNewData;
		float LastMassFlow;
		float LastVolumetricFlow;
		float LastTemperature;
		float LastPressure;
};
#endif
