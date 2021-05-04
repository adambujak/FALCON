/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: flightController.h
 *
 * Code generated for Simulink model 'flightController'.
 *
 * Model version                  : 1.139
 * Simulink Coder version         : 9.4 (R2020b) 29-Jul-2020
 * C/C++ source code generated on : Wed Apr 28 16:02:10 2021
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: ARM Compatible->ARM Cortex-M
 * Code generation objectives:
 *    1. Execution efficiency
 *    2. RAM efficiency
 * Validation result: Not run
 */

#ifndef RTW_HEADER_flightController_h_
#define RTW_HEADER_flightController_h_
#include "rtwtypes.h"
#include <math.h>
#include <string.h>
#ifndef flightController_COMMON_INCLUDES_
#define flightController_COMMON_INCLUDES_
#include "rtwtypes.h"
#endif                                 /* flightController_COMMON_INCLUDES_ */

/* Model Code Variants */

/* Macros for accessing real-time model data structure */

/* Forward declaration for rtModel */
typedef struct tag_RTM RT_MODEL;

#ifndef DEFINED_TYPEDEF_FOR_states_estimate_t_
#define DEFINED_TYPEDEF_FOR_states_estimate_t_

typedef struct {
  real32_T x;
  real32_T y;
  real32_T z;
  real32_T dx;
  real32_T dy;
  real32_T dz;
  real32_T yaw;
  real32_T pitch;
  real32_T roll;
  real32_T p;
  real32_T q;
  real32_T r;
} states_estimate_t;

#endif

#ifndef DEFINED_TYPEDEF_FOR_FCS_control_t_
#define DEFINED_TYPEDEF_FOR_FCS_control_t_

typedef struct {
  real32_T yaw_cmd;
  real32_T roll_cmd;
  real32_T pitch_cmd;
  real32_T alt_cmd;
} FCS_control_t;

#endif

#ifndef DEFINED_TYPEDEF_FOR_FCS_command_t_
#define DEFINED_TYPEDEF_FOR_FCS_command_t_

typedef struct {
  uint8_T control_type;
  FCS_control_t control_input;
} FCS_command_t;

#endif

#ifndef DEFINED_TYPEDEF_FOR_sensor_bias_t_
#define DEFINED_TYPEDEF_FOR_sensor_bias_t_

typedef struct {
  real32_T gyro_bias[3];
  real32_T accel_bias[3];
  real32_T quat_bias[4];
  real32_T alt_bias;
} sensor_bias_t;

#endif

#ifndef DEFINED_TYPEDEF_FOR_sensor_data_t_
#define DEFINED_TYPEDEF_FOR_sensor_data_t_

typedef struct {
  real32_T gyro_data_SI[3];
  real32_T accel_data_SI[3];
  real32_T quat_data[4];
  real32_T alt_data_SI;
} sensor_data_t;

#endif

/* Block signals and states (default storage) for system '<Root>' */
typedef struct {
  real32_T FIR_IMUaccel_states[15];    /* '<S14>/FIR_IMUaccel' */
  real32_T FIR_IMUgyro_states[10];     /* '<S14>/FIR_IMUgyro' */
  real32_T IIR_IMUgyro_r_states[5];    /* '<S14>/IIR_IMUgyro_r' */
  real32_T pressureFilter_IIR_states[3];/* '<S14>/pressureFilter_IIR' */
  real32_T Delay_DSTATE[2];            /* '<S7>/Delay' */
  real32_T DiscreteTimeIntegrator_DSTATE[2];/* '<S7>/Discrete-Time Integrator' */
  real32_T altitude_delay0_DSTATE;     /* '<S12>/altitude_delay0' */
  real32_T UD_DSTATE;                  /* '<S15>/UD' */
  real32_T altitude_delay_DSTATE;      /* '<S12>/altitude_delay' */
  int32_T FIR_IMUaccel_circBuf;        /* '<S14>/FIR_IMUaccel' */
  int32_T FIR_IMUgyro_circBuf;         /* '<S14>/FIR_IMUgyro' */
} DW;

