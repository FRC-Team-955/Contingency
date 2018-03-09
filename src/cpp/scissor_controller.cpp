#include <scissor_controller.h>

void ScissorLiftController::start(float sync_p_gain, float max_peak_output) {
	last_joystick_pov = -1;
	scissor->start_loop(sync_p_gain, max_peak_output);
}

void ScissorLiftController::update() {
	int current = joy->GetPOV(0);
	bool go_up = joy->GetRawAxis(3) > 0.2;
	bool go_dn = joy->GetRawAxis(2) > 0.2;
	if (go_up && reset_up) {
		reset_up = false;
		scissor->set_position(scissor->get_target() - position_delta);
	} else if (go_dn && reset_dn) {
		reset_dn = false;
		scissor->set_position(scissor->get_target() + position_delta);
	}

	if (!go_up && !go_dn) {
		reset_dn = true;
		reset_up = true;
	}

}

void ScissorLiftController::stop() {
	scissor->stop_loop();
}
