#include <limit_switch_test.h>

void LimitSwitchTest::begin() {
	std::cout << "Please depresss limit switch " << lm_switch->GetChannel() << " !!" << std::endl;
	timer.Reset();
	timer.Start();
}

bool LimitSwitchTest::update() {
	if (timer.Get() < 5.0) { //Wait 5 seconds or until the limit switch was depressed
		if (!was_successful) {
			was_successful = lm_switch->Get() == LimitSwitchState::Closed;
		}
		return false;
	} else {
		std::ostringstream info_oss;
		std::ostringstream sugg_oss;
		int lm_switch_id = lm_switch->GetChannel();
		if (was_successful) {
			info_oss << "Limit switch " << lm_switch_id << " moved successfully, everything looks good!";
			sugg_oss << "Celebrate!";
		} else {
			info_oss << "Limit switch " << lm_switch_id << " either failed to move, or the switch is not connected.";
			sugg_oss << "Check the connection to the DIO port on the RIO and run the diagnostic again.";
		}
		std::cout << "Interactive test finished. " << std::endl;
		info.append(info_oss.str());
		suggestion.append(sugg_oss.str());
		return true;	
	}
}
