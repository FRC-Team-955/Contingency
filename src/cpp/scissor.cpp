#include <scissor.h>

//TODO: Don't hardcode anything
void ScissorLift::start_loop(float sync_p_gain, float max_peak_output) {
	this->sync_p_gain = sync_p_gain;
	this->max_peak_output = max_peak_output;
	stopped = false;
	notifier->StartPeriodic(0.01); 
}

void ScissorLift::stop_loop() {
	//notifier->Stop();
	stopped = true;
}

void ScissorLift::run_loop() {
	float position_sync_error = get_sync_error();
	if (stopped) {
		notifier->Stop();
	}

	//TODO: Document how this works
	float left_slowdown = clamp_max(sync_p_gain * (talon_left->GetSelectedSensorPosition(0) - talon_right->GetSelectedSensorPosition(0)));
	float right_slowdown = clamp_max(sync_p_gain * (talon_right->GetSelectedSensorPosition(0) - talon_left->GetSelectedSensorPosition(0)));

	talon_left->ConfigPeakOutputForward(max_peak_output - left_slowdown, 10);
	talon_left->ConfigPeakOutputReverse(-(max_peak_output - right_slowdown), 10);
	talon_left->Set(ControlMode::Position, position_to_native(target_inch), 10);

	talon_right->ConfigPeakOutputForward(max_peak_output - right_slowdown, 10);
	talon_right->ConfigPeakOutputReverse(-(max_peak_output - left_slowdown), 10);
	talon_right->Set(ControlMode::Position, position_to_native(target_inch), 10);
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

float ScissorLift::position_to_native(float position) {
	//return scissor_arm_length - (scissor_arm_length * cos(asin(position / (scissor_arm_length * rotations_per_inch))));
	return position;
}

float ScissorLift::get_sync_error() {
	return talon_left->GetSelectedSensorPosition(0) - talon_right->GetSelectedSensorPosition(0);
}

void ScissorLift::set_position(float target_inch) {
	this->target_inch = target_inch;
}
