#include <motion_profile.h>

MotionProfile::~MotionProfile() {
	notifier->Stop();
	delete notifier;
}

void MotionProfile::stop() {
	run_profile = false;
	notifier->Stop();
}

void MotionProfile::start(ControlMode mode, float scalar) {
	run_profile = true;
	this->mode = mode;
	this->scalar = scalar;
	notifier->StartPeriodic(interval / 1000.0);
}

void MotionProfile::update() {
	if (!run_profile) { //Dumb hack. Shouldn't have to but...
		notifier->Stop();
	}
	tln_left->Set(mode, command.motion.velocity_left * scalar);
	tln_right->Set(mode, command.motion.velocity_right * scalar);
	bool response = false;
	jetson->write_to(&response, sizeof(response));
	jetson->read_to(&command, sizeof(command));
}

void MotionProfile::print_inputs() {
	std::cout << 
		command.motion.velocity_left << 
		" : " << 
		command.motion.velocity_right << 
		std::endl;
}