/* Constant parameters (default storage) */
typedef struct {
  /* Computed Parameter: FIR_IMUaccel_Coefficients
   * Referenced by: '<S14>/FIR_IMUaccel'
   */
  real32_T FIR_IMUaccel_Coefficients[6];

  /* Computed Parameter: FIR_IMUgyro_Coefficients
   * Referenced by: '<S14>/FIR_IMUgyro'
   */
  real32_T FIR_IMUgyro_Coefficients[6];

  /* Computed Parameter: IIR_IMUgyro_r_NumCoef
   * Referenced by: '<S14>/IIR_IMUgyro_r'
   */
  real32_T IIR_IMUgyro_r_NumCoef[6];

  /* Computed Parameter: IIR_IMUgyro_r_DenCoef
   * Referenced by: '<S14>/IIR_IMUgyro_r'
   */
  real32_T IIR_IMUgyro_r_DenCoef[6];
} ConstP;

/* Real-time Model Data Structure */
struct tag_RTM {
  DW *dwork;
};

/* External data declarations for dependent source files */
extern const states_estimate_t flightController_rtZstates_estimate_t;/* states_estimate_t ground */
extern const FCS_command_t flightController_rtZFCS_command;/* FCS_command_t ground */
extern const sensor_bias_t flightController_rtZsensor_bias;/* sensor_bias_t ground */
extern const sensor_data_t flightController_rtZsensor_data;/* sensor_data_t ground */

/* Constant parameters (default storage) */
extern const ConstP rtConstP;

/*
 * Exported Global Parameters
 *
 * Note: Exported global parameters are tunable parameters with an exported
 * global storage class designation.  Code generation will declare the memory for
 * these parameters and exports their symbols.
 *
 */
extern real32_T PID_alt_D;             /* Variable: PID_alt_D
                                        * Referenced by: '<S6>/Alt_D_Gain'
                                        */
extern real32_T PID_alt_P;             /* Variable: PID_alt_P
                                        * Referenced by: '<S6>/Alt_P_Gain'
                                        */
extern real32_T PID_pitch_D;           /* Variable: PID_pitch_D
                                        * Referenced by: '<S7>/D_pr'
                                        */
extern real32_T PID_pitch_P;           /* Variable: PID_pitch_P
                                        * Referenced by: '<S7>/P_pr'
                                        */
extern real32_T PID_pitch_roll_I;      /* Variable: PID_pitch_roll_I
                                        * Referenced by: '<S7>/I_pr'
                                        */
extern real32_T PID_yaw_D;             /* Variable: PID_yaw_D
                                        * Referenced by: '<S8>/Yaw_D_Gain'
                                        */
extern real32_T PID_yaw_P;             /* Variable: PID_yaw_P
                                        * Referenced by: '<S8>/Yaw_P_Gain'
                                        */

/* Model entry point functions */
extern void flightController_initialize(RT_MODEL *const rtM, FCS_command_t
  *rtU_Commands, sensor_bias_t *rtU_Bias, sensor_data_t *rtU_Sensors,
  states_estimate_t *rtY_State_Estim, uint16_T rtY_Throttle[4]);
extern void flightController_step(RT_MODEL *const rtM, FCS_command_t
  *rtU_Commands, sensor_bias_t *rtU_Bias, sensor_data_t *rtU_Sensors,
  states_estimate_t *rtY_State_Estim, uint16_T rtY_Throttle[4]);

/*-
 * These blocks were eliminated from the model due to optimizations:
 *
 * Block '<S15>/Data Type Duplicate' : Unused code path elimination
 * Block '<S26>/Reshape (9) to [3x3] column-major' : Reshape block reduction
 */

