#include <drivebase.h>

DriveBase::DriveBase(Joystick *joy,
		TalonSRX *tln_left,
		TalonSRX *tln_right,
		Solenoid* shifter_a,
		Solenoid* shifter_b,
		int x_axis_idx,
		int y_axis_idx,
		float x_axis_exponent,
		float y_axis_exponent,
		float max_velocity,
		int shift_counts_max,
		ControlMode mode)
	: joy(joy),
	tln_left(tln_left),
	tln_right(tln_right),
	x_axis_idx(x_axis_idx),
	y_axis_idx(y_axis_idx),
	x_axis_exponent(x_axis_exponent),
	y_axis_exponent(y_axis_exponent),
	max_velocity(max_velocity),
	shifter_a(shifter_a),
	shifter_b(shifter_b), 
	shift_counts_max(shift_counts_max),
	shift_counts(shift_counts_max),
	mode(mode) {}

	void DriveBase::reset_shift_count() {
		shift_counts = shift_counts_max;
	}

	void DriveBase::update() {
		int pov = joy->GetPOV();
		if (pov != pov_last) {
			switch (pov) {
				case 0:
					reverse = false;
					break;
				case 180:
					reverse = true;
					break;
				case 90:
					if (shift_counts > 0) {
						shifter_a->Set(true);
						shifter_b->Set(false);
						shift_counts--;
					}
					std::cout << shift_counts << " shifts left." << std::endl;
					break;
				case 270:
					if (shift_counts > 0) {
						shifter_a->Set(false);
						shifter_b->Set(true);
						shift_counts--;
					}
					std::cout << shift_counts << " shifts left." << std::endl;
					break;
			}
		}
		float joystick_adj_x = exponent_mirrored(x_axis_exponent, joy->GetRawAxis(x_axis_idx));
		float joystick_adj_y = exponent_mirrored(y_axis_exponent, joy->GetRawAxis(y_axis_idx)); 
		float left_tracks = clamp(-1.0, 1.0, joystick_adj_y - joystick_adj_x);
		float right_tracks = clamp(-1.0, 1.0, joystick_adj_y + joystick_adj_x);
		if (reverse) {
			tln_left->Set(mode, -right_tracks * max_velocity);
			tln_right->Set(mode, -left_tracks * max_velocity);
		} else {
			float velocity_left = left_tracks * max_velocity;
			float velocity_right = right_tracks * max_velocity;
			std::cout << max_velocity << std::endl;

			//float fgain_left = (((fabs(velocity_left) * 0.000212572541) + 0.2377763902) * 1023.0) / fabs(velocity_left);
			//float fgain_right = (((fabs(velocity_right) * 0.000212572541) + 0.2377763902) * 1023.0) / fabs(velocity_right);

			//tln_left->Config_kF(0, std::min(fgain_left, 4.0f), 10);
			//tln_right->Config_kF(0, std::min(fgain_right, 4.0f), 10);
			//std::cout << fgain_left << " : " << fgain_right << std::endl;

			tln_left->Set(mode, velocity_left);
			tln_right->Set(mode, velocity_right);
		}
		pov_last = pov;
	}

float DriveBase::clamp(float min, float max, float input) {
	if (input > max) {
		return max;
	}
	if (input < min) {
		return min;
	}
	return input;
}

float DriveBase::exponent_mirrored(float exponent, float input) {
	return pow(fabs(input), exponent) * (input > 0 ? 1.0 : -1.0);
}
