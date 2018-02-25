#include "WPILib.h"
#include <ctre/Phoenix.h>
#include <Joystick.h>

#include <iostream>

#include <common_settings.h>
#include <scissor.h>
#include <diagnostic.h>
#include <drivebase.h>

const float one_rotation = 4096.0;

//TODO: Add diagnostics (Esp. to check if the motor moves the correct direction when powered a certain way)
//           Limit switch diagnostic: Instruct user to click LS, wait 5 seconds for it... See if it is clicked.
//      FPID Tuning with the Smart Dashboard
//      Scissor lift clean up
//      Path planning & jetson
//      Manual motor control mode

class Robot : public IterativeRobot {
	private:
		//Talon object pointers macro hack
#define TALON(NAME, NUM) TalonSRX* tln_##NAME;
#include <talons.h>
#undef TALON
		ScissorLift *scissor;
		DigitalInput *dio_left, *dio_right;
		Diagnostic *diag;
		DriveBase *drive_base;
		Joystick *joy;
		//Solenoid *drive_base_shifter;

		void FPID() {
			tln_scissor_left_enc->Config_kF(talon_pid_loop_idx, std::atof(SmartDashboard::GetString("DB/String 0", "0.0").c_str()), talon_timeout_ms);
			tln_scissor_left_enc->Config_kP(talon_pid_loop_idx, std::atof(SmartDashboard::GetString("DB/String 1", "0.0").c_str()), talon_timeout_ms);
			tln_scissor_left_enc->Config_kI(talon_pid_loop_idx, std::atof(SmartDashboard::GetString("DB/String 2", "0.0").c_str()), talon_timeout_ms);
			tln_scissor_left_enc->Config_kD(talon_pid_loop_idx, std::atof(SmartDashboard::GetString("DB/String 3", "0.0").c_str()), talon_timeout_ms);

			tln_scissor_right_enc->Config_kF(talon_pid_loop_idx, std::atof(SmartDashboard::GetString("DB/String 5", "0.0").c_str()), talon_timeout_ms);
			tln_scissor_right_enc->Config_kP(talon_pid_loop_idx, std::atof(SmartDashboard::GetString("DB/String 6", "0.0").c_str()), talon_timeout_ms);
			tln_scissor_right_enc->Config_kI(talon_pid_loop_idx, std::atof(SmartDashboard::GetString("DB/String 7", "0.0").c_str()), talon_timeout_ms);
			tln_scissor_right_enc->Config_kD(talon_pid_loop_idx, std::atof(SmartDashboard::GetString("DB/String 8", "0.0").c_str()), talon_timeout_ms);
		}


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
			tln_drbase_left_fol->Set(ControlMode::Follower, tln_drbase_left_enc->GetDeviceID());
			tln_drbase_right_fol->Set(ControlMode::Follower, tln_drbase_right_enc->GetDeviceID());
			tln_drbase_left_enc->SetSensorPhase(true);
			tln_drbase_right_enc->SetSensorPhase(true);

			dio_left = new DigitalInput(dio_idx_scissor_left);
			dio_right = new DigitalInput(dio_idx_scissor_right);

			diag = new Diagnostic();

			joy = new Joystick(joy_idx);

			scissor = new ScissorLift(tln_scissor_left_enc, tln_scissor_right_enc, dio_left, dio_right);

			//drive_base_shifter = new Solenoid(solenoid_shifter_idx);

			drive_base = new DriveBase(
					joy,
					tln_drbase_left_enc,
					tln_drbase_right_enc,
					drive_x_axis_idx,
					drive_y_axis_idx,
					drive_x_axis_exponent,
					drive_y_axis_exponent,
					-1.0, ControlMode::PercentOutput);

			std::cout << "============ Initialization complete. ============" << std::endl;
		}

		void DisabledInit() {
			scissor->stop_loop();
		}

		bool reverse = true;
		int last = -1;
		void TeleopInit() {
			reverse = !reverse;
			//drive_base_shifter->Set(reverse);
			
			last = -1;
			//scissor->start_loop(
			//		SmartDashboard::GetNumber("DB/Slider 0", 0.0) / one_rotation,
			//		SmartDashboard::GetNumber("DB/Slider 1", 0.0)
			//		);
		}

		float delta = one_rotation;
		float pos = 0.0;
		void TeleopPeriodic() {
			int current = joy->GetPOV(0);
			if (last == -1) {
				switch (current) {
					case 0:
						pos -= delta;
						scissor->set_position(pos);
						break;
					case 180:
						pos += delta;
						scissor->set_position(pos);
						break;
					default:
						break;
				}
			}
			last = current;
			drive_base->update();
			std::cout << scissor->get_sync_error();
		}

		bool display_results_once = false;
		void TestInit() {
			display_results_once = false;
			diag->reset();
			//diag->push_diagnostic();
			diag->start();
		}

		void TestPeriodic() {
			if (!display_results_once && !diag->control()) {
				diag->results();
				diag->reset();
				display_results_once = true;
			}
		}
};

START_ROBOT_CLASS(Robot);
