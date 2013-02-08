#ifndef flowmeter_managerH
#define flowmeter_managerH
#include "settings_file_manager.h"
class alicat_flowmeter; // I chose to forward declare this rather then include the whole header file
class tsi_flowmeter;    // I chose to forward declare this rather then include the whole header file
//---------------------------------------------------------------------------
class flowmeter_manager{
	public:
		flowmeter_manager();
		int setup(settings_file_manager *); // this method should not finish until there is a valid flowmeter setup
		float mass_flow();
		~flowmeter_manager();

		bool flowmeter_ready; // this should be true if the flowmeter has been setup and the caller of this class can safely get data from it
		enum flowmeter_type_enum {none, alicat, tsi} flowmeter_type;
	private:
		bool test_port_existence(int port); // this checks if a port is a valid com port
		bool attempt_tsi_setup(int port);
		bool attempt_alicat_setup(int port);

		#define MAX_PORT_NUMBER 200 // this is the largest port number the program can cope with
		bool port_existence[MAX_PORT_NUMBER];
		alicat_flowmeter* alicat_pointer;  // this creates a pointer of type: flowmeter class instance
		tsi_flowmeter* tsi_pointer;
		settings_file_manager* settings_pointer;

};
#endif

