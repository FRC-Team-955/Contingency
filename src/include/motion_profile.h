#ifndef MOTION_PROFILE_H
#define MOTION_PROFILE_H

#include "WPILib.h"
#include <ctre/Phoenix.h>
#include <shared_network_types.h>
#include <socket.h>
#include <scissor.h>

// TODO: Use (e)poll() for socket connection (non-blocking)

class MotionProfile {
	public:
		MotionProfile(
				TalonSRX *tln_drbase_left,
				TalonSRX *tln_drbase_right,
				TalonSRX *tln_intake_left,
				TalonSRX *tln_intake_right,
				ScissorLift* scissor,
				Socket *jetson,
				ControlMode mode,
				float scalar) : 
			tln_drbase_left(tln_drbase_left),
			tln_drbase_right(tln_drbase_right),
			tln_intake_left(tln_intake_left),
			tln_intake_right(tln_intake_right),
			scissor(scissor),
			jetson(jetson),
			mode(mode),
			scalar(scalar),
			notifier(&MotionProfile::update, this) {}

		~MotionProfile();
		void start(char* layout);
		void stop();
		bool is_running();

	private:
		TalonSRX *tln_drbase_left, *tln_drbase_right, *tln_intake_left, *tln_intake_right;
		void update();
		Notifier notifier;
		Socket *jetson;
		ScissorLift *scissor;
		ControlMode mode;
		float scalar;
		JetsonCommand setup;
		bool run_profile = false;
		const float interval = 10.0; // ms
};

#endif
