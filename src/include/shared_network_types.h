#ifndef SHARED_NETWORK_TYPES_H
#define SHARED_NETWORK_TYPES_H

struct TankDriveMotionUnit {
	float position_left;
	float velocity_left;
	float position_right;
	float velocity_right;
};

enum Action {
	None = 0,
	Scissor_Switch = 1,
	Scissor_Scale = 2,
	Cube_Expel = 4,
	Cube_Intake = 8,
};

struct RioCommand {
	enum Type {
		RioNone,
		Stop,
		Motion,
		Request_Setup,
	} type;
	TankDriveMotionUnit motion;
	Action action;
};

struct JetsonCommand {
	enum Type {
		JetsonNone,
		Reset,
		Request_Motion,
	} type;
	char config[3];
};

#endif
