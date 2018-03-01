#include <scissor.h>

//TODO: Don't hardcode anything (AKA talon timeouts, PID loop idx)
//      Slowdown near the bottom when moving downwards
//      Don't use opposite 'slowdowns' when moving in the other direction, do it some cleaner way
//      Hard limit for going farther up than the physical limit

void ScissorLift::start_loop(float sync_p_gain, float max_peak_output) {
	this->sync_p_gain = sync_p_gain;
	this->max_peak_output = max_peak_output;
	stopped = false;
	notifier->StartPeriodic(0.02); 
	//TODO: Remove this
	talon_left->SetSelectedSensorPosition(0, 0, 10);
	talon_right->SetSelectedSensorPosition(0, 0, 10);
	target = 0.0;
}

void ScissorLift::stop_loop() {
	//notifier->Stop(); //Why the fuck won't this work?!
	stopped = true;
}

void ScissorLift::update() {
	dio_left_depressed = dio_left->Get() == LimitSwitchState::Closed;
	dio_right_depressed = dio_right->Get() == LimitSwitchState::Closed;
}

void ScissorLift::run_loop() {
	if (stopped) { //DO NOT REMOVE THIS I HAVE NO IDEA WHY IT IS NECESSARY
		notifier->Stop();
	}
	float position_left = talon_left->GetSelectedSensorPosition(0);
	float position_right = talon_right->GetSelectedSensorPosition(0);

	//TODO: Document how this works
	float left_slowdown = clamp_max(sync_p_gain * (position_left - position_right));
	float right_slowdown = clamp_max(sync_p_gain * (position_right - position_left));

	//TODO: Make it more clear that up is negative and that down is positive
	//      Shorten this if possible
	bool left_stop = false;//talon_left->GetClosedLoopTarget(0) > position_left && dio_left_depressed;
	bool right_stop = false;//talon_right->GetClosedLoopTarget(0) > position_right && dio_right_depressed;

	talon_left->Set(ControlMode::Position, target, 10);
	if (left_stop) {
		//Moving downward (Hopefully, haha) and the limit switch is depressed. Reset the encoder position and STOP!
		talon_left->ConfigPeakOutputForward(0.0, 10);
		talon_left->Set(ControlMode::PercentOutput, 0, 10);
	} else if (!right_stop) { 
		//Adjust your speed based on how far away from the other scissor you are
		talon_left->ConfigPeakOutputForward(max_peak_output - left_slowdown, 10);
		talon_left->ConfigPeakOutputReverse(-(max_peak_output - right_slowdown), 10);
	} else {
		talon_left->ConfigPeakOutputForward(0.25, 10);
	}

	talon_right->Set(ControlMode::Position, target, 10);
	if (right_stop) {
		talon_right->ConfigPeakOutputForward(0.0, 10);
		talon_right->Set(ControlMode::PercentOutput, 0, 10);
	} else if (!left_stop) {
		talon_right->ConfigPeakOutputForward(max_peak_output - right_slowdown, 10);
		talon_right->ConfigPeakOutputReverse(-(max_peak_output - left_slowdown), 10);
	} else {
		talon_right->ConfigPeakOutputForward(0.25, 10);
	}

	if (left_stop && right_stop) {
		target = 0.0;
		talon_left->SetSelectedSensorPosition(0, 0, 10);
		talon_right->SetSelectedSensorPosition(0, 0, 10);
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
