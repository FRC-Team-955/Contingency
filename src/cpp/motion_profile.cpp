#include <motion_profile.h>

MotionProfile::~MotionProfile() {
	notifier->Stop();
	delete notifier;
}

void MotionProfile::stop() {
	run_profile = false;
	tln_left->Set(ControlMode::PercentOutput, 0);
	tln_right->Set(ControlMode::PercentOutput, 0);
	notifier->Stop();
	bool response = true;
	jetson->write_to(&response, sizeof(response));
	jetson->read_to(&command, sizeof(command));
}

void MotionProfile::start(float scalar) {
	run_profile = true;
	this->mode = mode;
	this->scalar = scalar;
	notifier->StartPeriodic(interval / 1000.0);
}

void MotionProfile::update() {
	if (!run_profile) { //Dumb hack. Shouldn't have to but...
		notifier->Stop();
	}

	bool response = false;
	tln_left->Set(mode, command.motion.velocity_left * scalar);
	tln_right->Set(mode, command.motion.velocity_right * scalar);

	jetson->write_to(&response, sizeof(response));
	jetson->read_to(&command, sizeof(command));

	if (command.motion.delta_time == 0.0) {
		stop();
	}
}

void MotionProfile::print_inputs() {
	std::cout << 
		command.motion.velocity_left << 
		" : " << 
		command.motion.velocity_right << 
		std::endl;
}