/*-
 * The generated code includes comments that allow you to trace directly
 * back to the appropriate location in the model.  The basic format
 * is <system>/block_name, where system is the system number (uniquely
 * assigned by Simulink) and block_name is the name of the block.
 *
 * Use the MATLAB hilite_system command to trace the generated code back
 * to the model.  For example,
 *
 * hilite_system('<S3>')    - opens system 3
 * hilite_system('<S3>/Kp') - opens and selects block Kp which resides in S3
 *
 * Here is the system hierarchy for this model
 *
 * '<Root>' : 'flightController'
 * '<S1>'   : 'flightController/FCS'
 * '<S2>'   : 'flightController/StatesEstimator'
 * '<S3>'   : 'flightController/FCS/CommandHandling'
 * '<S4>'   : 'flightController/FCS/Controller'
 * '<S5>'   : 'flightController/FCS/MotorMixing'
 * '<S6>'   : 'flightController/FCS/Controller/AltitudeControl'
 * '<S7>'   : 'flightController/FCS/Controller/PitchRollControl'
 * '<S8>'   : 'flightController/FCS/Controller/YawControl'
 * '<S9>'   : 'flightController/FCS/Controller/PitchRollControl/Angle Conversion'
 * '<S10>'  : 'flightController/FCS/Controller/PitchRollControl/Angle Conversion1'
 * '<S11>'  : 'flightController/FCS/Controller/YawControl/Angle Conversion'
 * '<S12>'  : 'flightController/StatesEstimator/AltitudeEstimator'
 * '<S13>'  : 'flightController/StatesEstimator/OrientationEstimator'
 * '<S14>'  : 'flightController/StatesEstimator/SensorPreprocessing'
 * '<S15>'  : 'flightController/StatesEstimator/AltitudeEstimator/Discrete Derivative'
 * '<S16>'  : 'flightController/StatesEstimator/AltitudeEstimator/Quaternions to  Direction Cosine Matrix'
 * '<S17>'  : 'flightController/StatesEstimator/AltitudeEstimator/Quaternions to  Direction Cosine Matrix/A11'
 * '<S18>'  : 'flightController/StatesEstimator/AltitudeEstimator/Quaternions to  Direction Cosine Matrix/A12'
 * '<S19>'  : 'flightController/StatesEstimator/AltitudeEstimator/Quaternions to  Direction Cosine Matrix/A13'
 * '<S20>'  : 'flightController/StatesEstimator/AltitudeEstimator/Quaternions to  Direction Cosine Matrix/A21'
 * '<S21>'  : 'flightController/StatesEstimator/AltitudeEstimator/Quaternions to  Direction Cosine Matrix/A22'
 * '<S22>'  : 'flightController/StatesEstimator/AltitudeEstimator/Quaternions to  Direction Cosine Matrix/A23'
 * '<S23>'  : 'flightController/StatesEstimator/AltitudeEstimator/Quaternions to  Direction Cosine Matrix/A31'
 * '<S24>'  : 'flightController/StatesEstimator/AltitudeEstimator/Quaternions to  Direction Cosine Matrix/A32'
 * '<S25>'  : 'flightController/StatesEstimator/AltitudeEstimator/Quaternions to  Direction Cosine Matrix/A33'
 * '<S26>'  : 'flightController/StatesEstimator/AltitudeEstimator/Quaternions to  Direction Cosine Matrix/Create 3x3 Matrix'
 * '<S27>'  : 'flightController/StatesEstimator/AltitudeEstimator/Quaternions to  Direction Cosine Matrix/Quaternion Normalize'
 * '<S28>'  : 'flightController/StatesEstimator/AltitudeEstimator/Quaternions to  Direction Cosine Matrix/Quaternion Normalize/Quaternion Modulus'
 * '<S29>'  : 'flightController/StatesEstimator/AltitudeEstimator/Quaternions to  Direction Cosine Matrix/Quaternion Normalize/Quaternion Modulus/Quaternion Norm'
 * '<S30>'  : 'flightController/StatesEstimator/OrientationEstimator/orientation_quat2RPY'
 * '<S31>'  : 'flightController/StatesEstimator/OrientationEstimator/orientation_quat2RPY/Angle Calculation'
 * '<S32>'  : 'flightController/StatesEstimator/OrientationEstimator/orientation_quat2RPY/Quaternion Normalize'
 * '<S33>'  : 'flightController/StatesEstimator/OrientationEstimator/orientation_quat2RPY/Angle Calculation/Protect asincos input'
 * '<S34>'  : 'flightController/StatesEstimator/OrientationEstimator/orientation_quat2RPY/Angle Calculation/Protect asincos input/If Action Subsystem'
 * '<S35>'  : 'flightController/StatesEstimator/OrientationEstimator/orientation_quat2RPY/Angle Calculation/Protect asincos input/If Action Subsystem1'
 * '<S36>'  : 'flightController/StatesEstimator/OrientationEstimator/orientation_quat2RPY/Angle Calculation/Protect asincos input/If Action Subsystem2'
 * '<S37>'  : 'flightController/StatesEstimator/OrientationEstimator/orientation_quat2RPY/Quaternion Normalize/Quaternion Modulus'
 * '<S38>'  : 'flightController/StatesEstimator/OrientationEstimator/orientation_quat2RPY/Quaternion Normalize/Quaternion Modulus/Quaternion Norm'
 * '<S39>'  : 'flightController/StatesEstimator/SensorPreprocessing/preprocessing_RPY2quat'
 * '<S40>'  : 'flightController/StatesEstimator/SensorPreprocessing/preprocessing_quat2RPYbias'
 * '<S41>'  : 'flightController/StatesEstimator/SensorPreprocessing/preprocessing_quat2RPYsens'
 * '<S42>'  : 'flightController/StatesEstimator/SensorPreprocessing/preprocessing_quat2RPYbias/Angle Calculation'
 * '<S43>'  : 'flightController/StatesEstimator/SensorPreprocessing/preprocessing_quat2RPYbias/Quaternion Normalize'
 * '<S44>'  : 'flightController/StatesEstimator/SensorPreprocessing/preprocessing_quat2RPYbias/Angle Calculation/Protect asincos input'
 * '<S45>'  : 'flightController/StatesEstimator/SensorPreprocessing/preprocessing_quat2RPYbias/Angle Calculation/Protect asincos input/If Action Subsystem'
 * '<S46>'  : 'flightController/StatesEstimator/SensorPreprocessing/preprocessing_quat2RPYbias/Angle Calculation/Protect asincos input/If Action Subsystem1'
 * '<S47>'  : 'flightController/StatesEstimator/SensorPreprocessing/preprocessing_quat2RPYbias/Angle Calculation/Protect asincos input/If Action Subsystem2'
 * '<S48>'  : 'flightController/StatesEstimator/SensorPreprocessing/preprocessing_quat2RPYbias/Quaternion Normalize/Quaternion Modulus'
 * '<S49>'  : 'flightController/StatesEstimator/SensorPreprocessing/preprocessing_quat2RPYbias/Quaternion Normalize/Quaternion Modulus/Quaternion Norm'
 * '<S50>'  : 'flightController/StatesEstimator/SensorPreprocessing/preprocessing_quat2RPYsens/Angle Calculation'
 * '<S51>'  : 'flightController/StatesEstimator/SensorPreprocessing/preprocessing_quat2RPYsens/Quaternion Normalize'
 * '<S52>'  : 'flightController/StatesEstimator/SensorPreprocessing/preprocessing_quat2RPYsens/Angle Calculation/Protect asincos input'
 * '<S53>'  : 'flightController/StatesEstimator/SensorPreprocessing/preprocessing_quat2RPYsens/Angle Calculation/Protect asincos input/If Action Subsystem'
 * '<S54>'  : 'flightController/StatesEstimator/SensorPreprocessing/preprocessing_quat2RPYsens/Angle Calculation/Protect asincos input/If Action Subsystem1'
 * '<S55>'  : 'flightController/StatesEstimator/SensorPreprocessing/preprocessing_quat2RPYsens/Angle Calculation/Protect asincos input/If Action Subsystem2'
 * '<S56>'  : 'flightController/StatesEstimator/SensorPreprocessing/preprocessing_quat2RPYsens/Quaternion Normalize/Quaternion Modulus'
 * '<S57>'  : 'flightController/StatesEstimator/SensorPreprocessing/preprocessing_quat2RPYsens/Quaternion Normalize/Quaternion Modulus/Quaternion Norm'
 */
#endif                                 /* RTW_HEADER_flightController_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
