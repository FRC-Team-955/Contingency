#include <motion_profile.h>

void MotionProfile::stop() {
	std::cout << "Motion profile stopped!" << std::endl;
	run_profile = false;
	tln_drbase_left->Set(ControlMode::PercentOutput, 0);
	tln_drbase_right->Set(ControlMode::PercentOutput, 0);
	tln_intake_left->Set(ControlMode::PercentOutput, 0);
	tln_intake_right->Set(ControlMode::PercentOutput, 0);
}

void MotionProfile::start(char* layout) {
	strcpy(setup.config, layout);
	setup.type = JetsonCommand::Type::Reset;
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
			{
				tln_drbase_left->Set(mode, -input.motion.velocity_left * scalar);
				tln_drbase_right->Set(mode, input.motion.velocity_right * scalar);

				//float fgain_left = (((fabs(input.motion.velocity_left * scalar) * 0.000212572541) + 0.2377763902) * 1023.0) / fabs(input.motion.velocity_left * scalar);
				//float fgain_right = (((fabs(input.motion.velocity_right * scalar) * 0.000212572541) + 0.2377763902) * 1023.0) / fabs(input.motion.velocity_right * scalar);

				//tln_drbase_left->Config_kF(0, std::min(fgain_left, 4.0f), 10);
				//tln_drbase_right->Config_kF(0, std::min(fgain_right, 4.0f), 10);

				if (input.action & Action::Cube_Expel) {
					tln_intake_left->Set(ControlMode::PercentOutput, -1.0);
					tln_intake_right->Set(ControlMode::PercentOutput, -1.0);
				} else if (input.action & Action::Cube_Intake) {
					tln_intake_left->Set(ControlMode::PercentOutput, 1.0);
					tln_intake_right->Set(ControlMode::PercentOutput, 1.0);
				} else {
					tln_intake_left->Set(ControlMode::PercentOutput, 0);
					tln_intake_right->Set(ControlMode::PercentOutput, 0);
				}
				if (input.action & Action::Scissor_Switch) {
					scissor->set_position(-scissorlift_one_rotation_nu * 3.0 * 3);
				} else if (input.action & Action::Scissor_Scale) {
					scissor->set_position(-scissorlift_one_rotation_nu * 3.0 * 10);
				} else {
					scissor->set_position(0);
				}
				output.type = JetsonCommand::Type::Request_Motion;
				jetson->write_to(&output, sizeof(output));
				std::cout << "MOTION PROFILE: " <<  input.motion.velocity_left * scalar << " : "
					<< input.motion.velocity_right * scalar << std::endl;
			}
			break;
		case RioCommand::Type::Request_Setup:
			std::cout << "SENT SETUP" << std::endl;
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

}

MotionProfile::~MotionProfile() {
	run_profile = false;
	notifier.Stop();
}

bool MotionProfile::is_running() {
	return run_profile;
}
