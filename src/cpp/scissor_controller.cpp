#include <scissor_controller.h>

void ScissorLiftController::start(float sync_p_gain, float max_peak_output) {
	last_joystick_pov = -1;
	scissor->start_loop(sync_p_gain, max_peak_output);
}

void ScissorLiftController::update() {
	scissor->update();
	int current = joy->GetPOV(0);
	if (last_joystick_pov == -1) {
		switch (current) {
			//TODO: Make it more apparent why negative is up
			case 0: //Up
				if (scissor->get_target() < scissorlift_maximum_height_nu) {
					scissor->set_position(scissor->get_target() - position_delta);
				}
				break;
			case 180: //Down
				scissor->set_position(scissor->get_target() + position_delta);
				break;
			default:
				break;
		}
	}
	last_joystick_pov = current;
}

void ScissorLiftController::stop() {
	scissor->stop_loop();
}
