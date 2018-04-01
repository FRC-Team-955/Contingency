#include "WPILib.h"
#include <ctre/Phoenix.h>
#include <Joystick.h>

#include <iostream>
#include <fstream>
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

		CrapAuto* crap;

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
			tln_drbase_left_enc->ConfigClosedloopRamp(0.05, 10);
			tln_drbase_right_enc->ConfigClosedloopRamp(0.05, 10);
			tln_drbase_left_fol->Set(ControlMode::Follower, tln_drbase_left_enc->GetDeviceID());
			tln_drbase_right_fol->Set(ControlMode::Follower, tln_drbase_right_enc->GetDeviceID());

			tln_climb_fol->Set(ControlMode::Follower, tln_climb_enc->GetDeviceID());

			tln_scissor_left_enc->ConfigSelectedFeedbackSensor(FeedbackDevice::CTRE_MagEncoder_Relative, 0, 10);
			tln_scissor_right_enc->ConfigSelectedFeedbackSensor(FeedbackDevice::CTRE_MagEncoder_Relative, 0, 10);
			tln_drbase_left_enc->ConfigSelectedFeedbackSensor(FeedbackDevice::QuadEncoder, 0, 10);
			tln_drbase_right_enc->ConfigSelectedFeedbackSensor(FeedbackDevice::QuadEncoder, 0, 10);

			tln_drbase_left_enc->SetSensorPhase(true);
			tln_drbase_right_enc->SetSensorPhase(true);

			dio_left = new DigitalInput(dio_idx_scissor_left);
			dio_right = new DigitalInput(dio_idx_scissor_right);

			diag = new Diagnostic();

			joy = new Joystick(joy_idx);

			scissor = new ScissorLift(tln_scissor_left_enc, tln_scissor_right_enc, dio_left, dio_right);
			scissor_control = new ScissorLiftController(
					joy,
					scissor,
					//scissorlift_one_rotation_nu * 3.0);
					scissorlift_one_rotation_nu / 10);

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
					//-5000.0, 
					-1.0,
					shift_counts_max,
					//ControlMode::Velocity);
					ControlMode::PercentOutput);
			
			crap = new CrapAuto(
					tln_drbase_left_enc,
					tln_drbase_right_enc,
					drive_base_shifter_a,
					drive_base_shifter_b,
					0.5,
					1.6);


			//TODO: Non-blocking!
			//      Change to static IPs!!

			std::cout << "============ Initialization complete. ============" << std::endl;
		}

		void DisabledInit() {
			std::cout << tln_drbase_left_enc->GetSelectedSensorPosition(0) / 1600.0 << " : " << tln_drbase_right_enc->GetSelectedSensorPosition(0) / 1600.0 << std::endl;
			scissor_control->stop();
			if (profile) {
				profile->stop();
			}
		}

		void TeleopInit() {
			if (profile) {
				profile->stop();
			}
			drive_base_control->reset_shift_count();	
			tln_drbase_left_enc->ConfigPeakOutputForward(1.0, talon_timeout_ms);
			tln_drbase_left_enc->ConfigPeakOutputReverse(-1.0, talon_timeout_ms);
			tln_drbase_right_enc->ConfigPeakOutputForward(1.0, talon_timeout_ms);
			tln_drbase_right_enc->ConfigPeakOutputReverse(-1.0, talon_timeout_ms);

			tln_drbase_left_enc->SetSelectedSensorPosition(0, 0, 10);
			tln_drbase_right_enc->SetSelectedSensorPosition(0, 0, 10);

			scissor_control->start(scissorlift_p_gain, scissorlift_max_speed);
		}

		float pos = 0.0;
		void TeleopPeriodic() {
			drive_base_control->update();
			scissor_control->update();

			std::cout << tln_drbase_left_enc->GetClosedLoopError(0) << " : " << tln_drbase_right_enc->GetClosedLoopError(0) << std::endl;
			//std::cout << tln_drbase_left_enc->GetSelectedSensorVelocity(0) << " : " << tln_drbase_right_enc->GetSelectedSensorVelocity(0) << std::endl;

			if (joy->GetRawButton(4)) {
				tln_climb_enc->Set(ControlMode::PercentOutput, climb_speed);	
			} else {
				tln_climb_enc->Set(ControlMode::PercentOutput, 0);	
			}

			if (joy->GetRawButton(6)) {
				//drive_base_control->max_velocity = -0.5;
				tln_intake_left->Set(ControlMode::PercentOutput, intake_speed_in);	
				tln_intake_right->Set(ControlMode::PercentOutput, intake_speed_in);	
			} else if (joy->GetRawButton(5)) {
				//drive_base_control->max_velocity = -0.5;
				tln_intake_left->Set(ControlMode::PercentOutput, intake_speed_out);	
				tln_intake_right->Set(ControlMode::PercentOutput, intake_speed_out);	
			} else {
				//drive_base_control->max_velocity = -1.0;
				tln_intake_left->Set(ControlMode::PercentOutput, 0);	
				tln_intake_right->Set(ControlMode::PercentOutput, 0);	
			}

		}

		void AutonomousInit() {

			std::cout << "Connecting to Jetson" << std::endl;
			jetson = new SocketClient (5801, (char*)"10.9.55.6");
			std::cout << "Connected!" << std::endl;

			profile = new MotionProfile(
					tln_drbase_left_enc,
					tln_drbase_right_enc,
					tln_intake_left,
					tln_intake_right,
					scissor,
					jetson,
					ControlMode::Velocity,
					//(4096.0 * 100.0) / (2.0 * 0.728 * 104.775 * M_PI));
					  1600.0);

			tln_drbase_left_enc->ConfigClosedloopRamp(0.0, 10);
			tln_drbase_right_enc->ConfigClosedloopRamp(0.0, 10);

			tln_drbase_left_enc->SetSelectedSensorPosition(0, 0, 10);
			tln_drbase_right_enc->SetSelectedSensorPosition(0, 0, 10);

			scissor->start_loop(scissorlift_p_gain, scissorlift_max_speed);
			std::string game_data;
			int loc = DriverStation::GetInstance().GetLocation();
			for (int i = 0; i < 100; i++) {
				loc = DriverStation::GetInstance().GetLocation();
				game_data = DriverStation::GetInstance().GetGameSpecificMessage();
			}
			game_data.erase(game_data.end() - 1); //Screw the last char, we don't need to know
			game_data.append(std::to_string(loc)); //Add the positon as the last
			std::cout << "GAME DATA: " << game_data << std::endl;

			profile->start((char*)game_data.c_str());
			//crap->start();
		}

		void AutonomousPeriodic() {
			std::cout << tln_drbase_left_enc->GetClosedLoopError(0) << " : " << tln_drbase_right_enc->GetClosedLoopError(0) << std::endl;
			//crap->update();
		}

		bool display_results_once = false;
		void TestInit() {
			std::cout << "TEST INIT" << std::endl;
			tln_scissor_left_enc->ConfigPeakOutputForward(1.0, 10);
			tln_scissor_left_enc->ConfigPeakOutputReverse(-1.0, 10);
			tln_scissor_right_enc->ConfigPeakOutputForward(1.0, 10);
			tln_scissor_right_enc->ConfigPeakOutputReverse(-1.0, 10);
		}

		void TestPeriodic() {
			tln_scissor_left_enc->Set(ControlMode::PercentOutput, joy->GetRawAxis(1));
			tln_scissor_right_enc->Set(ControlMode::PercentOutput, joy->GetRawAxis(5));
			std::cout << tln_scissor_left_enc->GetSelectedSensorPosition(0) << " : " <<
				tln_scissor_right_enc->GetSelectedSensorPosition(0) << std::endl;
		}
};

START_ROBOT_CLASS(Robot);
