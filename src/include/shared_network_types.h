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
	Scissor_Up    =  1,
	Scissor_Down  =  2,
	Cube_Expel    =  4,
	Cube_Intake   =  8,
};

struct RobotCommand {
	TankDriveMotionUnit motion;
	Action action;
};

#endif
