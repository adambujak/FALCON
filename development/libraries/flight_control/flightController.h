/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: flightController.h
 *
 * Code generated for Simulink model 'flightController'.
 *
 * Model version                  : 1.123
 * Simulink Coder version         : 9.4 (R2020b) 29-Jul-2020
 * C/C++ source code generated on : Mon Apr 26 21:44:25 2021
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
  real_T Filter_DSTATE;                /* '<S136>/Filter' */
  real32_T FIR_IMUaccel_states[15];    /* '<S210>/FIR_IMUaccel' */
  real32_T FIR_IMUgyro_states[10];     /* '<S210>/FIR_IMUgyro' */
  real32_T IIR_IMUgyro_r_states[5];    /* '<S210>/IIR_IMUgyro_r' */
  real32_T pressureFilter_IIR_states[3];/* '<S210>/pressureFilter_IIR' */
  real32_T altitude_delay0_DSTATE;     /* '<S208>/altitude_delay0' */
  real32_T UD_DSTATE;                  /* '<S211>/UD' */
  real32_T altitude_delay_DSTATE;      /* '<S208>/altitude_delay' */
  real32_T Integrator_DSTATE;          /* '<S191>/Integrator' */
  real32_T Filter_DSTATE_k;            /* '<S88>/Filter' */
  real32_T Integrator_DSTATE_n;        /* '<S93>/Integrator' */
  real32_T Integrator_DSTATE_j;        /* '<S141>/Integrator' */
  real32_T Integrator_DSTATE_m;        /* '<S41>/Integrator' */
  int32_T FIR_IMUaccel_circBuf;        /* '<S210>/FIR_IMUaccel' */
  int32_T FIR_IMUgyro_circBuf;         /* '<S210>/FIR_IMUgyro' */
} DW;

