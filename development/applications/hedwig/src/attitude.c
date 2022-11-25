#include "attitude.h"

#include "MadgwickAHRS.h"


#include <stdbool.h>
#include <stdint.h>
#include <math.h>

rotationMatrix_t DCM = DCM_INITIALIZE;

static rotationMatrix_t DCMnwu2ned = { .m = { { 1,  0,  0 },
                                              { 0, -1,  0 },
                                              { 0,  0, -1 } } };

static quaternion_t qnwu2ned = { .w = 0, .x = 1, .y = 0, .z = 0 };
static quaternion_t qnwu2nedINV;

static float gravity_ned[XYZ] = { 0, 0, 9.80665 };
static float gravity_bof[XYZ];

// quaternion of sensor frame relative to earth frame
quaternion_t q = QUATERNION_INITIALIZE;
static quaternion_t qINV;
static quaternionProducts_t qP = QUATERNION_PRODUCTS_INITIALIZE;

attitudeEulerAngles_t attitude = EULER_INITIALIZE;

extern volatile float q0, q1, q2, q3;

static void QuaternionComputeProducts(quaternion_t *quat, quaternionProducts_t *quatProd)
{
  quatProd->ww = quat->w * quat->w;
  quatProd->wx = quat->w * quat->x;
  quatProd->wy = quat->w * quat->y;
  quatProd->wz = quat->w * quat->z;
  quatProd->xx = quat->x * quat->x;
  quatProd->xy = quat->x * quat->y;
  quatProd->xz = quat->x * quat->z;
  quatProd->yy = quat->y * quat->y;
  quatProd->yz = quat->y * quat->z;
  quatProd->zz = quat->z * quat->z;
}

static void dcm_rotation(float *v, rotationMatrix_t *rotationMatrix)
{
  float tmp[XYZ] = { v[X], v[Y], v[Z] };

  v[X] = (rotationMatrix->m[0][0] * tmp[X] + rotationMatrix->m[0][1] * tmp[Y] + rotationMatrix->m[0][2] * tmp[Z]);
  v[Y] = (rotationMatrix->m[1][0] * tmp[X] + rotationMatrix->m[1][1] * tmp[Y] + rotationMatrix->m[1][2] * tmp[Z]);
  v[Z] = (rotationMatrix->m[2][0] * tmp[X] + rotationMatrix->m[2][1] * tmp[Y] + rotationMatrix->m[2][2] * tmp[Z]);
}

static void quat2dcm(quaternion_t *quat, rotationMatrix_t *dcm)
{
  quaternionProducts_t qp;

  QuaternionComputeProducts(quat, &qp);

  dcm->m[0][0] = qp.ww + qp.xx - qp.yy - qp.zz;
  dcm->m[0][1] = 2.0f * (qp.xy + qp.wz);
  dcm->m[0][2] = 2.0f * (qp.xz - qp.wy);

  dcm->m[1][0] = 2.0f * (qp.xy - qp.wz);
  dcm->m[1][1] = qp.ww - qp.xx + qp.yy - qp.zz;
  dcm->m[1][2] = 2.0f * (qp.yz + qp.wx);

  dcm->m[2][0] = 2.0f * (qp.xz + qp.wy);
  dcm->m[2][1] = 2.0f * (qp.yz - qp.wx);
  dcm->m[2][2] = qp.ww - qp.xx - qp.yy + qp.zz;
}

static void quat2angles(quaternion_t *quat, attitudeEulerAngles_t *euler)
{
  quaternionProducts_t qp;

  QuaternionComputeProducts(quat, &qp);

  euler->yaw = atan2f(2.0f * (qp.xy + qp.wz), qp.ww + qp.xx - qp.yy - qp.zz);
  euler->pitch = asinf(-2.0f * (qp.xz - qp.wy));
  euler->roll = atan2f(2.0f * (qp.yz + qp.wx), qp.ww - qp.xx - qp.yy + qp.zz);
}

static void quaternion_copy(quaternion_t *quat, quaternion_t *dest)
{
  dest->w = quat->w;
  dest->x = quat->x;
  dest->y = quat->y;
  dest->z = quat->z;
}

