#ifndef ADAPTIVE_F_GAIN_H
#define ADAPTIVE_F_GAIN_H

#include "WPILib.h"
#include <ctre/Phoenix.h>
#include <Joystick.h>

namespace AdaptiveFGain {
	void AdaptiveControl(TalonSRX* talon, float (*map_velocity_to_fgain)(float))
};

#endif
