#ifndef COMMON_SETTINGS_H
#define COMMON_SETTINGS_H

// Controls
const int joy_idx = 0;

const int drive_x_axis_idx = 1;
const int drive_y_axis_idx = 4;

const float drive_x_axis_exponent = 3.0;
const float drive_y_axis_exponent = 2.0;

// Talon coeffs
const int talon_pid_loop_idx = 0;
const int talon_timeout_ms = 10;

// Digital io pins
const int dio_idx_scissor_left = 1;
const int dio_idx_scissor_right = 2;

// Speeds
const float intake_speed_in = 0.8;
const float intake_speed_out = -0.8;
const float climb_speed = -1.0;

//Turns out DIO pins float at '1' (Positive) So all of our limit switches are Normally Open and put 0 (Negative) when closed.
enum LimitSwitchState {
	Closed = 0,
	Open = 1,
};

// Solenoid
const int solenoid_shifter_idx = 1;

// Limits
const float scissorlift_one_rotation_nu = 4096.0;
const float scissorlift_maximum_height_nu = scissorlift_one_rotation_nu * 5.0 * 7.5;

// Scissor lift coeffs
const float scissorlift_p_gain = 1.16;
const float scissorlift_max_speed = 1.0;

#endif
