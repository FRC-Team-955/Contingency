#include <motion_profile.h>

void MotionProfile::stop() {
	std::cout << "Motion profile stopped!" << std::endl;
	run_profile = false;
	tln_drbase_left->Set(ControlMode::PercentOutput, 0);
	tln_drbase_right->Set(ControlMode::PercentOutput, 0);
	tln_intake_left->Set(ControlMode::PercentOutput, 0);
	tln_intake_right->Set(ControlMode::PercentOutput, 0);
}

void MotionProfile::start(JetsonCommand::Setup::LayoutBits bits) {
	setup.setup_data.layout_bits = bits;
	run_profile = true;
	//jetson->write_to(&setup, sizeof(setup));
	this->mode = mode;
	notifier.StartPeriodic(interval / 1000.0);
}

void MotionProfile::update() {
	if (!run_profile) { // Dumb hack. Shouldn't have to but...
		notifier.Stop();
	}

	RioCommand input;
	JetsonCommand output;

	jetson->read_to(&input, sizeof(RioCommand));
	// std::cerr << "Recv: " << input.type << std::endl;
	switch (input.type) {
		case RioCommand::Type::Motion:
			tln_drbase_left->Set(mode, -input.motion.velocity_left * scalar);
			tln_drbase_right->Set(mode, input.motion.velocity_right * scalar);
			if (input.action & RioCommand::Action::Cube_Intake) {
				tln_intake_left->Set(ControlMode::PercentOutput, 0.4);
				tln_intake_right->Set(ControlMode::PercentOutput, 0.4);
			} else {
				tln_intake_left->Set(ControlMode::PercentOutput, 0);
				tln_intake_right->Set(ControlMode::PercentOutput, 0);
			}
			output.type = JetsonCommand::Type::Request_Motion;
			jetson->write_to(&output, sizeof(output));
			break;
		case RioCommand::Type::Request_Setup:
			jetson->write_to(&setup, sizeof(setup));
			break;
		case RioCommand::Type::Stop:
			stop();
			break;
		case RioCommand::Type::RioNone:
			output.type = JetsonCommand::Type::Request_Motion;
			jetson->write_to(&output, sizeof(output));
			break;
		default:
			std::cerr << "ERROR Garbage received: " << input.type << std::endl;
			break;
	}


	//std::cout << input.motion.velocity_left * scalar << " : "
	//	<< input.motion.velocity_right * scalar << std::endl;

	//std::cout << input.motion.velocity_left << " : "
	//	<< input.motion.velocity_right << std::endl;
}

MotionProfile::~MotionProfile() {
	run_profile = false;
	notifier.Stop();
}

bool MotionProfile::is_running() {
	return run_profile;
}
