#ifndef ENCODED_SRX_TEST_H
#define ENCODED_SRX_TEST_H

#include <WPILib.h>
#include <test.h>
#include <ctre/Phoenix.h>
#include <iostream>

class EncodedSRXTest : public Test {
	public:
		EncodedSRXTest (TalonSRX* talon) : talon(talon) {};
		void begin ();
		bool update ();
	private:
		TalonSRX* talon;
		Timer timer;
};

#endif
