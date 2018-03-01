#include <scissor_home_test.h>

void ScissorHomeTest::begin() {
	std::cout << "STARTING SCISSOR LIFT HOME!! WATCH OUT" << std::endl;
	timer.Reset();
	timer.Start();
}

bool ScissorHomeTest::update() {
	if (timer.Get() < 5.0) { //Wait 5 seconds or until the limit switch was depressed
		if (!was_successful) {
			was_successful = scissor->home(speed);
		}
		return false;
	} else {
		std::ostringstream info_oss;
		std::ostringstream sugg_oss;
		if (was_successful) {
			info_oss << "Scissor lift homed successfully, everything looks good!";
			sugg_oss << "Celebrate!";
		} else {
			info_oss << "Scissor lift either failed to move, or a switch is not connected.";
			sugg_oss << "Check the connections to the DIO ports on the RIO and run the diagnostic again.";
		}
		std::cout << "Interactive test finished. " << std::endl;
		info.append(info_oss.str());
		suggestion.append(sugg_oss.str());
		return true;	
	}
}
