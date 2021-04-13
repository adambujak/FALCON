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
  FPT_MODE_COMMAND = 0,
  FPT_MODE_QUERY = 1,
  FPT_MODE_RESPONSE = 2,
  FPT_FLIGHT_CONTROL_COMMAND = 3,
  FPT_STATUS_RESPONSE = 4,
  FPT_TEST_QUERY = 5,
  FPT_TEST_RESPONSE = 6,
  FPT_CNT = 7
} fp_type_t;

typedef enum {
  FE_FLIGHT_MODE_IDLE = 1,
  FE_FLIGHT_MODE_CALIBRATING = 2,
  FE_FLIGHT_MODE_FCS_READY = 3,
  FE_FLIGHT_MODE_FLY = 4
} fe_falcon_mode_t;

/* Types */
typedef struct {
  uint16_t motor1;
  uint16_t motor2;
  uint16_t motor3;
  uint16_t motor4;
} ft_motor_pwm_control_data_t;

typedef struct {
  float x;
  float y;
  float z;
  float dx;
  float dy;
  float dz;
  float yaw;
  float pitch;
  float roll;
  float p;
  float q;
  float r;
} ft_fcs_state_estimate_t;

typedef struct {
  fe_falcon_mode_t mode;
  ft_motor_pwm_control_data_t motor;
  ft_fcs_state_estimate_t states;
} ft_status_data_t;

typedef struct {
  float yaw;
  float pitch;
  float roll;
  float alt;
} ft_fcs_control_input_t;

/* Packets */
typedef struct {
  fe_falcon_mode_t mode;
} fpc_mode_t;

typedef struct {
} fpq_mode_t;

typedef struct {
  fe_falcon_mode_t mode;
} fpr_mode_t;

typedef struct {
  ft_fcs_control_input_t fcsControlCmd;
} fpc_flight_control_t;

typedef struct {
  ft_status_data_t status;
} fpr_status_t;

typedef struct {
} fpq_test_t;

typedef struct {
  uint32_t cookie;
} fpr_test_t;


/* Functions */
fp_type_t fp_get_packet_type(uint8_t packetID);
uint8_t fp_get_packet_length(fp_type_t packetType);

#endif /* FALCON_PACKET_H */