#include <scissor.h>

//TODO: Don't hardcode anything (AKA talon timeouts, PID loop idx)
//      Slowdown near the bottom when moving downwards
//      Don't use opposite 'slowdowns' when moving in the other direction, do it some cleaner way
//      Hard limit for going farther up than the physical limit

void ScissorLift::start_loop(float sync_p_gain, float max_peak_output) {
	this->sync_p_gain = sync_p_gain;
	this->max_peak_output = max_peak_output;
	stopped = false;
	notifier->StartPeriodic(0.01); 
}

void ScissorLift::stop_loop() {
	//notifier->Stop(); //Why the fuck won't this work?!
	stopped = true;
}

void ScissorLift::run_loop() {
	float position_sync_error = get_sync_error();
	if (stopped) { //DO NOT REMOVE THIS I HAVE NO IDEA WHY IT IS NECESSARY
		notifier->Stop();
	}

	//TODO: Document how this works
	float left_slowdown = clamp_max(sync_p_gain * (talon_left->GetSelectedSensorPosition(0) - talon_right->GetSelectedSensorPosition(0)));
	float right_slowdown = clamp_max(sync_p_gain * (talon_right->GetSelectedSensorPosition(0) - talon_left->GetSelectedSensorPosition(0)));

	//TODO: Make it more clear that up is negative and that down is positive
	//      Shorten this if possible
	if (target > talon_left->GetSelectedSensorPosition(0) && dio_left->Get() == LimitSwitchState::Closed) {
		//Moving downward (Positive direction) (Hopefully, haha) and the limit switch is depressed. Reset the encoder position and STOP!
		talon_left->Set(ControlMode::PercentOutput, 0, 10);
		talon_left->SetSelectedSensorPosition(0, 0, 10);
	} else {
		//Adjust your speed based on how far away from the other scissor you are
		talon_left->ConfigPeakOutputForward(max_peak_output - left_slowdown, 10);
		talon_left->ConfigPeakOutputReverse(-(max_peak_output - right_slowdown), 10);
		talon_left->Set(ControlMode::Position, target, 10);
	}

	if (target > talon_right->GetSelectedSensorPosition(0) && dio_right->Get() == LimitSwitchState::Closed) {
		talon_right->Set(ControlMode::PercentOutput, 0, 10);
		talon_right->SetSelectedSensorPosition(0, 0, 10);
	} else {
		talon_right->ConfigPeakOutputForward(max_peak_output - right_slowdown, 10);
		talon_right->ConfigPeakOutputReverse(-(max_peak_output - left_slowdown), 10);
		talon_right->Set(ControlMode::Position, target, 10);
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
	return (talon_left->GetSelectedSensorPosition(0) + talon_right->GetSelectedSensorPosition(0) / 2);
}

float ScissorLift::get_target() {
	return target;
}

void ScissorLift::set_position(float target_inch) {
	this->target = target_inch;
}
