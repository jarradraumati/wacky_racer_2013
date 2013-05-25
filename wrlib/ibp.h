#ifndef IBP_H_
#define IBP_H_

#define I2C_CAMERA_ADDR 0x32
#define I2C_COMMS_ADDR	0X23
#define I2C_MOTOR_ADDR	0x04

#define IBP_I2C_TIMEOUT 1000

typedef uint8_t ibp_command;
typedef uint8_t ibp_info;


enum
{
	// Addresses of public data held by the motor board
	MD_SPEED, MD_FAULT, MD_CHARGE, MD_DIRECTION,
	// Address of public data held by the camera board
	CD_PHOTO_READY, CD_PHOTO_LINE, CD_PHOTO_NEXT_LINE, CD_FAULT,
	// Commands for the motor board
	MC_LEFT, MC_RIGHT, MC_FORWARD, MC_REVERSE, MC_STOP, MC_BOOST, MC_SLEEP,
	// Commands for the camera board
	CC_CAPTURE, CC_SLEEP_CAM, CC_WAKE_CAM, CC_SLEEP
};

#endif
