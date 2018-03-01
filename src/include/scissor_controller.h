#ifndef SCISSOR_CONTROLLER
#define SCISSOR_CONTROLLER

#include <Joystick.h>
#include <scissor.h>

class ScissorLiftController {
	public:
		ScissorLiftController(Joystick* joy, ScissorLift* scissor, float position_delta) : joy(joy), scissor(scissor), position_delta(position_delta) {};
		void start(float sync_p_gain, float max_peak_output);
		void update();
		void stop();

	private:
		int last_joystick_pov = -1;
		float position_delta;
		Joystick* joy; 
		ScissorLift* scissor;
};

#endif
