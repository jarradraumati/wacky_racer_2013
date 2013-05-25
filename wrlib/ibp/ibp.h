#ifndef IBP_H_
#define IBP_H_

typedef uint8_t ibp_command;
typedef uint8_t ibp_info;

// Addresses of public data held by the motor board
enum {MD_SPEED, MD_FAULT, MD_CHARGE, MD_DIRECTION};

// Address of public data held by the camera board
enum {CD_PHOTO_READY, CD_PHOTO, CD_FAULT};

// Commands for the motor board
enum {MC_LEFT, MC_RIGHT, MC_FORWARD, MC_REVERSE, MC_STOP, MC_BOOST, MC_SLEEP};

// Commands for the camera board
enum {CC_CAPTURE, CC_SLEEP_CAM, CC_WAKE_CAM, CC_SLEEP};

#endif