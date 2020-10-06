#ifndef FALCON_PACKET_H
#define FALCON_PACKET_H

#include <stdint.h>

typedef enum {
  FPT_HEDWIG_STATUS_REQUEST = 0,
  FPT_HEDWIG_STATUS,
  FPT_HEDWIG_CONTROL_DATA,
} fpt_t;

typedef struct {
  fpt_t type;
  void *data;
} falcon_packet_t;


/* FPT HEDWIG STATUS REQUEST */

typedef struct
{
  uint32_t flags;
} fp_hedwig_status_request_data_t;

/* FPT HEDWIG STATUS */

#define FP_HEDWIG_STATUS_CONNECTED_FLAG_Mask  0x1
#define FP_HEDWIG_STATUS_CONNECTED_FLAG_Shift 0

#define FP_HEDWIG_STATUS_OPERATION_MODE_Mask  0x7
#define FP_HEDWIG_STATUS_OPERATION_MODE_Shift 1

typedef enum {
  FP_HOM_MANUAL = 0,
  FP_HOM_AUTO
} fp_hedwig_operation_mode_t;

typedef struct
{
  uint8_t isConnected;
  fp_hedwig_operation_mode_t operationMode;
} fp_hedwig_status_request_data_t;

/* FPT HEDWIG CONTROL DATA */

typedef enum {
  FP_HEDWIG_CONTROL_DATA_TYPE_MANUAL,
  FP_HEDWIG_CONTROL_DATA_TYPE_GPS,
} fp_hedwig_control_data_type_t;

#define FP_HEDWIG_MANUAL_CONTROL_DATA_X_Mask      0x3
#define FP_HEDWIG_MANUAL_CONTROL_DATA_X_Shift     0

#define FP_HEDWIG_MANUAL_CONTROL_DATA_Y_Mask      0x3
#define FP_HEDWIG_MANUAL_CONTROL_DATA_Y_Shift     2

#define FP_HEDWIG_MANUAL_CONTROL_DATA_Z_Mask      0x3
#define FP_HEDWIG_MANUAL_CONTROL_DATA_Z_Shift     4

#define FP_HEDWIG_MANUAL_CONTROL_DATA_PITCH_Mask  0x3
#define FP_HEDWIG_MANUAL_CONTROL_DATA_PITCH_Shift 6

typedef struct {
  int8_t x;
  int8_t y;
  int8_t z;
  int8_t pitch;
} fp_hedwig_manual_control_data_t;

#define FP_HEDWIG_GPS_CONTROL_DATA_LATITUDE_Mask      0xFFFFFFFF
#define FP_HEDWIG_GPS_CONTROL_DATA_LATITUDE_Shift     0

#define FP_HEDWIG_GPS_CONTROL_DATA_LONGITUDE_Mask     0xFFFFFFFF
#define FP_HEDWIG_GPS_CONTROL_DATA_LONGITUDE_Shift    32

typedef struct {
  float latitude;
  float longitude;
} fp_hedwig_gps_control_data_t;

typedef struct {
  fp_hedwig_control_data_type_t type;
  void *controlData;
};

#endif // FALCON_PACKET_H
