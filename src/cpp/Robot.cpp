#include "WPILib.h"
#include <ctre/Phoenix.h>
#include <Joystick.h>

#include <iostream>
#include <math.h>

#include <common_settings.h>
#include <scissor_controller.h>

#include <diagnostic.h>
#include <limit_switch_test.h>
#include <encoded_srx_test.h>
#include <scissor_home_test.h>
#include <srx_test.h>

#include <drivebase.h>
#include <socket.h>
#include <shared_network_types.h>
#include <motion_profile.h>
#include <crapauto.h>

//TODO: 
//      FPID Tuning with the Smart Dashboard
//      Scissor lift clean up
//      Path planning & jetson
//      Use poll() or epoll() for socket connection and SOCK_NONBLOCK
//            Use a state machine to manage the socket and it's connection...?
//            Only use async and polling + sockfd r/w

class Robot : public IterativeRobot {
	private:
		// I/O
#define TALON(NAME, NUM) TalonSRX* tln_##NAME;
#include <talons.h>
#undef TALON
		DigitalInput *dio_left, *dio_right;
		Joystick *joy;
		Solenoid *drive_base_shifter_a;
		Solenoid *drive_base_shifter_b;

		// Controller components
		Diagnostic *diag;
		SocketClient *jetson;
		ScissorLift *scissor;

		// Control managers (User control, auto)
		DriveBase *drive_base_control;
		ScissorLiftController* scissor_control;
		MotionProfile* profile;

		CrapAuto* crap_auto;

		void FPID() {
			//TODO: Hardcoded PIDF
			tln_scissor_left_enc->Config_kF(talon_pid_loop_idx, 0, talon_timeout_ms);
			tln_scissor_left_enc->Config_kP(talon_pid_loop_idx, 1, talon_timeout_ms);
			tln_scissor_left_enc->Config_kI(talon_pid_loop_idx, 0, talon_timeout_ms);
			tln_scissor_left_enc->Config_kD(talon_pid_loop_idx, 10, talon_timeout_ms);

			tln_scissor_right_enc->Config_kF(talon_pid_loop_idx, 0, talon_timeout_ms);
			tln_scissor_right_enc->Config_kP(talon_pid_loop_idx, 1, talon_timeout_ms);
			tln_scissor_right_enc->Config_kI(talon_pid_loop_idx, 0, talon_timeout_ms);
			tln_scissor_right_enc->Config_kD(talon_pid_loop_idx, 10, talon_timeout_ms);

			tln_drbase_left_enc->Config_kF(talon_pid_loop_idx, drbase_f_gain_left, talon_timeout_ms);
			//tln_drbase_left_enc->Config_kP(talon_pid_loop_idx, std::atof(SmartDashboard::GetString("DB/String 1", "0.0").c_str()), talon_timeout_ms);
			//tln_drbase_left_enc->Config_kI(talon_pid_loop_idx, std::atof(SmartDashboard::GetString("DB/String 2", "0.0").c_str()), talon_timeout_ms);
			//tln_drbase_left_enc->Config_kD(talon_pid_loop_idx, std::atof(SmartDashboard::GetString("DB/String 3", "0.0").c_str()), talon_timeout_ms);

			tln_drbase_right_enc->Config_kF(talon_pid_loop_idx, drbase_f_gain_right, talon_timeout_ms);
			//tln_drbase_right_enc->Config_kP(talon_pid_loop_idx, std::atof(SmartDashboard::GetString("DB/String 6", "0.0").c_str()), talon_timeout_ms);
			//tln_drbase_right_enc->Config_kI(talon_pid_loop_idx, std::atof(SmartDashboard::GetString("DB/String 7", "0.0").c_str()), talon_timeout_ms);
			//tln_drbase_right_enc->Config_kD(talon_pid_loop_idx, std::atof(SmartDashboard::GetString("DB/String 8", "0.0").c_str()), talon_timeout_ms);

			//tln_drbase_left_enc->Config_kF(talon_pid_loop_idx, std::atof(SmartDashboard::GetString("DB/String 0", "0.0").c_str()), talon_timeout_ms);
			tln_drbase_left_enc->Config_kP(talon_pid_loop_idx, std::atof(SmartDashboard::GetString("DB/String 1", "0.0").c_str()), talon_timeout_ms);
			tln_drbase_left_enc->Config_kI(talon_pid_loop_idx, std::atof(SmartDashboard::GetString("DB/String 2", "0.0").c_str()), talon_timeout_ms);
			tln_drbase_left_enc->Config_kD(talon_pid_loop_idx, std::atof(SmartDashboard::GetString("DB/String 3", "0.0").c_str()), talon_timeout_ms);

			//tln_drbase_right_enc->Config_kF(talon_pid_loop_idx, std::atof(SmartDashboard::GetString("DB/String 5", "0.0").c_str()), talon_timeout_ms);
			tln_drbase_right_enc->Config_kP(talon_pid_loop_idx, std::atof(SmartDashboard::GetString("DB/String 6", "0.0").c_str()), talon_timeout_ms);
			tln_drbase_right_enc->Config_kI(talon_pid_loop_idx, std::atof(SmartDashboard::GetString("DB/String 7", "0.0").c_str()), talon_timeout_ms);
			tln_drbase_right_enc->Config_kD(talon_pid_loop_idx, std::atof(SmartDashboard::GetString("DB/String 8", "0.0").c_str()), talon_timeout_ms);
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

			tln_climb_fol->Set(ControlMode::Follower, tln_climb_enc->GetDeviceID());

			tln_scissor_left_enc->ConfigSelectedFeedbackSensor(FeedbackDevice::CTRE_MagEncoder_Relative, 0, 10);
			tln_scissor_right_enc->ConfigSelectedFeedbackSensor(FeedbackDevice::CTRE_MagEncoder_Relative, 0, 10);
			tln_drbase_left_enc->ConfigSelectedFeedbackSensor(FeedbackDevice::QuadEncoder, 0, 10);
			tln_drbase_right_enc->ConfigSelectedFeedbackSensor(FeedbackDevice::QuadEncoder, 0, 10);

			tln_drbase_left_enc->SetSensorPhase(true);

			dio_left = new DigitalInput(dio_idx_scissor_left);
			dio_right = new DigitalInput(dio_idx_scissor_right);

			diag = new Diagnostic();

			joy = new Joystick(joy_idx);

			scissor = new ScissorLift(tln_scissor_left_enc, tln_scissor_right_enc, dio_left, dio_right);
			scissor_control = new ScissorLiftController(joy, scissor, scissorlift_one_rotation_nu * 3.0);

			drive_base_shifter_a = new Solenoid(solenoid_shifter_idx_a);
			drive_base_shifter_b = new Solenoid(solenoid_shifter_idx_b);

			drive_base_control = new DriveBase(
					joy,
					tln_drbase_left_enc,
					tln_drbase_right_enc,
					drive_base_shifter_a,
					drive_base_shifter_b,
					drive_x_axis_idx,
					drive_y_axis_idx,
					drive_x_axis_exponent,
					drive_y_axis_exponent,
					-6000.0, 
					shift_counts_max,
					ControlMode::Velocity);
					//-1.0, ControlMode::PercentOutput);

			//TODO: Non-blocking!
			//      Change to static IPs!!
			//std::cout << "Connecting to Jetson" << std::endl;
			//jetson = new SocketClient (5801, (char*)"tegra-ubuntu.local");
			//profile = new MotionProfile(tln_drbase_left_enc, tln_drbase_right_enc, jetson, ControlMode::Velocity);
			//std::cout << "Finished." << std::endl;

			crap_auto = new CrapAuto(tln_drbase_left_enc, tln_drbase_right_enc, drive_base_shifter_a, drive_base_shifter_b, 1.0, 1.4);

			std::cout << "============ Initialization complete. ============" << std::endl;
		}

