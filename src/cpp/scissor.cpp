#include <scissor.h>

//TODO: Don't hardcode anything (AKA talon timeouts, PID loop idx)
//      Slowdown near the bottom when moving downwards
//      Don't use opposite 'slowdowns' when moving in the other direction, do it some cleaner way
//      Hard limit for going farther up than the physical limit

void ScissorLift::start_loop(float sync_p_gain, float max_peak_output) {
	this->sync_p_gain = sync_p_gain;
	this->max_peak_output = max_peak_output;
	target = get_position();
	stopped = false;
	notifier->StartPeriodic(0.01); 
}

void ScissorLift::stop_loop() {
	//notifier->Stop(); //Why the fuck won't this work?!
	stopped = true;
}

void ScissorLift::run_loop() {
	if (stopped) { //DO NOT REMOVE THIS I HAVE NO IDEA WHY IT IS NECESSARY
		notifier->Stop();
	}
	float position_left = talon_left->GetSelectedSensorPosition(0);
	float position_right = talon_right->GetSelectedSensorPosition(0);

	float left_slowdown = clamp_max(sync_p_gain * (position_left - position_right));
	float right_slowdown = clamp_max(sync_p_gain * (position_right - position_left));

	//TODO: Make it more clear that up is negative and that down is positive
	//      Shorten this if possible
	talon_left->Set(ControlMode::Position, target, 10);
	//Adjust your speed based on how far away from the other scissor you are
	talon_left->ConfigPeakOutputForward(max_peak_output - left_slowdown, 10);
	talon_left->ConfigPeakOutputReverse(-(max_peak_output - right_slowdown), 10);

	talon_right->Set(ControlMode::Position, target, 10);
	talon_right->ConfigPeakOutputForward(max_peak_output - right_slowdown, 10);
	talon_right->ConfigPeakOutputReverse(-(max_peak_output - left_slowdown), 10);
}

bool ScissorLift::home(float speed) {
	talon_left->ConfigPeakOutputForward(speed, 10);
	talon_right->ConfigPeakOutputForward(speed, 10);
	bool dio_left_depressed = dio_left->Get() == LimitSwitchState::Closed;
	bool dio_right_depressed = dio_right->Get() == LimitSwitchState::Closed;
	if (dio_left_depressed) {
		talon_left->Set(ControlMode::PercentOutput, 0);
		//std::cout << "STOP LEFT" << std::endl;
	} else {
		talon_left->Set(ControlMode::PercentOutput, 0.25);
	}
	if (dio_right_depressed) {
		talon_right->Set(ControlMode::PercentOutput, 0);
		//std::cout << "STOP RIGHT" << std::endl;
	} else {
		talon_right->Set(ControlMode::PercentOutput, 0.25);
	}
	if (dio_left_depressed && dio_right_depressed) {
		target = 0.0;
		talon_left->SetSelectedSensorPosition(0, 0, 10);
		talon_right->SetSelectedSensorPosition(0, 0, 10);
		return true; 
	} else {
		return false;
	}
}

inline float ScissorLift::clamp_max (float input) {
	if (input < 0.0) {
		return 0.0;
	}
	if (input > max_peak_output) {
		return max_peak_output;
	}
	return input;
}

float ScissorLift::get_sync_error() {
	return talon_left->GetSelectedSensorPosition(0) - talon_right->GetSelectedSensorPosition(0);
}

float ScissorLift::get_position() {
	return std::min(talon_left->GetSelectedSensorPosition(0), talon_right->GetSelectedSensorPosition(0));
}

float ScissorLift::get_target() {
	return target;
}

void ScissorLift::set_position(float target) {
	// Don't set the target if you're going to go out of position limits
	if (target >= scissorlift_minumum_height_in * scissorlift_in_to_nu) {
		this->target = 0;
	} else if (target <= scissorlift_maximum_height_in * scissorlift_in_to_nu) {
		this->target = scissorlift_maximum_height_in * scissorlift_in_to_nu;
	} else {
		this->target = target;
	}
}