/* Constant parameters (default storage) */
typedef struct {
  /* Computed Parameter: FIR_IMUaccel_Coefficients
   * Referenced by: '<S210>/FIR_IMUaccel'
   */
  real32_T FIR_IMUaccel_Coefficients[6];

  /* Computed Parameter: FIR_IMUgyro_Coefficients
   * Referenced by: '<S210>/FIR_IMUgyro'
   */
  real32_T FIR_IMUgyro_Coefficients[6];

  /* Computed Parameter: IIR_IMUgyro_r_NumCoef
   * Referenced by: '<S210>/IIR_IMUgyro_r'
   */
  real32_T IIR_IMUgyro_r_NumCoef[6];

  /* Computed Parameter: IIR_IMUgyro_r_DenCoef
   * Referenced by: '<S210>/IIR_IMUgyro_r'
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
extern real32_T PID_pitch_D;           /* Variable: PID_pitch_D
                                        * Referenced by:
                                        *   '<S87>/Derivative Gain'
                                        *   '<S135>/Derivative Gain'
                                        */
extern real32_T PID_pitch_I;           /* Variable: PID_pitch_I
                                        * Referenced by:
                                        *   '<S90>/Integral Gain'
                                        *   '<S138>/Integral Gain'
                                        */
extern real32_T PID_pitch_N;           /* Variable: PID_pitch_N
                                        * Referenced by:
                                        *   '<S96>/Filter Coefficient'
                                        *   '<S144>/Filter Coefficient'
                                        */
extern real32_T PID_pitch_P;           /* Variable: PID_pitch_P
                                        * Referenced by:
                                        *   '<S98>/Proportional Gain'
                                        *   '<S146>/Proportional Gain'
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
 * Block '<S211>/Data Type Duplicate' : Unused code path elimination
 * Block '<S222>/Reshape (9) to [3x3] column-major' : Reshape block reduction
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
 * '<S9>'   : 'flightController/FCS/Controller/AltitudeControl/Discrete PID Controller'
 * '<S10>'  : 'flightController/FCS/Controller/AltitudeControl/Discrete PID Controller/Anti-windup'
 * '<S11>'  : 'flightController/FCS/Controller/AltitudeControl/Discrete PID Controller/D Gain'
 * '<S12>'  : 'flightController/FCS/Controller/AltitudeControl/Discrete PID Controller/Filter'
 * '<S13>'  : 'flightController/FCS/Controller/AltitudeControl/Discrete PID Controller/Filter ICs'
 * '<S14>'  : 'flightController/FCS/Controller/AltitudeControl/Discrete PID Controller/I Gain'
 * '<S15>'  : 'flightController/FCS/Controller/AltitudeControl/Discrete PID Controller/Ideal P Gain'
 * '<S16>'  : 'flightController/FCS/Controller/AltitudeControl/Discrete PID Controller/Ideal P Gain Fdbk'
 * '<S17>'  : 'flightController/FCS/Controller/AltitudeControl/Discrete PID Controller/Integrator'
 * '<S18>'  : 'flightController/FCS/Controller/AltitudeControl/Discrete PID Controller/Integrator ICs'
 * '<S19>'  : 'flightController/FCS/Controller/AltitudeControl/Discrete PID Controller/N Copy'
 * '<S20>'  : 'flightController/FCS/Controller/AltitudeControl/Discrete PID Controller/N Gain'
 * '<S21>'  : 'flightController/FCS/Controller/AltitudeControl/Discrete PID Controller/P Copy'
 * '<S22>'  : 'flightController/FCS/Controller/AltitudeControl/Discrete PID Controller/Parallel P Gain'
 * '<S23>'  : 'flightController/FCS/Controller/AltitudeControl/Discrete PID Controller/Reset Signal'
 * '<S24>'  : 'flightController/FCS/Controller/AltitudeControl/Discrete PID Controller/Saturation'
 * '<S25>'  : 'flightController/FCS/Controller/AltitudeControl/Discrete PID Controller/Saturation Fdbk'
 * '<S26>'  : 'flightController/FCS/Controller/AltitudeControl/Discrete PID Controller/Sum'
 * '<S27>'  : 'flightController/FCS/Controller/AltitudeControl/Discrete PID Controller/Sum Fdbk'
 * '<S28>'  : 'flightController/FCS/Controller/AltitudeControl/Discrete PID Controller/Tracking Mode'
 * '<S29>'  : 'flightController/FCS/Controller/AltitudeControl/Discrete PID Controller/Tracking Mode Sum'
 * '<S30>'  : 'flightController/FCS/Controller/AltitudeControl/Discrete PID Controller/Tsamp - Integral'
 * '<S31>'  : 'flightController/FCS/Controller/AltitudeControl/Discrete PID Controller/Tsamp - Ngain'
 * '<S32>'  : 'flightController/FCS/Controller/AltitudeControl/Discrete PID Controller/postSat Signal'
 * '<S33>'  : 'flightController/FCS/Controller/AltitudeControl/Discrete PID Controller/preSat Signal'
 * '<S34>'  : 'flightController/FCS/Controller/AltitudeControl/Discrete PID Controller/Anti-windup/Passthrough'
 * '<S35>'  : 'flightController/FCS/Controller/AltitudeControl/Discrete PID Controller/D Gain/Disabled'
 * '<S36>'  : 'flightController/FCS/Controller/AltitudeControl/Discrete PID Controller/Filter/Disabled'
 * '<S37>'  : 'flightController/FCS/Controller/AltitudeControl/Discrete PID Controller/Filter ICs/Disabled'
 * '<S38>'  : 'flightController/FCS/Controller/AltitudeControl/Discrete PID Controller/I Gain/Internal Parameters'
 * '<S39>'  : 'flightController/FCS/Controller/AltitudeControl/Discrete PID Controller/Ideal P Gain/Passthrough'
 * '<S40>'  : 'flightController/FCS/Controller/AltitudeControl/Discrete PID Controller/Ideal P Gain Fdbk/Disabled'
 * '<S41>'  : 'flightController/FCS/Controller/AltitudeControl/Discrete PID Controller/Integrator/Discrete'
 * '<S42>'  : 'flightController/FCS/Controller/AltitudeControl/Discrete PID Controller/Integrator ICs/Internal IC'
 * '<S43>'  : 'flightController/FCS/Controller/AltitudeControl/Discrete PID Controller/N Copy/Disabled wSignal Specification'
 * '<S44>'  : 'flightController/FCS/Controller/AltitudeControl/Discrete PID Controller/N Gain/Disabled'
 * '<S45>'  : 'flightController/FCS/Controller/AltitudeControl/Discrete PID Controller/P Copy/Disabled'
 * '<S46>'  : 'flightController/FCS/Controller/AltitudeControl/Discrete PID Controller/Parallel P Gain/Internal Parameters'
 * '<S47>'  : 'flightController/FCS/Controller/AltitudeControl/Discrete PID Controller/Reset Signal/Disabled'
 * '<S48>'  : 'flightController/FCS/Controller/AltitudeControl/Discrete PID Controller/Saturation/Passthrough'
 * '<S49>'  : 'flightController/FCS/Controller/AltitudeControl/Discrete PID Controller/Saturation Fdbk/Disabled'
 * '<S50>'  : 'flightController/FCS/Controller/AltitudeControl/Discrete PID Controller/Sum/Sum_PI'
 * '<S51>'  : 'flightController/FCS/Controller/AltitudeControl/Discrete PID Controller/Sum Fdbk/Disabled'
 * '<S52>'  : 'flightController/FCS/Controller/AltitudeControl/Discrete PID Controller/Tracking Mode/Disabled'
 * '<S53>'  : 'flightController/FCS/Controller/AltitudeControl/Discrete PID Controller/Tracking Mode Sum/Passthrough'
 * '<S54>'  : 'flightController/FCS/Controller/AltitudeControl/Discrete PID Controller/Tsamp - Integral/Passthrough'
 * '<S55>'  : 'flightController/FCS/Controller/AltitudeControl/Discrete PID Controller/Tsamp - Ngain/Passthrough'
 * '<S56>'  : 'flightController/FCS/Controller/AltitudeControl/Discrete PID Controller/postSat Signal/Forward_Path'
 * '<S57>'  : 'flightController/FCS/Controller/AltitudeControl/Discrete PID Controller/preSat Signal/Forward_Path'
 * '<S58>'  : 'flightController/FCS/Controller/PitchRollControl/Angle Conversion'
 * '<S59>'  : 'flightController/FCS/Controller/PitchRollControl/Angle Conversion1'
 * '<S60>'  : 'flightController/FCS/Controller/PitchRollControl/PID_Pitch'
 * '<S61>'  : 'flightController/FCS/Controller/PitchRollControl/PID_Roll'
 * '<S62>'  : 'flightController/FCS/Controller/PitchRollControl/PID_Pitch/Anti-windup'
 * '<S63>'  : 'flightController/FCS/Controller/PitchRollControl/PID_Pitch/D Gain'
 * '<S64>'  : 'flightController/FCS/Controller/PitchRollControl/PID_Pitch/Filter'
 * '<S65>'  : 'flightController/FCS/Controller/PitchRollControl/PID_Pitch/Filter ICs'
 * '<S66>'  : 'flightController/FCS/Controller/PitchRollControl/PID_Pitch/I Gain'
 * '<S67>'  : 'flightController/FCS/Controller/PitchRollControl/PID_Pitch/Ideal P Gain'
 * '<S68>'  : 'flightController/FCS/Controller/PitchRollControl/PID_Pitch/Ideal P Gain Fdbk'
 * '<S69>'  : 'flightController/FCS/Controller/PitchRollControl/PID_Pitch/Integrator'
 * '<S70>'  : 'flightController/FCS/Controller/PitchRollControl/PID_Pitch/Integrator ICs'
 * '<S71>'  : 'flightController/FCS/Controller/PitchRollControl/PID_Pitch/N Copy'
 * '<S72>'  : 'flightController/FCS/Controller/PitchRollControl/PID_Pitch/N Gain'
 * '<S73>'  : 'flightController/FCS/Controller/PitchRollControl/PID_Pitch/P Copy'
 * '<S74>'  : 'flightController/FCS/Controller/PitchRollControl/PID_Pitch/Parallel P Gain'
 * '<S75>'  : 'flightController/FCS/Controller/PitchRollControl/PID_Pitch/Reset Signal'
 * '<S76>'  : 'flightController/FCS/Controller/PitchRollControl/PID_Pitch/Saturation'
 * '<S77>'  : 'flightController/FCS/Controller/PitchRollControl/PID_Pitch/Saturation Fdbk'
 * '<S78>'  : 'flightController/FCS/Controller/PitchRollControl/PID_Pitch/Sum'
 * '<S79>'  : 'flightController/FCS/Controller/PitchRollControl/PID_Pitch/Sum Fdbk'
 * '<S80>'  : 'flightController/FCS/Controller/PitchRollControl/PID_Pitch/Tracking Mode'
 * '<S81>'  : 'flightController/FCS/Controller/PitchRollControl/PID_Pitch/Tracking Mode Sum'
 * '<S82>'  : 'flightController/FCS/Controller/PitchRollControl/PID_Pitch/Tsamp - Integral'
 * '<S83>'  : 'flightController/FCS/Controller/PitchRollControl/PID_Pitch/Tsamp - Ngain'
 * '<S84>'  : 'flightController/FCS/Controller/PitchRollControl/PID_Pitch/postSat Signal'
 * '<S85>'  : 'flightController/FCS/Controller/PitchRollControl/PID_Pitch/preSat Signal'
 * '<S86>'  : 'flightController/FCS/Controller/PitchRollControl/PID_Pitch/Anti-windup/Passthrough'
 * '<S87>'  : 'flightController/FCS/Controller/PitchRollControl/PID_Pitch/D Gain/Internal Parameters'
 * '<S88>'  : 'flightController/FCS/Controller/PitchRollControl/PID_Pitch/Filter/Disc. Forward Euler Filter'
 * '<S89>'  : 'flightController/FCS/Controller/PitchRollControl/PID_Pitch/Filter ICs/Internal IC - Filter'
 * '<S90>'  : 'flightController/FCS/Controller/PitchRollControl/PID_Pitch/I Gain/Internal Parameters'
 * '<S91>'  : 'flightController/FCS/Controller/PitchRollControl/PID_Pitch/Ideal P Gain/Passthrough'
 * '<S92>'  : 'flightController/FCS/Controller/PitchRollControl/PID_Pitch/Ideal P Gain Fdbk/Disabled'
 * '<S93>'  : 'flightController/FCS/Controller/PitchRollControl/PID_Pitch/Integrator/Discrete'
 * '<S94>'  : 'flightController/FCS/Controller/PitchRollControl/PID_Pitch/Integrator ICs/Internal IC'
 * '<S95>'  : 'flightController/FCS/Controller/PitchRollControl/PID_Pitch/N Copy/Disabled'
 * '<S96>'  : 'flightController/FCS/Controller/PitchRollControl/PID_Pitch/N Gain/Internal Parameters'
 * '<S97>'  : 'flightController/FCS/Controller/PitchRollControl/PID_Pitch/P Copy/Disabled'
 * '<S98>'  : 'flightController/FCS/Controller/PitchRollControl/PID_Pitch/Parallel P Gain/Internal Parameters'
 * '<S99>'  : 'flightController/FCS/Controller/PitchRollControl/PID_Pitch/Reset Signal/Disabled'
 * '<S100>' : 'flightController/FCS/Controller/PitchRollControl/PID_Pitch/Saturation/Passthrough'
 * '<S101>' : 'flightController/FCS/Controller/PitchRollControl/PID_Pitch/Saturation Fdbk/Disabled'
 * '<S102>' : 'flightController/FCS/Controller/PitchRollControl/PID_Pitch/Sum/Sum_PID'
 * '<S103>' : 'flightController/FCS/Controller/PitchRollControl/PID_Pitch/Sum Fdbk/Disabled'
 * '<S104>' : 'flightController/FCS/Controller/PitchRollControl/PID_Pitch/Tracking Mode/Disabled'
 * '<S105>' : 'flightController/FCS/Controller/PitchRollControl/PID_Pitch/Tracking Mode Sum/Passthrough'
 * '<S106>' : 'flightController/FCS/Controller/PitchRollControl/PID_Pitch/Tsamp - Integral/Passthrough'
 * '<S107>' : 'flightController/FCS/Controller/PitchRollControl/PID_Pitch/Tsamp - Ngain/Passthrough'
 * '<S108>' : 'flightController/FCS/Controller/PitchRollControl/PID_Pitch/postSat Signal/Forward_Path'
 * '<S109>' : 'flightController/FCS/Controller/PitchRollControl/PID_Pitch/preSat Signal/Forward_Path'
 * '<S110>' : 'flightController/FCS/Controller/PitchRollControl/PID_Roll/Anti-windup'
 * '<S111>' : 'flightController/FCS/Controller/PitchRollControl/PID_Roll/D Gain'
 * '<S112>' : 'flightController/FCS/Controller/PitchRollControl/PID_Roll/Filter'
 * '<S113>' : 'flightController/FCS/Controller/PitchRollControl/PID_Roll/Filter ICs'
 * '<S114>' : 'flightController/FCS/Controller/PitchRollControl/PID_Roll/I Gain'
 * '<S115>' : 'flightController/FCS/Controller/PitchRollControl/PID_Roll/Ideal P Gain'
 * '<S116>' : 'flightController/FCS/Controller/PitchRollControl/PID_Roll/Ideal P Gain Fdbk'
 * '<S117>' : 'flightController/FCS/Controller/PitchRollControl/PID_Roll/Integrator'
 * '<S118>' : 'flightController/FCS/Controller/PitchRollControl/PID_Roll/Integrator ICs'
 * '<S119>' : 'flightController/FCS/Controller/PitchRollControl/PID_Roll/N Copy'
 * '<S120>' : 'flightController/FCS/Controller/PitchRollControl/PID_Roll/N Gain'
 * '<S121>' : 'flightController/FCS/Controller/PitchRollControl/PID_Roll/P Copy'
 * '<S122>' : 'flightController/FCS/Controller/PitchRollControl/PID_Roll/Parallel P Gain'
 * '<S123>' : 'flightController/FCS/Controller/PitchRollControl/PID_Roll/Reset Signal'
 * '<S124>' : 'flightController/FCS/Controller/PitchRollControl/PID_Roll/Saturation'
 * '<S125>' : 'flightController/FCS/Controller/PitchRollControl/PID_Roll/Saturation Fdbk'
 * '<S126>' : 'flightController/FCS/Controller/PitchRollControl/PID_Roll/Sum'
 * '<S127>' : 'flightController/FCS/Controller/PitchRollControl/PID_Roll/Sum Fdbk'
 * '<S128>' : 'flightController/FCS/Controller/PitchRollControl/PID_Roll/Tracking Mode'
 * '<S129>' : 'flightController/FCS/Controller/PitchRollControl/PID_Roll/Tracking Mode Sum'
 * '<S130>' : 'flightController/FCS/Controller/PitchRollControl/PID_Roll/Tsamp - Integral'
 * '<S131>' : 'flightController/FCS/Controller/PitchRollControl/PID_Roll/Tsamp - Ngain'
 * '<S132>' : 'flightController/FCS/Controller/PitchRollControl/PID_Roll/postSat Signal'
 * '<S133>' : 'flightController/FCS/Controller/PitchRollControl/PID_Roll/preSat Signal'
 * '<S134>' : 'flightController/FCS/Controller/PitchRollControl/PID_Roll/Anti-windup/Passthrough'
 * '<S135>' : 'flightController/FCS/Controller/PitchRollControl/PID_Roll/D Gain/Internal Parameters'
 * '<S136>' : 'flightController/FCS/Controller/PitchRollControl/PID_Roll/Filter/Disc. Forward Euler Filter'
 * '<S137>' : 'flightController/FCS/Controller/PitchRollControl/PID_Roll/Filter ICs/Internal IC - Filter'
 * '<S138>' : 'flightController/FCS/Controller/PitchRollControl/PID_Roll/I Gain/Internal Parameters'
 * '<S139>' : 'flightController/FCS/Controller/PitchRollControl/PID_Roll/Ideal P Gain/Passthrough'
 * '<S140>' : 'flightController/FCS/Controller/PitchRollControl/PID_Roll/Ideal P Gain Fdbk/Disabled'
 * '<S141>' : 'flightController/FCS/Controller/PitchRollControl/PID_Roll/Integrator/Discrete'
 * '<S142>' : 'flightController/FCS/Controller/PitchRollControl/PID_Roll/Integrator ICs/Internal IC'
 * '<S143>' : 'flightController/FCS/Controller/PitchRollControl/PID_Roll/N Copy/Disabled'
 * '<S144>' : 'flightController/FCS/Controller/PitchRollControl/PID_Roll/N Gain/Internal Parameters'
 * '<S145>' : 'flightController/FCS/Controller/PitchRollControl/PID_Roll/P Copy/Disabled'
 * '<S146>' : 'flightController/FCS/Controller/PitchRollControl/PID_Roll/Parallel P Gain/Internal Parameters'
 * '<S147>' : 'flightController/FCS/Controller/PitchRollControl/PID_Roll/Reset Signal/Disabled'
 * '<S148>' : 'flightController/FCS/Controller/PitchRollControl/PID_Roll/Saturation/Passthrough'
 * '<S149>' : 'flightController/FCS/Controller/PitchRollControl/PID_Roll/Saturation Fdbk/Disabled'
 * '<S150>' : 'flightController/FCS/Controller/PitchRollControl/PID_Roll/Sum/Sum_PID'
 * '<S151>' : 'flightController/FCS/Controller/PitchRollControl/PID_Roll/Sum Fdbk/Disabled'
 * '<S152>' : 'flightController/FCS/Controller/PitchRollControl/PID_Roll/Tracking Mode/Disabled'
 * '<S153>' : 'flightController/FCS/Controller/PitchRollControl/PID_Roll/Tracking Mode Sum/Passthrough'
 * '<S154>' : 'flightController/FCS/Controller/PitchRollControl/PID_Roll/Tsamp - Integral/Passthrough'
 * '<S155>' : 'flightController/FCS/Controller/PitchRollControl/PID_Roll/Tsamp - Ngain/Passthrough'
 * '<S156>' : 'flightController/FCS/Controller/PitchRollControl/PID_Roll/postSat Signal/Forward_Path'
 * '<S157>' : 'flightController/FCS/Controller/PitchRollControl/PID_Roll/preSat Signal/Forward_Path'
 * '<S158>' : 'flightController/FCS/Controller/YawControl/Angle Conversion'
 * '<S159>' : 'flightController/FCS/Controller/YawControl/PI_Yaw_100Hz'
 * '<S160>' : 'flightController/FCS/Controller/YawControl/PI_Yaw_100Hz/Anti-windup'
 * '<S161>' : 'flightController/FCS/Controller/YawControl/PI_Yaw_100Hz/D Gain'
 * '<S162>' : 'flightController/FCS/Controller/YawControl/PI_Yaw_100Hz/Filter'
 * '<S163>' : 'flightController/FCS/Controller/YawControl/PI_Yaw_100Hz/Filter ICs'
 * '<S164>' : 'flightController/FCS/Controller/YawControl/PI_Yaw_100Hz/I Gain'
 * '<S165>' : 'flightController/FCS/Controller/YawControl/PI_Yaw_100Hz/Ideal P Gain'
 * '<S166>' : 'flightController/FCS/Controller/YawControl/PI_Yaw_100Hz/Ideal P Gain Fdbk'
 * '<S167>' : 'flightController/FCS/Controller/YawControl/PI_Yaw_100Hz/Integrator'
 * '<S168>' : 'flightController/FCS/Controller/YawControl/PI_Yaw_100Hz/Integrator ICs'
 * '<S169>' : 'flightController/FCS/Controller/YawControl/PI_Yaw_100Hz/N Copy'
 * '<S170>' : 'flightController/FCS/Controller/YawControl/PI_Yaw_100Hz/N Gain'
 * '<S171>' : 'flightController/FCS/Controller/YawControl/PI_Yaw_100Hz/P Copy'
 * '<S172>' : 'flightController/FCS/Controller/YawControl/PI_Yaw_100Hz/Parallel P Gain'
 * '<S173>' : 'flightController/FCS/Controller/YawControl/PI_Yaw_100Hz/Reset Signal'
 * '<S174>' : 'flightController/FCS/Controller/YawControl/PI_Yaw_100Hz/Saturation'
 * '<S175>' : 'flightController/FCS/Controller/YawControl/PI_Yaw_100Hz/Saturation Fdbk'
 * '<S176>' : 'flightController/FCS/Controller/YawControl/PI_Yaw_100Hz/Sum'
 * '<S177>' : 'flightController/FCS/Controller/YawControl/PI_Yaw_100Hz/Sum Fdbk'
 * '<S178>' : 'flightController/FCS/Controller/YawControl/PI_Yaw_100Hz/Tracking Mode'
 * '<S179>' : 'flightController/FCS/Controller/YawControl/PI_Yaw_100Hz/Tracking Mode Sum'
 * '<S180>' : 'flightController/FCS/Controller/YawControl/PI_Yaw_100Hz/Tsamp - Integral'
 * '<S181>' : 'flightController/FCS/Controller/YawControl/PI_Yaw_100Hz/Tsamp - Ngain'
 * '<S182>' : 'flightController/FCS/Controller/YawControl/PI_Yaw_100Hz/postSat Signal'
 * '<S183>' : 'flightController/FCS/Controller/YawControl/PI_Yaw_100Hz/preSat Signal'
 * '<S184>' : 'flightController/FCS/Controller/YawControl/PI_Yaw_100Hz/Anti-windup/Passthrough'
 * '<S185>' : 'flightController/FCS/Controller/YawControl/PI_Yaw_100Hz/D Gain/Disabled'
 * '<S186>' : 'flightController/FCS/Controller/YawControl/PI_Yaw_100Hz/Filter/Disabled'
 * '<S187>' : 'flightController/FCS/Controller/YawControl/PI_Yaw_100Hz/Filter ICs/Disabled'
 * '<S188>' : 'flightController/FCS/Controller/YawControl/PI_Yaw_100Hz/I Gain/Internal Parameters'
 * '<S189>' : 'flightController/FCS/Controller/YawControl/PI_Yaw_100Hz/Ideal P Gain/Passthrough'
 * '<S190>' : 'flightController/FCS/Controller/YawControl/PI_Yaw_100Hz/Ideal P Gain Fdbk/Disabled'
 * '<S191>' : 'flightController/FCS/Controller/YawControl/PI_Yaw_100Hz/Integrator/Discrete'
 * '<S192>' : 'flightController/FCS/Controller/YawControl/PI_Yaw_100Hz/Integrator ICs/Internal IC'
 * '<S193>' : 'flightController/FCS/Controller/YawControl/PI_Yaw_100Hz/N Copy/Disabled wSignal Specification'
 * '<S194>' : 'flightController/FCS/Controller/YawControl/PI_Yaw_100Hz/N Gain/Disabled'
 * '<S195>' : 'flightController/FCS/Controller/YawControl/PI_Yaw_100Hz/P Copy/Disabled'
 * '<S196>' : 'flightController/FCS/Controller/YawControl/PI_Yaw_100Hz/Parallel P Gain/Internal Parameters'
 * '<S197>' : 'flightController/FCS/Controller/YawControl/PI_Yaw_100Hz/Reset Signal/Disabled'
 * '<S198>' : 'flightController/FCS/Controller/YawControl/PI_Yaw_100Hz/Saturation/Passthrough'
 * '<S199>' : 'flightController/FCS/Controller/YawControl/PI_Yaw_100Hz/Saturation Fdbk/Disabled'
 * '<S200>' : 'flightController/FCS/Controller/YawControl/PI_Yaw_100Hz/Sum/Sum_PI'
 * '<S201>' : 'flightController/FCS/Controller/YawControl/PI_Yaw_100Hz/Sum Fdbk/Disabled'
 * '<S202>' : 'flightController/FCS/Controller/YawControl/PI_Yaw_100Hz/Tracking Mode/Disabled'
 * '<S203>' : 'flightController/FCS/Controller/YawControl/PI_Yaw_100Hz/Tracking Mode Sum/Passthrough'
 * '<S204>' : 'flightController/FCS/Controller/YawControl/PI_Yaw_100Hz/Tsamp - Integral/Passthrough'
 * '<S205>' : 'flightController/FCS/Controller/YawControl/PI_Yaw_100Hz/Tsamp - Ngain/Passthrough'
 * '<S206>' : 'flightController/FCS/Controller/YawControl/PI_Yaw_100Hz/postSat Signal/Forward_Path'
 * '<S207>' : 'flightController/FCS/Controller/YawControl/PI_Yaw_100Hz/preSat Signal/Forward_Path'
 * '<S208>' : 'flightController/StatesEstimator/AltitudeEstimator'
 * '<S209>' : 'flightController/StatesEstimator/OrientationEstimator'
 * '<S210>' : 'flightController/StatesEstimator/SensorPreprocessing'
 * '<S211>' : 'flightController/StatesEstimator/AltitudeEstimator/Discrete Derivative'
 * '<S212>' : 'flightController/StatesEstimator/AltitudeEstimator/Quaternions to  Direction Cosine Matrix'
 * '<S213>' : 'flightController/StatesEstimator/AltitudeEstimator/Quaternions to  Direction Cosine Matrix/A11'
 * '<S214>' : 'flightController/StatesEstimator/AltitudeEstimator/Quaternions to  Direction Cosine Matrix/A12'
 * '<S215>' : 'flightController/StatesEstimator/AltitudeEstimator/Quaternions to  Direction Cosine Matrix/A13'
 * '<S216>' : 'flightController/StatesEstimator/AltitudeEstimator/Quaternions to  Direction Cosine Matrix/A21'
 * '<S217>' : 'flightController/StatesEstimator/AltitudeEstimator/Quaternions to  Direction Cosine Matrix/A22'
 * '<S218>' : 'flightController/StatesEstimator/AltitudeEstimator/Quaternions to  Direction Cosine Matrix/A23'
 * '<S219>' : 'flightController/StatesEstimator/AltitudeEstimator/Quaternions to  Direction Cosine Matrix/A31'
 * '<S220>' : 'flightController/StatesEstimator/AltitudeEstimator/Quaternions to  Direction Cosine Matrix/A32'
 * '<S221>' : 'flightController/StatesEstimator/AltitudeEstimator/Quaternions to  Direction Cosine Matrix/A33'
 * '<S222>' : 'flightController/StatesEstimator/AltitudeEstimator/Quaternions to  Direction Cosine Matrix/Create 3x3 Matrix'
 * '<S223>' : 'flightController/StatesEstimator/AltitudeEstimator/Quaternions to  Direction Cosine Matrix/Quaternion Normalize'
 * '<S224>' : 'flightController/StatesEstimator/AltitudeEstimator/Quaternions to  Direction Cosine Matrix/Quaternion Normalize/Quaternion Modulus'
 * '<S225>' : 'flightController/StatesEstimator/AltitudeEstimator/Quaternions to  Direction Cosine Matrix/Quaternion Normalize/Quaternion Modulus/Quaternion Norm'
 * '<S226>' : 'flightController/StatesEstimator/OrientationEstimator/orientation_quat2RPY'
 * '<S227>' : 'flightController/StatesEstimator/OrientationEstimator/orientation_quat2RPY/Angle Calculation'
 * '<S228>' : 'flightController/StatesEstimator/OrientationEstimator/orientation_quat2RPY/Quaternion Normalize'
 * '<S229>' : 'flightController/StatesEstimator/OrientationEstimator/orientation_quat2RPY/Angle Calculation/Protect asincos input'
 * '<S230>' : 'flightController/StatesEstimator/OrientationEstimator/orientation_quat2RPY/Angle Calculation/Protect asincos input/If Action Subsystem'
 * '<S231>' : 'flightController/StatesEstimator/OrientationEstimator/orientation_quat2RPY/Angle Calculation/Protect asincos input/If Action Subsystem1'
 * '<S232>' : 'flightController/StatesEstimator/OrientationEstimator/orientation_quat2RPY/Angle Calculation/Protect asincos input/If Action Subsystem2'
 * '<S233>' : 'flightController/StatesEstimator/OrientationEstimator/orientation_quat2RPY/Quaternion Normalize/Quaternion Modulus'
 * '<S234>' : 'flightController/StatesEstimator/OrientationEstimator/orientation_quat2RPY/Quaternion Normalize/Quaternion Modulus/Quaternion Norm'
 * '<S235>' : 'flightController/StatesEstimator/SensorPreprocessing/preprocessing_RPY2quat'
 * '<S236>' : 'flightController/StatesEstimator/SensorPreprocessing/preprocessing_quat2RPYbias'
 * '<S237>' : 'flightController/StatesEstimator/SensorPreprocessing/preprocessing_quat2RPYsens'
 * '<S238>' : 'flightController/StatesEstimator/SensorPreprocessing/preprocessing_quat2RPYbias/Angle Calculation'
 * '<S239>' : 'flightController/StatesEstimator/SensorPreprocessing/preprocessing_quat2RPYbias/Quaternion Normalize'
 * '<S240>' : 'flightController/StatesEstimator/SensorPreprocessing/preprocessing_quat2RPYbias/Angle Calculation/Protect asincos input'
 * '<S241>' : 'flightController/StatesEstimator/SensorPreprocessing/preprocessing_quat2RPYbias/Angle Calculation/Protect asincos input/If Action Subsystem'
 * '<S242>' : 'flightController/StatesEstimator/SensorPreprocessing/preprocessing_quat2RPYbias/Angle Calculation/Protect asincos input/If Action Subsystem1'
 * '<S243>' : 'flightController/StatesEstimator/SensorPreprocessing/preprocessing_quat2RPYbias/Angle Calculation/Protect asincos input/If Action Subsystem2'
 * '<S244>' : 'flightController/StatesEstimator/SensorPreprocessing/preprocessing_quat2RPYbias/Quaternion Normalize/Quaternion Modulus'
 * '<S245>' : 'flightController/StatesEstimator/SensorPreprocessing/preprocessing_quat2RPYbias/Quaternion Normalize/Quaternion Modulus/Quaternion Norm'
 * '<S246>' : 'flightController/StatesEstimator/SensorPreprocessing/preprocessing_quat2RPYsens/Angle Calculation'
 * '<S247>' : 'flightController/StatesEstimator/SensorPreprocessing/preprocessing_quat2RPYsens/Quaternion Normalize'
 * '<S248>' : 'flightController/StatesEstimator/SensorPreprocessing/preprocessing_quat2RPYsens/Angle Calculation/Protect asincos input'
 * '<S249>' : 'flightController/StatesEstimator/SensorPreprocessing/preprocessing_quat2RPYsens/Angle Calculation/Protect asincos input/If Action Subsystem'
 * '<S250>' : 'flightController/StatesEstimator/SensorPreprocessing/preprocessing_quat2RPYsens/Angle Calculation/Protect asincos input/If Action Subsystem1'
 * '<S251>' : 'flightController/StatesEstimator/SensorPreprocessing/preprocessing_quat2RPYsens/Angle Calculation/Protect asincos input/If Action Subsystem2'
 * '<S252>' : 'flightController/StatesEstimator/SensorPreprocessing/preprocessing_quat2RPYsens/Quaternion Normalize/Quaternion Modulus'
 * '<S253>' : 'flightController/StatesEstimator/SensorPreprocessing/preprocessing_quat2RPYsens/Quaternion Normalize/Quaternion Modulus/Quaternion Norm'
 */
#endif                                 /* RTW_HEADER_flightController_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
