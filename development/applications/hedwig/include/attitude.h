#include <stdint.h>

#ifndef ATTITUDE_H
#define ATTITUDE_H

#define M_PIf                 3.14159265358979323846f
#define RAD2DEGREES(angle)    ((angle) * (180.0f / M_PIf))
#define DEGREES2RAD(angle)    ((angle) * (M_PIf / 180.0f))
#ifndef sq
#define sq(x)                 ((x) * (x))
#endif

typedef enum
{
  X = 0,
  Y,
  Z
} axis_e;

#define XYZ    3

typedef struct {
  float w, x, y, z;
} quaternion_t;
#define QUATERNION_INITIALIZE    { .w = 1, .x = 0, .y = 0, .z = 0 }

typedef struct {
  float ww, wx, wy, wz, xx, xy, xz, yy, yz, zz;
} quaternionProducts_t;
#define QUATERNION_PRODUCTS_INITIALIZE    { .ww = 1, .wx = 0, .wy = 0, .wz = 0, .xx = 0, .xy = 0, .xz = 0, .yy = 0, \
                                            .yz = 0, .zz = 0 }

typedef struct {
  float yaw, pitch, roll;
} attitudeEulerAngles_t;
#define EULER_INITIALIZE    { .yaw = 0, .pitch = 0, .roll = 0 }

typedef struct {
  float m[3][3];
} rotationMatrix_t;
#define DCM_INITIALIZE    { .m = { { 1, 0, 0 }, { 0, 1, 0 }, { 0, 0, 1 } } }


extern quaternion_t q;
extern attitudeEulerAngles_t attitude;
extern rotationMatrix_t DCM;

void attitude_compute_estimation(uint32_t currentTimeUs, float *accel, float *gyro, float *mag);
void attitude_init_axis(void);

#endif // ATTITUDE_H
