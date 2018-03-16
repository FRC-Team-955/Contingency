#include <crapauto.h>

void CrapAuto::start() {
	timer.Reset();
	timer.Start();
	shifter_a->Set(false);
	shifter_b->Set(true);
	left->Set(ControlMode::PercentOutput, -speed);	
	right->Set(ControlMode::PercentOutput, speed);	
}

void CrapAuto::update() {
	if (timer.Get() > duration) {
		left->Set(ControlMode::PercentOutput, 0);	
		right->Set(ControlMode::PercentOutput, 0);	
	}
}

