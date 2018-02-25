#ifndef LIMIT_SWITCH_TEST_H
#define LIMIT_SWITCH_TEST_H

#include <WPILib.h>
#include <test.h>
#include <ctre/Phoenix.h>
#include <iostream>

class LimitSwitchTest : public Test {
	public:
		LimitSwitchTest (DigitalInput* lm_switch) : lm_switch(lm_switch) {};
		void begin ();
		bool update ();
	private:
		DigitalInput* lm_switch;
		Timer timer;
};

#endif
