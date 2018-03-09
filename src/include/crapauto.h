#ifndef CRAPAUTO_H
#define CRAPAUTO_H

#include "WPILib.h"
#include <ctre/Phoenix.h>

class CrapAuto {
	public:
		CrapAuto(TalonSRX* left, TalonSRX* right, float speed, float duration) : left(left), right(right), speed(speed), duration(duration) {}
		void start();
		void update();
	private:
		TalonSRX* left, *right;
		Timer timer;
		float speed;
		float duration;
};

#endif
