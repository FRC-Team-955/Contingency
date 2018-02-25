#include <limit_switch_test.h>

void LimitSwitchTest::begin() {
	timer.Reset();
	timer.Start();
}

bool LimitSwitchTest::update() {
	if (timer.Get() > 5.0) {
		if (!was_successful) {
			was_successful = lm_switch->Get();
		}
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
		info.append(info_oss.str());
		suggestion.append(sugg_oss.str());
		return true;
	} else {
		return false;	
	}
}
