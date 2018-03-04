#include <encoded_srx_test.h>

void EncodedSRXTest::begin() {
	std::cout << "Moving talon " << talon->GetDeviceID() << "!!";
	talon->SetSelectedSensorPosition(0.0, 0, 10);
	talon->Set(ControlMode::PercentOutput, speed, 10);
	timer.Reset();
	timer.Start();
}

bool EncodedSRXTest::update() {
	if (timer.Get() > duration) {
		talon->Set(ControlMode::PercentOutput, 0.0, 10);
		was_successful = std::fabs(talon->GetSelectedSensorPosition(0)) > 10;
		std::ostringstream info_oss;
		std::ostringstream sugg_oss;
		int talon_id = talon->GetDeviceID();
		if (was_successful) {
			info_oss << "Talon " << talon_id << " moved successfully, everything looks good!";
			sugg_oss << "Celebrate!";
		} else {
			info_oss << "Talon " << talon_id << " either failed to move, or the encoder is not connected.";
			sugg_oss << "If talon " << talon_id << " did move, it is likely that the encoder is broken. If it did not, the motor is likely disconnected";
			//TODO: Make it suggest a test that the user can move the motor and check the output
			// And autoqueue another test to run the motor again and see if any other encoders on the bot move instead, 
			// then suggesting that that encoder is wired to the wrong location. 
		}
		info.append(info_oss.str());
		suggestion.append(sugg_oss.str());
		return true;
	} else {
		return false;	
	}
}
