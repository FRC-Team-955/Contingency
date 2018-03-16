#ifndef CRAPAUTO_H
#define CRAPAUTO_H

#include "WPILib.h"
#include <ctre/Phoenix.h>

class CrapAuto {
	public:
		CrapAuto(TalonSRX* left,
				TalonSRX* right,
				Solenoid* shifter_a,
				Solenoid* shifter_b,
				float speed,
				float duration) : left(left),
		right(right),
		speed(speed),
		shifter_a(shifter_a),
		shifter_b(shifter_b),
		duration(duration) {}
		void start();
		void update();
	private:
		TalonSRX* left, *right;
		Solenoid* shifter_a;
		Solenoid* shifter_b;
		Timer timer;
		float speed;
		float duration;
};

#endif
