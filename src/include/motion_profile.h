#ifndef MOTION_PROFILE_H
#define MOTION_PROFILE_H

#include "WPILib.h"
#include <ctre/Phoenix.h>
#include <socket.h>
#include <shared_network_types.h>

//TODO: Use (e)poll() for socket connection (non-blocking)

class MotionProfile {
	public:
		MotionProfile(TalonSRX* tln_left, TalonSRX* tln_right, Socket* jetson) : tln_left(tln_left), tln_right(tln_right), jetson(jetson) {
			notifier = new Notifier(&MotionProfile::update, this);
		}
		~MotionProfile();
		void start(ControlMode mode, float scalar);
		void stop();
		void print_inputs();
	private:	
		TalonSRX *tln_left, *tln_right;
		void update();
		Notifier* notifier;
		Socket* jetson;
		ControlMode mode;
		float scalar;
		RobotCommand command;
		bool run_profile = false;
		const float interval = 10.0; //ms
};

#endif
