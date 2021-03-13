#ifndef FALCON_PACKET_H
#define FALCON_PACKET_H


#include <stdint.h>

#define MAX_FRAME_SIZE 96
#define MAX_PACKET_SIZE 94
#define FRAME_DELIMITER1 81
#define FRAME_DELIMITER2 152
#define PACKET_DELIMITER1 167
#define PACKET_DELIMITER2 185
#define PACKET_HEADER_SIZE 3
#define FRAME_HEADER_SIZE 3
#define FRAME_FOOTER_SIZE 2

/* Enums */
typedef enum {
  FPT_FLIGHT_CONTROL_STICK_COMMAND = 0,
  FPT_FLIGHT_CONTROL_POSITION_COMMAND = 1,
  FPT_MOTOR_SPEED_COMMAND = 2,
  FPT_MOTOR_SPEED_QUERY = 3,
  FPT_MOTOR_SPEED_RESPONSE = 4,
  FPT_SET_DESTINATION_COMMAND = 5,
  FPT_STATUS_RESPONSE = 6,
  FPT_CNT = 7
} fp_type_t;

typedef enum {
  FE_FLIGHT_CONTROL_MODE_STICK = 0,
  FE_FLIGHT_CONTROL_MODE_MOTION = 1,
  FE_FLIGHT_CONTROL_MODE_POSITION = 2,
  FE_FLIGHT_CONTROL_MODE_CNT = 3
} fe_flight_control_mode_t;

/* Types */
typedef struct {
  uint16_t motor1;
  uint16_t motor2;
  uint16_t motor3;
  uint16_t motor4;
} ft_motor_pwm_control_data_t;

typedef struct {
  float longitude;
  float latitude;
} ft_gps_coordinate_data_t;

typedef struct {
  ft_gps_coordinate_data_t gps;
  ft_motor_pwm_control_data_t motor;
  uint8_t someEight;
  int8_t someIEight;
  int16_t someSixteen;
  uint32_t someThirtyTwo;
  int32_t someIThirtyTwo;
} ft_status_data_t;

typedef struct {
  float yaw;
  float pitch;
  float roll;
  float z;
} ft_orientation_reference_t;

typedef struct {
  float x;
  float y;
  float z;
  float yaw;
} ft_position_reference_t;

/* Packets */
typedef struct {
  ft_orientation_reference_t orientationReferenceCmd;
} fpc_flight_control_stick_t;

typedef struct {
  ft_position_reference_t positionReferenceCMD;
} fpc_flight_control_position_t;

typedef struct {
  ft_motor_pwm_control_data_t pwmData;
} fpc_motor_speed_t;

typedef struct {
  ft_motor_pwm_control_data_t pwmData;
} fpq_motor_speed_t;

typedef struct {
  ft_motor_pwm_control_data_t pwmData;
} fpr_motor_speed_t;

typedef struct {
  ft_gps_coordinate_data_t gpsData;
} fpc_set_destination_t;

typedef struct {
  ft_gps_coordinate_data_t gpsData;
  ft_status_data_t status;
} fpr_status_t;


/* Functions */
fp_type_t fp_get_packet_type(uint8_t packetID);
uint8_t fp_get_packet_length(fp_type_t packetType);

#endif /* FALCON_PACKET_H */