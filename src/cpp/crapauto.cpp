#include <crapauto.h>

void CrapAuto::start() {
	timer.Reset();
	timer.Start();
}

void CrapAuto::update() {
	if (timer.Get() > duration) {
		left->Set(ControlMode::PercentOutput, 0);	
		right->Set(ControlMode::PercentOutput, 0);	
	} else {
		left->Set(ControlMode::PercentOutput, -speed);	
		right->Set(ControlMode::PercentOutput, speed);	
	}
}

