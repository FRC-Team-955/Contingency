#ifndef DRIVEBASE_H
#define DRIVEBASE_H

#include "WPILib.h"
#include <ctre/Phoenix.h>
#include <Joystick.h>
#include <iostream>

class DriveBase {
	public:
		DriveBase(Joystick* joy,
				TalonSRX* tln_left,
				TalonSRX* tln_right,
				Solenoid* shifter_a,
				Solenoid* shifter_b,
				int x_axis_idx,
				int y_axis_idx,
				float x_axis_exponent,
				float y_axis_exponent,
				float max_velocity,
				int shift_counts_max,
				ControlMode mode);

		void update();
		void reset_shift_count();
	private:
		ControlMode mode;

		Joystick* joy;
		TalonSRX *tln_left, *tln_right;
		Solenoid *shifter_a;
		Solenoid *shifter_b;

		bool reverse = false;
		int pov_last = -1;

		float max_velocity;
		int shift_counts_max;
		int shift_counts;

		float exponent_mirrored(float exponent, float input);
		float clamp(float min, float max, float input);

		int x_axis_idx;
		int y_axis_idx;

		float x_axis_exponent;
		float y_axis_exponent;
};

#endif
