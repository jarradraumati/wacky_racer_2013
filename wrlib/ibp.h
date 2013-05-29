#ifndef IBP_H_
#define IBP_H_

#define I2C_CAMERA_ADDR 0x32
#define I2C_COMMS_ADDR	0X23
#define I2C_MOTOR_ADDR	0x04

#define IBP_I2C_TIMEOUT 1000

typedef uint8_t ibp_command;
typedef uint8_t ibp_info;

#define CMD_NONE 0
#define COMMS_COMMAND 2
#define CMD_ASSUME_CTRL 4

#define MC_VALUES_START 40
#define MC_VALUES_STOP 49

#define CC_VALUES_START 50
#define CC_VALUES_STOP 59

#define ARRAY_OFFSET 70

// Addresses of public data held by the motor board
enum {MD_SPEED, MD_FAULT, MD_CHARGE, MD_DIRECTION};
// Address of public data held by the camera board
enum {CD_PHOTO_READY=ARRAY_OFFSET, CD_PHOTO_LINE, CD_PHOTO_NEXT_LINE, CD_FAULT};

// Commands for the motor board
enum {MC_LEFT = MC_VALUES_START, MC_RIGHT, MC_FORWARD, MC_BACK, MC_STOP};

// Commands for the camera board
enum {CC_CAPTURE = CC_VALUES_START, CC_SLEEP_CAM, CC_WAKE_CAM, CC_SLEEP};

#endif