static void quaternion_rotation(quaternion_t *quat, float *vector, float *output)
{
  quaternionProducts_t qp;

  QuaternionComputeProducts(quat, &qp);

  float v[3] = { vector[X], vector[Y], vector[Z] };

  output[X] = qp.ww * v[X] + 2 * qp.wy * v[Z] - 2 * qp.wz * v[Y] +
              qp.xx * v[X] + 2 * qp.xy * v[Y] + 2 * qp.xz * v[Z] -
              qp.zz * v[X] - qp.yy * v[X];

  output[Y] = 2 * qp.xy * v[X] + qp.yy * v[Y] + 2 * qp.yz * v[Z] +
              2 * qp.wz * v[X] - qp.zz * v[Y] + qp.ww * v[Y] -
              2 * qp.wx * v[Z] - qp.xx * v[Y];

  output[Z] = 2 * qp.xz * v[X] + 2 * qp.yz * v[Y] + qp.zz * v[Z] -
              2 * qp.wy * v[X] - qp.yy * v[Z] + 2 * qp.wx * v[Y] -
              qp.xx * v[Z] + qp.ww * v[Z];
}

static void quaternion_multiply(quaternion_t *q1, quaternion_t *q2, quaternion_t *output)
{
  quaternion_t tmp1;
  quaternion_t tmp2;

  quaternion_copy(q1, &tmp1);
  quaternion_copy(q2, &tmp2);

  output->w = tmp1.w * tmp2.w - tmp1.x * tmp2.x - tmp1.y * tmp2.y - tmp1.z * tmp2.z;
  output->x = tmp1.x * tmp2.w + tmp1.w * tmp2.x + tmp1.y * tmp2.z - tmp1.z * tmp2.y;
  output->y = tmp1.w * tmp2.y - tmp1.x * tmp2.z + tmp1.y * tmp2.w + tmp1.z * tmp2.x;
  output->z = tmp1.w * tmp2.z + tmp1.x * tmp2.y - tmp1.y * tmp2.x + tmp1.z * tmp2.w;
}

static void quaternion_inverse(quaternion_t *quat, quaternion_t *output)
{
  quaternion_t tmp;

  quaternion_copy(quat, &tmp);

  quaternionProducts_t qp;

  QuaternionComputeProducts(&tmp, &qp);

  float denom = qp.ww + qp.xx + qp.yy + qp.zz;

  output->w = tmp.w / denom;
  output->x = -tmp.x / denom;
  output->y = -tmp.y / denom;
  output->z = -tmp.z / denom;
}

static void vector_subtraction(float *v1, float *v2, float *output)
{
  output[X] = v1[X] - v2[X];
  output[Y] = v1[Y] - v2[Y];
  output[Z] = v1[Z] - v2[Z];
}

static void vector_addition(float *v1, float *v2, float *output)
{
  output[X] = v1[X] + v2[X];
  output[Y] = v1[Y] + v2[Y];
  output[Z] = v1[Z] + v2[Z];
}

void attitude_compute_estimation(uint32_t currentTimeUs, float *accel, float *gyro, float *mag)
{
  static uint32_t previousIMUUpdateTime;
  const uint32_t deltaT = currentTimeUs - previousIMUUpdateTime;

  previousIMUUpdateTime = currentTimeUs;

  MadgwickAHRSupdateIMU(deltaT * 1e-6f, gyro[X], gyro[Y], gyro[Z], accel[X], accel[Y], accel[Z]);

  q.w = q0;
  q.x = q1;
  q.y = q2;
  q.z = q3;

  quaternion_rotation(&qnwu2ned, accel, accel);
  quaternion_rotation(&qnwu2ned, gyro, gyro);

  quaternion_multiply(&qnwu2ned, &q, &q);
  quaternion_multiply(&q, &qnwu2nedINV, &q);

  quaternion_inverse(&q, &qINV);
  quaternion_rotation(&qINV, gravity_ned, gravity_bof);

  vector_addition(accel, gravity_bof, accel);

  quat2dcm(&q, &DCM);

  quat2angles(&q, &attitude);
}

void attitude_init_axis(void)
{
  quaternion_inverse(&qnwu2ned, &qnwu2nedINV);
}
