#ifndef SRX_TEST_H
#define SRX_TEST_H

#include <WPILib.h>
#include <test.h>
#include <ctre/Phoenix.h>
#include <iostream>

class SRXTest : public Test {
	public:
		SRXTest (TalonSRX* talon, float duration, float speed) : talon(talon), duration(duration), speed(speed) {};
		void begin ();
		bool update ();
	private:
		float duration, speed;
		TalonSRX* talon;
		Timer timer;
};

#endif
