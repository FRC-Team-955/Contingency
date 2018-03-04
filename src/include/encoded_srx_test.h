#ifndef ENCODED_SRX_TEST_H
#define ENCODED_SRX_TEST_H

#include <WPILib.h>
#include <test.h>
#include <ctre/Phoenix.h>
#include <iostream>

//TODO: Shorten this by using srx_test.h internally?
class EncodedSRXTest : public Test {
	public:
		EncodedSRXTest  (TalonSRX* talon, float duration, float speed) : talon(talon), duration(duration), speed(speed) {};
		void begin ();
		bool update ();
	private:
		float duration, speed;
		TalonSRX* talon;
		Timer timer;
};

#endif
