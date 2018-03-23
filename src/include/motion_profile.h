#ifndef MOTION_PROFILE_H
#define MOTION_PROFILE_H

#include "WPILib.h"
#include <ctre/Phoenix.h>
#include <shared_network_types.h>
#include <socket.h>

// TODO: Use (e)poll() for socket connection (non-blocking)

class MotionProfile {
	public:
		MotionProfile(
				TalonSRX *tln_drbase_left,
				TalonSRX *tln_drbase_right,
				TalonSRX *tln_intake_left,
				TalonSRX *tln_intake_right,
				Socket *jetson,
				ControlMode mode,
				JetsonCommand setup,
				float scalar) : 
				tln_drbase_left(tln_drbase_left),
				tln_drbase_right(tln_drbase_right),
				tln_intake_left(tln_intake_left),
				tln_intake_right(tln_intake_right),
				jetson(jetson),
				mode(mode),
				setup(setup),
				scalar(scalar),
				notifier(&MotionProfile::update, this) {}

		~MotionProfile();
		void start(JetsonCommand::Setup::LayoutBits bits);
		void stop();
		bool is_running();

	private:
		TalonSRX *tln_drbase_left, *tln_drbase_right, *tln_intake_left, *tln_intake_right;
		void update();
		Notifier notifier;
		Socket *jetson;
		ControlMode mode;
		float scalar;
		JetsonCommand setup;
		bool run_profile = false;
		const float interval = 10.0; // ms
};

#endif
