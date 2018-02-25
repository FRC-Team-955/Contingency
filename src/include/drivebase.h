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
				int x_axis_idx,
				int y_axis_idx,
				float x_axis_exponent,
				float y_axis_exponent,
				float max_velocity,
				ControlMode mode);

		void update();
	private:
		ControlMode mode;

		Joystick* joy;
		TalonSRX *tln_left, *tln_right;

		float max_velocity;

		float exponent_mirrored(float exponent, float input);
		float clamp(float min, float max, float input);

		int x_axis_idx;
		int y_axis_idx;

		float x_axis_exponent;
		float y_axis_exponent;
};

#endif