		void DisabledInit() {
			//profile->stop();
			scissor_control->stop();
		}

		void TeleopInit() {
			FPID();
			drive_base_control->reset_shift_count();	
			tln_drbase_left_enc->ConfigPeakOutputForward(1.0, talon_timeout_ms);
			tln_drbase_left_enc->ConfigPeakOutputReverse(-1.0, talon_timeout_ms);
			tln_drbase_right_enc->ConfigPeakOutputForward(1.0, talon_timeout_ms);
			tln_drbase_right_enc->ConfigPeakOutputReverse(-1.0, talon_timeout_ms);
			//scissor_control->start(
			//		SmartDashboard::GetNumber("DB/Slider 0", 0.0) / scissorlift_one_rotation_nu,
			//		SmartDashboard::GetNumber("DB/Slider 1", 0.0)
			//		);
			
			scissor_control->start(scissorlift_p_gain, scissorlift_max_speed);
		}

		float pos = 0.0;
		void TeleopPeriodic() {
			drive_base_control->update();
			scissor_control->update();

			//std::cout << tln_drbase_left_enc->GetClosedLoopError(0) << " : " << tln_drbase_right_enc->GetClosedLoopError(0) << std::endl;
			//std::cout << tln_drbase_left_enc->GetSelectedSensorVelocity(0) << " : " << tln_drbase_right_enc->GetSelectedSensorVelocity(0) << std::endl;

			if (joy->GetRawButton(4)) {
				tln_climb_enc->Set(ControlMode::PercentOutput, climb_speed);	
			} else {
				tln_climb_enc->Set(ControlMode::PercentOutput, 0);	
			}

			if (joy->GetRawButton(6)) {
				tln_intake_left->Set(ControlMode::PercentOutput, intake_speed_in);	
				tln_intake_right->Set(ControlMode::PercentOutput, intake_speed_in);	
			} else if (joy->GetRawButton(5)) {
				tln_intake_left->Set(ControlMode::PercentOutput, intake_speed_out);	
				tln_intake_right->Set(ControlMode::PercentOutput, intake_speed_out);	
			} else {
				tln_intake_left->Set(ControlMode::PercentOutput, 0);	
				tln_intake_right->Set(ControlMode::PercentOutput, 0);	
			}
			
		}

		void AutonomousInit() {
			crap_auto->start();
			//profile->start((4096.0 * 100.0) / (104.775 * M_PI * 0.6));
		}

		void AutonomousPeriodic() {
			crap_auto->update();
			//std::cout << "Inputs: "; profile->print_inputs();
			//std::cout << tln_drbase_left_enc->GetClosedLoopError(0) << " : " << tln_drbase_right_enc->GetClosedLoopError(0) << std::endl;
		}

		bool display_results_once = false;
		void TestInit() {
			display_results_once = false;
			diag->reset();
			diag->push_diagnostic(new SRXTest(tln_intake_left, 1.0, intake_speed_in));
			diag->push_diagnostic(new SRXTest(tln_intake_right, 1.0, intake_speed_in));

			diag->push_diagnostic(new EncodedSRXTest(tln_drbase_left_enc, 2.0, 0.25));
			diag->push_diagnostic(new EncodedSRXTest(tln_drbase_right_enc, 2.0, 0.25));

			auto lm_test_left = new LimitSwitchTest(dio_left);
			auto lm_test_right = new LimitSwitchTest(dio_right);
			lm_test_left->subtests.push_back(lm_test_right);
			auto scissor_home = new ScissorHomeTest(scissor, 0.25);
			lm_test_right->subtests.push_back(scissor_home);
			
			diag->push_diagnostic(lm_test_left);
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
