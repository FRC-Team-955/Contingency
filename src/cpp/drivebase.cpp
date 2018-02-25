#include <drivebase.h>

DriveBase::DriveBase(Joystick *joy, TalonSRX *tln_left, TalonSRX *tln_right,
                     int x_axis_idx, int y_axis_idx, float x_axis_exponent,
                     float y_axis_exponent, float max_velocity, ControlMode mode)
    : joy(joy), tln_left(tln_left), tln_right(tln_right), x_axis_idx(x_axis_idx),
      y_axis_idx(y_axis_idx), x_axis_exponent(x_axis_exponent),
      y_axis_exponent(y_axis_exponent), max_velocity(max_velocity), mode(mode) {}

void DriveBase::update() {
  float joystick_adj_x = exponent_mirrored(x_axis_exponent, joy->GetRawAxis(x_axis_idx));
  float joystick_adj_y = exponent_mirrored(y_axis_exponent, joy->GetRawAxis(y_axis_idx)); 
  float left_tracks = clamp(-1.0, 1.0, joystick_adj_y - joystick_adj_x);
  float right_tracks = clamp(-1.0, 1.0, joystick_adj_y + joystick_adj_x);
  tln_left->Set(mode, left_tracks * max_velocity);
  tln_right->Set(mode, right_tracks * max_velocity);
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
