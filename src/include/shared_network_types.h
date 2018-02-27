#ifndef SHARED_NETWORK_TYPES_H
#define SHARED_NETWORK_TYPES_H

struct TankDriveMotionUnit {
	float position_left;
	float velocity_left;
	float position_right;
	float velocity_right;
	float delta_time;
};

enum Action {
	Scissor_UP,		
	Scissor_AND_EXPEL,		
	Scissor_DOWN,		
};

struct RobotCommand {
	TankDriveMotionUnit motion;
	Action action;
};

#endif
