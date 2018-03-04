#include <srx_test.h>

void SRXTest::begin() {
	std::cout << "Moving talon " << talon->GetDeviceID() << "!!";
	talon->Set(ControlMode::PercentOutput, speed, 10);
	timer.Reset();
	timer.Start();
}

bool SRXTest::update() {
	if (timer.Get() > duration) {
		talon->Set(ControlMode::PercentOutput, 0.0, 10);
		was_successful = true;
		std::ostringstream info_oss;
		std::ostringstream sugg_oss;
		info_oss << "Talon " << talon->GetDeviceID() << " tried to move. Hope it moved!";
		sugg_oss << "Good luck.";
		return true;
	} else {
		return false;	
	}
}
