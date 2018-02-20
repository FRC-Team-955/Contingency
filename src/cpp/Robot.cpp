#include "WPILib.h"
#include <ctre/Phoenix.h>
#include <iostream>
#include <common_settings.h>
#include <scissor.h>
#include <diagnostic.h>

class Robot : public IterativeRobot {
	private:
#define TALON(NAME, NUM) TalonSRX* tln_##NAME;
#include <talons.h>
#undef TALON
		ScissorLift *scissor;
		DigitalInput *dio_left, *dio_right;
		Diagnostic *diag;

			void RobotInit() {
				std::cout << "================= Initializing... =================" << std::endl;
#define TALON(NAME, NUM) tln_##NAME = new TalonSRX(NUM); \
				tln_##NAME->ConfigPeakOutputForward(1.0, talon_timeout_ms); \
				tln_##NAME->ConfigPeakOutputReverse(-1.0, talon_timeout_ms); \
				tln_##NAME->SelectProfileSlot(0, talon_pid_loop_idx); \
				tln_##NAME->SetSelectedSensorPosition(0.0, talon_pid_loop_idx, talon_timeout_ms); \
				std::cout << "Talon " << #NAME << " initialized with number " << NUM << std::endl;
#include <talons.h>
#undef TALON
				dio_left = new DigitalInput(dio_idx_scissor_left);
				dio_right = new DigitalInput(dio_idx_scissor_right);
				scissor = new ScissorLift(tln_scissor_left_enc, tln_scissor_right_enc, dio_left, dio_right);
				diag = new Diagnostic();
				std::cout << "============ Initialization complete. ============" << std::endl;
			}

		void TeleopInit() {
			
		}

		void TeleopPeriodic() {
		}
};

START_ROBOT_CLASS(Robot);
