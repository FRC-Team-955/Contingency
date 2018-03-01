#ifndef SCISSOR_HOME_TEST_H
#define SCISSOR_HOME_TEST_H

//TODO: Make this check the motors and their polarities too

#include <WPILib.h>
#include <test.h>
#include <ctre/Phoenix.h>
#include <iostream>
#include <scissor.h>

class ScissorHomeTest : public Test {
	public:
		ScissorHomeTest (ScissorLift* scissor, float speed) : scissor(scissor), speed(speed) {};
		void begin ();
		bool update ();
	private:
		float speed;
		ScissorLift* scissor;
		Timer timer;
};

#endif
