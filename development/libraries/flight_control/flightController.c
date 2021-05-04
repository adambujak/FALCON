/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: flightController.c
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

#include "flightController.h"

const states_estimate_t flightController_rtZstates_estimate_t = {
  0.0F,                                /* x */
  0.0F,                                /* y */
  0.0F,                                /* z */
  0.0F,                                /* dx */
  0.0F,                                /* dy */
  0.0F,                                /* dz */
  0.0F,                                /* yaw */
  0.0F,                                /* pitch */
  0.0F,                                /* roll */
  0.0F,                                /* p */
  0.0F,                                /* q */
  0.0F                                 /* r */
} ;                                    /* states_estimate_t ground */

/* Exported block parameters */
real32_T PID_alt_D = 0.24F;            /* Variable: PID_alt_D
                                        * Referenced by: '<S6>/Alt_D_Gain'
                                        */
real32_T PID_alt_P = 0.64F;            /* Variable: PID_alt_P
                                        * Referenced by: '<S6>/Alt_P_Gain'
                                        */
real32_T PID_pitch_D = 0.005F;         /* Variable: PID_pitch_D
                                        * Referenced by: '<S7>/D_pr'
                                        */
real32_T PID_pitch_P = 0.005F;         /* Variable: PID_pitch_P
                                        * Referenced by: '<S7>/P_pr'
                                        */
real32_T PID_pitch_roll_I = 0.003F;    /* Variable: PID_pitch_roll_I
                                        * Referenced by: '<S7>/I_pr'
                                        */
real32_T PID_yaw_D = 0.14F;            /* Variable: PID_yaw_D
                                        * Referenced by: '<S8>/Yaw_D_Gain'
                                        */
real32_T PID_yaw_P = 0.1F;             /* Variable: PID_yaw_P
                                        * Referenced by: '<S8>/Yaw_P_Gain'
                                        */
const FCS_command_t flightController_rtZFCS_command = { 0U,/* control_type */
  { 0.0F,                              /* yaw_cmd */
    0.0F,                              /* roll_cmd */
    0.0F,                              /* pitch_cmd */
    0.0F                               /* alt_cmd */
  }                                    /* control_input */
};

const sensor_bias_t flightController_rtZsensor_bias = { { 0.0F, 0.0F, 0.0F },/* gyro_bias */
  { 0.0F, 0.0F, 0.0F },                /* accel_bias */
  { 0.7071F, 0.0F, 0.0F, -0.7071F },          /* quat_bias */
  0.0F                                 /* alt_bias */
};

const sensor_data_t flightController_rtZsensor_data = { { 0.0F, 0.0F, 0.0F },/* gyro_data_SI */
  { 0.0F, 0.0F, 0.0F },                /* accel_data_SI */
  { 1.0F, 0.0F, 0.0F, 0.0F },          /* quat_data */
  0.0F                                 /* alt_data_SI */
};

/* Model step function */
void flightController_step(RT_MODEL *const rtM, FCS_command_t *rtU_Commands,
  sensor_bias_t *rtU_Bias, sensor_data_t *rtU_Sensors, states_estimate_t
  *rtY_State_Estim, uint16_T rtY_Throttle[4])
{
  DW *rtDW = rtM->dwork;
  real_T rtb_VectorConcatenate[9];
  real_T rtb_VectorConcatenate_p[3];
  real_T rtb_MotorBL;
  real_T rtb_MotorBR;
  real_T rtb_Saturation_idx_0;
  real_T rtb_Saturation_idx_1;
  real_T rtb_VectorConcatenate_h_tmp;
  real_T rtb_fcn2;
  real_T rtb_fcn3;
  real_T rtb_fcn4;
  real_T rtb_sincos_o2_idx_0;
  real_T rtb_sincos_o2_idx_0_tmp;
  real_T rtb_sincos_o2_idx_0_tmp_0;
  real_T rtb_sincos_o2_idx_2;
  real_T u1_tmp;
  real_T u1_tmp_0;
  int32_T cff;
  int32_T i;
  real32_T Subtract1[3];
  real32_T rtb_VectorConcatenate_0[3];
  real32_T IIR_IMUgyro_r_tmp;
  real32_T Subtract2_idx_0;
  real32_T Subtract2_idx_1;
  real32_T rtb_preprocessing_accel_out_i_0;
  real32_T rtb_preprocessing_accel_out_i_1;
  real32_T rtb_preprocessing_accel_out_idx;
  real32_T rtb_preprocessing_alt_ned;
  real32_T rtb_preprocessing_alt_out;
  real32_T rtb_preprocessing_gyro_out_id_0;
  real32_T rtb_preprocessing_gyro_out_id_1;
  real32_T rtb_preprocessing_gyro_out_idx_;

  /* Outputs for Atomic SubSystem: '<Root>/StatesEstimator' */
  /* Outputs for Atomic SubSystem: '<S2>/SensorPreprocessing' */
  /* Sqrt: '<S56>/sqrt' incorporates:
   *  DataTypeConversion: '<S14>/preprocessing_quat2doubleSens'
   *  Inport: '<Root>/Sensors'
   *  Product: '<S57>/Product'
   *  Product: '<S57>/Product1'
   *  Product: '<S57>/Product2'
   *  Product: '<S57>/Product3'
   *  Sum: '<S57>/Sum'
   */
  rtb_fcn2 = sqrt((((real_T)rtU_Sensors->quat_data[0] * rtU_Sensors->quat_data[0]
                    + (real_T)rtU_Sensors->quat_data[1] * rtU_Sensors->
                    quat_data[1]) + (real_T)rtU_Sensors->quat_data[2] *
                   rtU_Sensors->quat_data[2]) + (real_T)rtU_Sensors->quat_data[3]
                  * rtU_Sensors->quat_data[3]);

  /* Product: '<S51>/Product' incorporates:
   *  DataTypeConversion: '<S14>/preprocessing_quat2doubleSens'
   *  Inport: '<Root>/Sensors'
   */
  rtb_MotorBR = rtU_Sensors->quat_data[0] / rtb_fcn2;

  /* Product: '<S51>/Product1' incorporates:
   *  DataTypeConversion: '<S14>/preprocessing_quat2doubleSens'
   *  Inport: '<Root>/Sensors'
   */
  rtb_MotorBL = rtU_Sensors->quat_data[1] / rtb_fcn2;

  /* Product: '<S51>/Product2' incorporates:
   *  DataTypeConversion: '<S14>/preprocessing_quat2doubleSens'
   *  Inport: '<Root>/Sensors'
   */
  rtb_fcn4 = rtU_Sensors->quat_data[2] / rtb_fcn2;

  /* Product: '<S51>/Product3' incorporates:
   *  DataTypeConversion: '<S14>/preprocessing_quat2doubleSens'
   *  Inport: '<Root>/Sensors'
   */
  rtb_fcn2 = rtU_Sensors->quat_data[3] / rtb_fcn2;

  /* Fcn: '<S41>/fcn2' incorporates:
   *  Fcn: '<S41>/fcn5'
   */
  rtb_Saturation_idx_1 = rtb_MotorBR * rtb_MotorBR;
  rtb_sincos_o2_idx_2 = rtb_MotorBL * rtb_MotorBL;
  rtb_sincos_o2_idx_0_tmp = rtb_fcn4 * rtb_fcn4;
  rtb_sincos_o2_idx_0_tmp_0 = rtb_fcn2 * rtb_fcn2;

  /* Trigonometry: '<S50>/Trigonometric Function1' incorporates:
   *  Fcn: '<S41>/fcn1'
   *  Fcn: '<S41>/fcn2'
   */
  rtb_sincos_o2_idx_0 = atan2((rtb_MotorBL * rtb_fcn4 + rtb_MotorBR * rtb_fcn2) *
    2.0, ((rtb_Saturation_idx_1 + rtb_sincos_o2_idx_2) - rtb_sincos_o2_idx_0_tmp)
    - rtb_sincos_o2_idx_0_tmp_0);

  /* Fcn: '<S41>/fcn3' */
  rtb_fcn3 = (rtb_MotorBL * rtb_fcn2 - rtb_MotorBR * rtb_fcn4) * -2.0;

  /* If: '<S52>/If' incorporates:
   *  Constant: '<S53>/Constant'
   *  Constant: '<S54>/Constant'
   *  Inport: '<S55>/In'
   */
  if (rtb_fcn3 > 1.0) {
    /* Outputs for IfAction SubSystem: '<S52>/If Action Subsystem' incorporates:
     *  ActionPort: '<S53>/Action Port'
     */
    rtb_Saturation_idx_0 = 1.0;

    /* End of Outputs for SubSystem: '<S52>/If Action Subsystem' */
  } else if (rtb_fcn3 < -1.0) {
    /* Outputs for IfAction SubSystem: '<S52>/If Action Subsystem1' incorporates:
     *  ActionPort: '<S54>/Action Port'
     */
    rtb_Saturation_idx_0 = 1.0;

    /* End of Outputs for SubSystem: '<S52>/If Action Subsystem1' */
  } else {
    /* Outputs for IfAction SubSystem: '<S52>/If Action Subsystem2' incorporates:
     *  ActionPort: '<S55>/Action Port'
     */
    rtb_Saturation_idx_0 = rtb_fcn3;

    /* End of Outputs for SubSystem: '<S52>/If Action Subsystem2' */
  }

  /* End of If: '<S52>/If' */

  /* Trigonometry: '<S50>/Trigonometric Function3' incorporates:
   *  Fcn: '<S41>/fcn4'
   *  Fcn: '<S41>/fcn5'
   */
  rtb_sincos_o2_idx_2 = atan2((rtb_fcn4 * rtb_fcn2 + rtb_MotorBR * rtb_MotorBL) *
    2.0, ((rtb_Saturation_idx_1 - rtb_sincos_o2_idx_2) - rtb_sincos_o2_idx_0_tmp)
    + rtb_sincos_o2_idx_0_tmp_0);

  /* Sqrt: '<S48>/sqrt' incorporates:
   *  DataTypeConversion: '<S14>/preprocessing_quat2doubleBias'
   *  Inport: '<Root>/Bias'
   *  Product: '<S49>/Product'
   *  Product: '<S49>/Product1'
   *  Product: '<S49>/Product2'
   *  Product: '<S49>/Product3'
   *  Sum: '<S49>/Sum'
   */
  rtb_fcn3 = sqrt((((real_T)rtU_Bias->quat_bias[0] * rtU_Bias->quat_bias[0] +
                    (real_T)rtU_Bias->quat_bias[1] * rtU_Bias->quat_bias[1]) +
                   (real_T)rtU_Bias->quat_bias[2] * rtU_Bias->quat_bias[2]) +
                  (real_T)rtU_Bias->quat_bias[3] * rtU_Bias->quat_bias[3]);

  /* Product: '<S43>/Product' incorporates:
   *  DataTypeConversion: '<S14>/preprocessing_quat2doubleBias'
   *  Inport: '<Root>/Bias'
   */
  rtb_fcn2 = rtU_Bias->quat_bias[0] / rtb_fcn3;

  /* Product: '<S43>/Product1' incorporates:
   *  DataTypeConversion: '<S14>/preprocessing_quat2doubleBias'
   *  Inport: '<Root>/Bias'
   */
  rtb_MotorBR = rtU_Bias->quat_bias[1] / rtb_fcn3;

  /* Product: '<S43>/Product2' incorporates:
   *  DataTypeConversion: '<S14>/preprocessing_quat2doubleBias'
   *  Inport: '<Root>/Bias'
   */
  rtb_MotorBL = rtU_Bias->quat_bias[2] / rtb_fcn3;

  /* Product: '<S43>/Product3' incorporates:
   *  DataTypeConversion: '<S14>/preprocessing_quat2doubleBias'
   *  Inport: '<Root>/Bias'
   */
  rtb_fcn3 = rtU_Bias->quat_bias[3] / rtb_fcn3;

  /* Fcn: '<S40>/fcn2' incorporates:
   *  Fcn: '<S40>/fcn5'
   */
  rtb_sincos_o2_idx_0_tmp = rtb_fcn2 * rtb_fcn2;
  rtb_sincos_o2_idx_0_tmp_0 = rtb_MotorBR * rtb_MotorBR;
  u1_tmp = rtb_MotorBL * rtb_MotorBL;
  u1_tmp_0 = rtb_fcn3 * rtb_fcn3;

  /* Fcn: '<S40>/fcn3' */
  rtb_fcn4 = (rtb_MotorBR * rtb_fcn3 - rtb_fcn2 * rtb_MotorBL) * -2.0;

  /* If: '<S44>/If' incorporates:
   *  Constant: '<S45>/Constant'
   *  Constant: '<S46>/Constant'
   */
  if (rtb_fcn4 > 1.0) {
    /* Outputs for IfAction SubSystem: '<S44>/If Action Subsystem' incorporates:
     *  ActionPort: '<S45>/Action Port'
     */
    rtb_fcn4 = 1.0;

    /* End of Outputs for SubSystem: '<S44>/If Action Subsystem' */
  } else {
    if (rtb_fcn4 < -1.0) {
      /* Outputs for IfAction SubSystem: '<S44>/If Action Subsystem1' incorporates:
       *  ActionPort: '<S46>/Action Port'
       */
      rtb_fcn4 = 1.0;

      /* End of Outputs for SubSystem: '<S44>/If Action Subsystem1' */
    }
  }

  /* End of If: '<S44>/If' */

  /* Gain: '<S39>/1//2' incorporates:
   *  Fcn: '<S40>/fcn1'
   *  Fcn: '<S40>/fcn2'
   *  Sum: '<S14>/Subtract'
   *  Trigonometry: '<S42>/Trigonometric Function1'
   */
  rtb_Saturation_idx_1 = (rtb_sincos_o2_idx_0 - atan2((rtb_MotorBR * rtb_MotorBL
    + rtb_fcn2 * rtb_fcn3) * 2.0, ((rtb_sincos_o2_idx_0_tmp +
    rtb_sincos_o2_idx_0_tmp_0) - u1_tmp) - u1_tmp_0)) * 0.5;

  /* Sum: '<S14>/Subtract1' incorporates:
   *  Inport: '<Root>/Bias'
   *  Inport: '<Root>/Sensors'
   */
  Subtract1[0] = rtU_Sensors->accel_data_SI[0] - rtU_Bias->accel_bias[0];

  /* Trigonometry: '<S39>/sincos' */
  rtb_sincos_o2_idx_0 = cos(rtb_Saturation_idx_1);
  rtb_VectorConcatenate_p[0] = sin(rtb_Saturation_idx_1);

  /* Gain: '<S39>/1//2' incorporates:
   *  Sum: '<S14>/Subtract'
   *  Trigonometry: '<S42>/trigFcn'
   *  Trigonometry: '<S50>/trigFcn'
   */
  rtb_Saturation_idx_1 = (asin(rtb_Saturation_idx_0) - asin(rtb_fcn4)) * 0.5;

  /* Sum: '<S14>/Subtract1' incorporates:
   *  Inport: '<Root>/Bias'
   *  Inport: '<Root>/Sensors'
   */
  Subtract1[1] = rtU_Sensors->accel_data_SI[1] - rtU_Bias->accel_bias[1];

  /* Trigonometry: '<S39>/sincos' */
  rtb_fcn4 = cos(rtb_Saturation_idx_1);
  rtb_VectorConcatenate_p[1] = sin(rtb_Saturation_idx_1);

  /* Gain: '<S39>/1//2' incorporates:
   *  Fcn: '<S40>/fcn4'
   *  Fcn: '<S40>/fcn5'
   *  Sum: '<S14>/Subtract'
   *  Trigonometry: '<S42>/Trigonometric Function3'
   */
  rtb_Saturation_idx_1 = (rtb_sincos_o2_idx_2 - atan2((rtb_MotorBL * rtb_fcn3 +
    rtb_fcn2 * rtb_MotorBR) * 2.0, ((rtb_sincos_o2_idx_0_tmp -
    rtb_sincos_o2_idx_0_tmp_0) - u1_tmp) + u1_tmp_0)) * 0.5;

  /* Trigonometry: '<S39>/sincos' */
  rtb_MotorBR = cos(rtb_Saturation_idx_1);

  /* Sum: '<S14>/Subtract1' incorporates:
   *  Inport: '<Root>/Bias'
   *  Inport: '<Root>/Sensors'
   */
  Subtract1[2] = rtU_Sensors->accel_data_SI[2] - rtU_Bias->accel_bias[2];

  /* Trigonometry: '<S39>/sincos' */
  rtb_VectorConcatenate_p[2] = sin(rtb_Saturation_idx_1);

  /* DiscreteFir: '<S14>/FIR_IMUaccel' incorporates:
   *  Sum: '<S14>/Subtract1'
   */
  IIR_IMUgyro_r_tmp = Subtract1[0] * 0.0264077242F;
  cff = 1;
  for (i = rtDW->FIR_IMUaccel_circBuf; i < 5; i++) {
    IIR_IMUgyro_r_tmp += rtDW->FIR_IMUaccel_states[i] *
      rtConstP.FIR_IMUaccel_Coefficients[cff];
    cff++;
  }

  for (i = 0; i < rtDW->FIR_IMUaccel_circBuf; i++) {
    IIR_IMUgyro_r_tmp += rtDW->FIR_IMUaccel_states[i] *
      rtConstP.FIR_IMUaccel_Coefficients[cff];
    cff++;
  }

  rtb_preprocessing_accel_out_idx = IIR_IMUgyro_r_tmp;

  /* Sum: '<S14>/Subtract2' incorporates:
   *  Inport: '<Root>/Bias'
   *  Inport: '<Root>/Sensors'
   */
  Subtract2_idx_0 = rtU_Sensors->gyro_data_SI[0] - rtU_Bias->gyro_bias[0];

  /* DiscreteFir: '<S14>/FIR_IMUaccel' incorporates:
   *  Sum: '<S14>/Subtract1'
   */
  IIR_IMUgyro_r_tmp = Subtract1[1] * 0.0264077242F;
  cff = 1;
  for (i = rtDW->FIR_IMUaccel_circBuf; i < 5; i++) {
    IIR_IMUgyro_r_tmp += rtDW->FIR_IMUaccel_states[i + 5] *
      rtConstP.FIR_IMUaccel_Coefficients[cff];
    cff++;
  }

  for (i = 0; i < rtDW->FIR_IMUaccel_circBuf; i++) {
    IIR_IMUgyro_r_tmp += rtDW->FIR_IMUaccel_states[i + 5] *
      rtConstP.FIR_IMUaccel_Coefficients[cff];
    cff++;
  }

  rtb_preprocessing_accel_out_i_0 = IIR_IMUgyro_r_tmp;

  /* Sum: '<S14>/Subtract2' incorporates:
   *  Inport: '<Root>/Bias'
   *  Inport: '<Root>/Sensors'
   */
  Subtract2_idx_1 = rtU_Sensors->gyro_data_SI[1] - rtU_Bias->gyro_bias[1];

  /* DiscreteFir: '<S14>/FIR_IMUaccel' incorporates:
   *  Sum: '<S14>/Subtract1'
   */
  IIR_IMUgyro_r_tmp = Subtract1[2] * 0.0264077242F;
  cff = 1;
  for (i = rtDW->FIR_IMUaccel_circBuf; i < 5; i++) {
    IIR_IMUgyro_r_tmp += rtDW->FIR_IMUaccel_states[i + 10] *
      rtConstP.FIR_IMUaccel_Coefficients[cff];
    cff++;
  }

  for (i = 0; i < rtDW->FIR_IMUaccel_circBuf; i++) {
    IIR_IMUgyro_r_tmp += rtDW->FIR_IMUaccel_states[i + 10] *
      rtConstP.FIR_IMUaccel_Coefficients[cff];
    cff++;
  }

  rtb_preprocessing_accel_out_i_1 = IIR_IMUgyro_r_tmp;

  /* DiscreteFir: '<S14>/FIR_IMUgyro' */
  IIR_IMUgyro_r_tmp = Subtract2_idx_0 * -2.48169962E-18F;
  cff = 1;
  for (i = rtDW->FIR_IMUgyro_circBuf; i < 5; i++) {
    IIR_IMUgyro_r_tmp += rtDW->FIR_IMUgyro_states[i] *
      rtConstP.FIR_IMUgyro_Coefficients[cff];
    cff++;
  }

  for (i = 0; i < rtDW->FIR_IMUgyro_circBuf; i++) {
    IIR_IMUgyro_r_tmp += rtDW->FIR_IMUgyro_states[i] *
      rtConstP.FIR_IMUgyro_Coefficients[cff];
    cff++;
  }

  rtb_preprocessing_gyro_out_id_1 = IIR_IMUgyro_r_tmp;
  IIR_IMUgyro_r_tmp = Subtract2_idx_1 * -2.48169962E-18F;
  cff = 1;
  for (i = rtDW->FIR_IMUgyro_circBuf; i < 5; i++) {
    IIR_IMUgyro_r_tmp += rtDW->FIR_IMUgyro_states[i + 5] *
      rtConstP.FIR_IMUgyro_Coefficients[cff];
    cff++;
  }

  for (i = 0; i < rtDW->FIR_IMUgyro_circBuf; i++) {
    IIR_IMUgyro_r_tmp += rtDW->FIR_IMUgyro_states[i + 5] *
      rtConstP.FIR_IMUgyro_Coefficients[cff];
    cff++;
  }

  rtb_preprocessing_gyro_out_id_0 = IIR_IMUgyro_r_tmp;

  /* End of DiscreteFir: '<S14>/FIR_IMUgyro' */

  /* DiscreteFilter: '<S14>/IIR_IMUgyro_r' incorporates:
   *  Inport: '<Root>/Bias'
   *  Inport: '<Root>/Sensors'
   *  Sum: '<S14>/Subtract2'
   */
  IIR_IMUgyro_r_tmp = rtU_Sensors->gyro_data_SI[2] - rtU_Bias->gyro_bias[2];
  cff = 1;
  for (i = 0; i < 5; i++) {
    IIR_IMUgyro_r_tmp -= rtConstP.IIR_IMUgyro_r_DenCoef[cff] *
      rtDW->IIR_IMUgyro_r_states[i];
    cff++;
  }

  rtb_preprocessing_alt_ned = 0.282124132F * IIR_IMUgyro_r_tmp;
  cff = 1;
  for (i = 0; i < 5; i++) {
    rtb_preprocessing_alt_ned += rtConstP.IIR_IMUgyro_r_NumCoef[cff] *
      rtDW->IIR_IMUgyro_r_states[i];
    cff++;
  }

  rtb_preprocessing_gyro_out_idx_ = rtb_preprocessing_alt_ned;

  /* End of DiscreteFilter: '<S14>/IIR_IMUgyro_r' */

  /* DiscreteFilter: '<S14>/pressureFilter_IIR' incorporates:
   *  Gain: '<S14>/Alt2NED'
   *  Inport: '<Root>/Bias'
   *  Inport: '<Root>/Sensors'
   *  Sum: '<S14>/Subtract3'
   */
  rtb_preprocessing_alt_ned = ((-(rtU_Sensors->alt_data_SI - rtU_Bias->alt_bias)
    - -2.93717074F * rtDW->pressureFilter_IIR_states[0]) - 2.87629962F *
    rtDW->pressureFilter_IIR_states[1]) - -0.939098954F *
    rtDW->pressureFilter_IIR_states[2];
  rtb_preprocessing_alt_out = ((3.75683794E-6F * rtb_preprocessing_alt_ned +
    1.12705138E-5F * rtDW->pressureFilter_IIR_states[0]) + 1.12705138E-5F *
    rtDW->pressureFilter_IIR_states[1]) + 3.75683794E-6F *
    rtDW->pressureFilter_IIR_states[2];

  /* Update for DiscreteFir: '<S14>/FIR_IMUaccel' incorporates:
   *  Sum: '<S14>/Subtract1'
   */
  /* Update circular buffer index */
  rtDW->FIR_IMUaccel_circBuf--;
  if (rtDW->FIR_IMUaccel_circBuf < 0) {
    rtDW->FIR_IMUaccel_circBuf = 4;
  }

  /* Update circular buffer */
  rtDW->FIR_IMUaccel_states[rtDW->FIR_IMUaccel_circBuf] = Subtract1[0];
  rtDW->FIR_IMUaccel_states[rtDW->FIR_IMUaccel_circBuf + 5] = Subtract1[1];
  rtDW->FIR_IMUaccel_states[rtDW->FIR_IMUaccel_circBuf + 10] = Subtract1[2];

  /* End of Update for DiscreteFir: '<S14>/FIR_IMUaccel' */

  /* Update for DiscreteFir: '<S14>/FIR_IMUgyro' */
  /* Update circular buffer index */
  rtDW->FIR_IMUgyro_circBuf--;
  if (rtDW->FIR_IMUgyro_circBuf < 0) {
    rtDW->FIR_IMUgyro_circBuf = 4;
  }

  /* Update circular buffer */
  rtDW->FIR_IMUgyro_states[rtDW->FIR_IMUgyro_circBuf] = Subtract2_idx_0;
  rtDW->FIR_IMUgyro_states[rtDW->FIR_IMUgyro_circBuf + 5] = Subtract2_idx_1;

  /* End of Update for DiscreteFir: '<S14>/FIR_IMUgyro' */

  /* Update for DiscreteFilter: '<S14>/IIR_IMUgyro_r' */
  rtDW->IIR_IMUgyro_r_states[4] = rtDW->IIR_IMUgyro_r_states[3];
  rtDW->IIR_IMUgyro_r_states[3] = rtDW->IIR_IMUgyro_r_states[2];
  rtDW->IIR_IMUgyro_r_states[2] = rtDW->IIR_IMUgyro_r_states[1];
  rtDW->IIR_IMUgyro_r_states[1] = rtDW->IIR_IMUgyro_r_states[0];
  rtDW->IIR_IMUgyro_r_states[0] = IIR_IMUgyro_r_tmp;

  /* Update for DiscreteFilter: '<S14>/pressureFilter_IIR' */
  rtDW->pressureFilter_IIR_states[2] = rtDW->pressureFilter_IIR_states[1];
  rtDW->pressureFilter_IIR_states[1] = rtDW->pressureFilter_IIR_states[0];
  rtDW->pressureFilter_IIR_states[0] = rtb_preprocessing_alt_ned;

  /* Fcn: '<S39>/q0' incorporates:
   *  Fcn: '<S39>/q1'
   */
  rtb_fcn2 = rtb_sincos_o2_idx_0 * rtb_fcn4;
  rtb_MotorBL = rtb_VectorConcatenate_p[0] * rtb_VectorConcatenate_p[1];

  /* Outputs for Atomic SubSystem: '<S2>/AltitudeEstimator' */
  /* Outputs for Atomic SubSystem: '<S2>/OrientationEstimator' */
  /* DataTypeConversion: '<S13>/orientation_quat2double' incorporates:
   *  DataTypeConversion: '<S12>/altitude_quat2double'
   *  DataTypeConversion: '<S14>/preprocessing_quat2single'
   *  Fcn: '<S39>/q0'
   *  Fcn: '<S39>/q1'
   */
  rtb_Saturation_idx_0 = (real32_T)(rtb_fcn2 * rtb_MotorBR + rtb_MotorBL *
    rtb_VectorConcatenate_p[2]);
  rtb_Saturation_idx_1 = (real32_T)(rtb_fcn2 * rtb_VectorConcatenate_p[2] -
    rtb_MotorBL * rtb_MotorBR);

  /* End of Outputs for SubSystem: '<S2>/OrientationEstimator' */
  /* End of Outputs for SubSystem: '<S2>/AltitudeEstimator' */

  /* Fcn: '<S39>/q2' incorporates:
   *  Fcn: '<S39>/q0'
   *  Fcn: '<S39>/q3'
   */
  rtb_fcn2 = rtb_VectorConcatenate_p[0] * rtb_fcn4;
  rtb_MotorBL = rtb_sincos_o2_idx_0 * rtb_VectorConcatenate_p[1];

  /* Outputs for Atomic SubSystem: '<S2>/AltitudeEstimator' */
  /* Outputs for Atomic SubSystem: '<S2>/OrientationEstimator' */
  /* DataTypeConversion: '<S13>/orientation_quat2double' incorporates:
   *  DataTypeConversion: '<S12>/altitude_quat2double'
   *  DataTypeConversion: '<S14>/preprocessing_quat2single'
   *  Fcn: '<S39>/q0'
   *  Fcn: '<S39>/q2'
   *  Fcn: '<S39>/q3'
   */
  rtb_sincos_o2_idx_0 = (real32_T)(rtb_MotorBL * rtb_MotorBR + rtb_fcn2 *
    rtb_VectorConcatenate_p[2]);
  rtb_sincos_o2_idx_2 = (real32_T)(rtb_fcn2 * rtb_MotorBR - rtb_MotorBL *
    rtb_VectorConcatenate_p[2]);

  /* End of Outputs for SubSystem: '<S2>/SensorPreprocessing' */

  /* Sqrt: '<S37>/sqrt' incorporates:
   *  DataTypeConversion: '<S13>/orientation_quat2double'
   *  Product: '<S38>/Product'
   *  Product: '<S38>/Product1'
   *  Product: '<S38>/Product2'
   *  Product: '<S38>/Product3'
   *  Sqrt: '<S28>/sqrt'
   *  Sum: '<S38>/Sum'
   */
  rtb_sincos_o2_idx_0_tmp = sqrt(((rtb_Saturation_idx_0 * rtb_Saturation_idx_0 +
    rtb_Saturation_idx_1 * rtb_Saturation_idx_1) + rtb_sincos_o2_idx_0 *
    rtb_sincos_o2_idx_0) + rtb_sincos_o2_idx_2 * rtb_sincos_o2_idx_2);

  /* End of Outputs for SubSystem: '<S2>/AltitudeEstimator' */

  /* Product: '<S32>/Product' incorporates:
   *  DataTypeConversion: '<S13>/orientation_quat2double'
   *  Sqrt: '<S37>/sqrt'
   */
  rtb_fcn2 = rtb_Saturation_idx_0 / rtb_sincos_o2_idx_0_tmp;

  /* Product: '<S32>/Product1' incorporates:
   *  DataTypeConversion: '<S13>/orientation_quat2double'
   *  Sqrt: '<S37>/sqrt'
   */
  rtb_MotorBR = rtb_Saturation_idx_1 / rtb_sincos_o2_idx_0_tmp;

  /* Product: '<S32>/Product2' incorporates:
   *  DataTypeConversion: '<S13>/orientation_quat2double'
   *  Sqrt: '<S37>/sqrt'
   */
  rtb_MotorBL = rtb_sincos_o2_idx_0 / rtb_sincos_o2_idx_0_tmp;

  /* Product: '<S32>/Product3' incorporates:
   *  DataTypeConversion: '<S13>/orientation_quat2double'
   *  Sqrt: '<S37>/sqrt'
   */
  rtb_fcn3 = rtb_sincos_o2_idx_2 / rtb_sincos_o2_idx_0_tmp;

  /* Fcn: '<S30>/fcn2' incorporates:
   *  Fcn: '<S30>/fcn5'
   */
  rtb_sincos_o2_idx_0_tmp_0 = rtb_fcn2 * rtb_fcn2;
  u1_tmp = rtb_MotorBR * rtb_MotorBR;
  u1_tmp_0 = rtb_MotorBL * rtb_MotorBL;
  rtb_VectorConcatenate_h_tmp = rtb_fcn3 * rtb_fcn3;

  /* Trigonometry: '<S31>/Trigonometric Function1' incorporates:
   *  Fcn: '<S30>/fcn1'
   *  Fcn: '<S30>/fcn2'
   */
  rtb_VectorConcatenate_p[0] = atan2((rtb_MotorBR * rtb_MotorBL + rtb_fcn2 *
    rtb_fcn3) * 2.0, ((rtb_sincos_o2_idx_0_tmp_0 + u1_tmp) - u1_tmp_0) -
    rtb_VectorConcatenate_h_tmp);

  /* Fcn: '<S30>/fcn3' */
  rtb_fcn4 = (rtb_MotorBR * rtb_fcn3 - rtb_fcn2 * rtb_MotorBL) * -2.0;

  /* If: '<S33>/If' incorporates:
   *  Constant: '<S34>/Constant'
   *  Constant: '<S35>/Constant'
   */
  if (rtb_fcn4 > 1.0) {
    /* Outputs for IfAction SubSystem: '<S33>/If Action Subsystem' incorporates:
     *  ActionPort: '<S34>/Action Port'
     */
    rtb_fcn4 = 1.0;

    /* End of Outputs for SubSystem: '<S33>/If Action Subsystem' */
  } else {
    if (rtb_fcn4 < -1.0) {
      /* Outputs for IfAction SubSystem: '<S33>/If Action Subsystem1' incorporates:
       *  ActionPort: '<S35>/Action Port'
       */
      rtb_fcn4 = 1.0;

      /* End of Outputs for SubSystem: '<S33>/If Action Subsystem1' */
    }
  }

  /* End of If: '<S33>/If' */

  /* Trigonometry: '<S31>/trigFcn' */
  rtb_VectorConcatenate_p[1] = asin(rtb_fcn4);

  /* Trigonometry: '<S31>/Trigonometric Function3' incorporates:
   *  Fcn: '<S30>/fcn4'
   *  Fcn: '<S30>/fcn5'
   */
  rtb_VectorConcatenate_p[2] = atan2((rtb_MotorBL * rtb_fcn3 + rtb_fcn2 *
    rtb_MotorBR) * 2.0, ((rtb_sincos_o2_idx_0_tmp_0 - u1_tmp) - u1_tmp_0) +
    rtb_VectorConcatenate_h_tmp);

  /* End of Outputs for SubSystem: '<S2>/OrientationEstimator' */

  /* Outputs for Atomic SubSystem: '<S2>/AltitudeEstimator' */
  /* Product: '<S27>/Product3' incorporates:
   *  DataTypeConversion: '<S12>/altitude_quat2double'
   */
  rtb_fcn2 = rtb_sincos_o2_idx_2 / rtb_sincos_o2_idx_0_tmp;

  /* Product: '<S27>/Product2' incorporates:
   *  DataTypeConversion: '<S12>/altitude_quat2double'
   */
  rtb_MotorBR = rtb_sincos_o2_idx_0 / rtb_sincos_o2_idx_0_tmp;

  /* Product: '<S27>/Product1' incorporates:
   *  DataTypeConversion: '<S12>/altitude_quat2double'
   */
  rtb_MotorBL = rtb_Saturation_idx_1 / rtb_sincos_o2_idx_0_tmp;

  /* Product: '<S27>/Product' incorporates:
   *  DataTypeConversion: '<S12>/altitude_quat2double'
   */
  rtb_fcn3 = rtb_Saturation_idx_0 / rtb_sincos_o2_idx_0_tmp;

  /* Product: '<S17>/Product3' incorporates:
   *  Product: '<S21>/Product3'
   */
  rtb_fcn4 = rtb_fcn3 * rtb_fcn3;

  /* Product: '<S17>/Product2' incorporates:
   *  Product: '<S21>/Product2'
   */
  rtb_sincos_o2_idx_0 = rtb_MotorBL * rtb_MotorBL;

  /* Product: '<S17>/Product1' incorporates:
   *  Product: '<S21>/Product1'
   *  Product: '<S25>/Product1'
   */
  rtb_Saturation_idx_0 = rtb_MotorBR * rtb_MotorBR;

  /* Product: '<S17>/Product' incorporates:
   *  Product: '<S21>/Product'
   *  Product: '<S25>/Product'
   */
  rtb_Saturation_idx_1 = rtb_fcn2 * rtb_fcn2;

  /* Sum: '<S17>/Sum' incorporates:
   *  Product: '<S17>/Product'
   *  Product: '<S17>/Product1'
   *  Product: '<S17>/Product2'
   *  Product: '<S17>/Product3'
   */
  rtb_VectorConcatenate[0] = ((rtb_fcn4 + rtb_sincos_o2_idx_0) -
    rtb_Saturation_idx_0) - rtb_Saturation_idx_1;

  /* Product: '<S18>/Product2' incorporates:
   *  Product: '<S20>/Product2'
   */
  rtb_sincos_o2_idx_2 = rtb_MotorBL * rtb_MotorBR;

  /* Product: '<S18>/Product3' incorporates:
   *  Product: '<S20>/Product3'
   */
  rtb_sincos_o2_idx_0_tmp = rtb_fcn2 * rtb_fcn3;

  /* Gain: '<S18>/Gain' incorporates:
   *  Product: '<S18>/Product2'
   *  Product: '<S18>/Product3'
   *  Sum: '<S18>/Sum'
   */
  rtb_VectorConcatenate[3] = (rtb_sincos_o2_idx_0_tmp + rtb_sincos_o2_idx_2) *
    2.0;

  /* Product: '<S19>/Product1' incorporates:
   *  Product: '<S23>/Product1'
   */
  rtb_sincos_o2_idx_0_tmp_0 = rtb_fcn3 * rtb_MotorBR;

  /* Product: '<S19>/Product2' incorporates:
   *  Product: '<S23>/Product2'
   */
  u1_tmp = rtb_MotorBL * rtb_fcn2;

  /* Gain: '<S19>/Gain' incorporates:
   *  Product: '<S19>/Product1'
   *  Product: '<S19>/Product2'
   *  Sum: '<S19>/Sum'
   */
  rtb_VectorConcatenate[6] = (u1_tmp - rtb_sincos_o2_idx_0_tmp_0) * 2.0;

  /* Gain: '<S20>/Gain' incorporates:
   *  Sum: '<S20>/Sum'
   */
  rtb_VectorConcatenate[1] = (rtb_sincos_o2_idx_2 - rtb_sincos_o2_idx_0_tmp) *
    2.0;

  /* Sum: '<S21>/Sum' incorporates:
   *  Sum: '<S25>/Sum'
   */
  rtb_fcn4 -= rtb_sincos_o2_idx_0;
  rtb_VectorConcatenate[4] = (rtb_fcn4 + rtb_Saturation_idx_0) -
    rtb_Saturation_idx_1;

  /* Product: '<S22>/Product2' incorporates:
   *  Product: '<S24>/Product2'
   */
  rtb_sincos_o2_idx_0 = rtb_MotorBR * rtb_fcn2;

  /* Product: '<S22>/Product1' incorporates:
   *  Product: '<S24>/Product1'
   */
  rtb_sincos_o2_idx_2 = rtb_fcn3 * rtb_MotorBL;

  /* Gain: '<S22>/Gain' incorporates:
   *  Product: '<S22>/Product1'
   *  Product: '<S22>/Product2'
   *  Sum: '<S22>/Sum'
   */
  rtb_VectorConcatenate[7] = (rtb_sincos_o2_idx_2 + rtb_sincos_o2_idx_0) * 2.0;

  /* Gain: '<S23>/Gain' incorporates:
   *  Sum: '<S23>/Sum'
   */
  rtb_VectorConcatenate[2] = (rtb_sincos_o2_idx_0_tmp_0 + u1_tmp) * 2.0;

  /* Gain: '<S24>/Gain' incorporates:
   *  Sum: '<S24>/Sum'
   */
  rtb_VectorConcatenate[5] = (rtb_sincos_o2_idx_0 - rtb_sincos_o2_idx_2) * 2.0;

  /* Sum: '<S25>/Sum' */
  rtb_VectorConcatenate[8] = (rtb_fcn4 - rtb_Saturation_idx_0) +
    rtb_Saturation_idx_1;

  /* SampleTimeMath: '<S15>/TSamp'
   *
   * About '<S15>/TSamp':
   *  y = u * K where K = 1 / ( w * Ts )
   */
  rtb_preprocessing_alt_ned = rtb_preprocessing_alt_out * 100.0F;

  /* Outputs for Atomic SubSystem: '<S2>/OrientationEstimator' */
  for (i = 0; i < 3; i++) {
    /* DataTypeConversion: '<S13>/orientation_RPY2single' */
    Subtract1[i] = (real32_T)rtb_VectorConcatenate_p[i];

    /* Product: '<S12>/altitude_matrix_multiply' incorporates:
     *  Concatenate: '<S26>/Vector Concatenate'
     *  DataTypeConversion: '<S12>/altitude_DCM2single'
     *  DiscreteFir: '<S14>/FIR_IMUaccel'
     */
    rtb_VectorConcatenate_0[i] = (real32_T)rtb_VectorConcatenate[3 * i + 2] *
      rtb_preprocessing_accel_out_i_1 + ((real32_T)rtb_VectorConcatenate[3 * i +
      1] * rtb_preprocessing_accel_out_i_0 + (real32_T)rtb_VectorConcatenate[3 *
      i] * rtb_preprocessing_accel_out_idx);
  }

  /* End of Outputs for SubSystem: '<S2>/OrientationEstimator' */

  /* Sum: '<S12>/altitude_velocity_baroCorrection' incorporates:
   *  Delay: '<S12>/altitude_delay0'
   *  Gain: '<S12>/altitude_Ts_Gain0'
   *  Gain: '<S12>/altitude_compFilter_accel_Gain0'
   *  Gain: '<S12>/altitude_compFilter_baro_Gain0'
   *  Product: '<S12>/altitude_matrix_multiply'
   *  Sum: '<S12>/altitude_velCorrection'
   *  Sum: '<S15>/Diff'
   *  UnitDelay: '<S15>/UD'
   *
   * Block description for '<S15>/Diff':
   *
   *  Add in CPU
   *
   * Block description for '<S15>/UD':
   *
   *  Store in Global RAM
   */
  IIR_IMUgyro_r_tmp = (0.01F * rtb_VectorConcatenate_0[2] +
                       rtDW->altitude_delay0_DSTATE) * 0.999F +
    (rtb_preprocessing_alt_ned - rtDW->UD_DSTATE) * 0.001F;

  /* Sum: '<S12>/altitude_baroCorrection' incorporates:
   *  Delay: '<S12>/altitude_delay'
   *  Gain: '<S12>/altitude_Ts_Gain'
   *  Gain: '<S12>/altitude_compFilter_accel_Gain'
   *  Gain: '<S12>/altitude_compFilter_baro_Gain'
   *  Sum: '<S12>/altitude_accelCorrection'
   */
  rtb_preprocessing_alt_out = (0.01F * IIR_IMUgyro_r_tmp +
    rtDW->altitude_delay_DSTATE) * 0.99F + 0.01F * rtb_preprocessing_alt_out;

  /* Update for Delay: '<S12>/altitude_delay0' */
  rtDW->altitude_delay0_DSTATE = IIR_IMUgyro_r_tmp;

  /* Update for UnitDelay: '<S15>/UD'
   *
   * Block description for '<S15>/UD':
   *
   *  Store in Global RAM
   */
  rtDW->UD_DSTATE = rtb_preprocessing_alt_ned;

  /* Update for Delay: '<S12>/altitude_delay' */
  rtDW->altitude_delay_DSTATE = rtb_preprocessing_alt_out;

  /* End of Outputs for SubSystem: '<S2>/AltitudeEstimator' */
  /* End of Outputs for SubSystem: '<Root>/StatesEstimator' */

  /* Outputs for Atomic SubSystem: '<Root>/FCS' */
  /* Outputs for Atomic SubSystem: '<S1>/CommandHandling' */
  /* Saturate: '<S3>/Saturation3' incorporates:
   *  Inport: '<Root>/Commands'
   */
  if (rtU_Commands->control_input.alt_cmd > 10.0F) {
    rtb_preprocessing_accel_out_i_0 = 10.0F;
  } else if (rtU_Commands->control_input.alt_cmd < -10.0F) {
    rtb_preprocessing_accel_out_i_0 = -10.0F;
  } else {
    rtb_preprocessing_accel_out_i_0 = rtU_Commands->control_input.alt_cmd;
  }

  /* End of Saturate: '<S3>/Saturation3' */
  /* End of Outputs for SubSystem: '<S1>/CommandHandling' */

  /* Outputs for Atomic SubSystem: '<S1>/Controller' */
  /* Outputs for Atomic SubSystem: '<S4>/AltitudeControl' */
  /* Sum: '<S6>/Sum2' incorporates:
   *  Constant: '<S6>/Constant'
   *  Gain: '<S6>/Alt_D_Gain'
   *  Gain: '<S6>/Alt_P_Gain'
   *  Gain: '<S6>/Gain1'
   *  Gain: '<S6>/Gain2'
   *  Sum: '<S6>/Sum'
   *  Sum: '<S6>/Sum1'
   */
  rtb_MotorBR = ((rtb_preprocessing_accel_out_i_0 - (-rtb_preprocessing_alt_out))
                 * PID_alt_P - PID_alt_D * -IIR_IMUgyro_r_tmp) + 1.2581325;

  /* End of Outputs for SubSystem: '<S4>/AltitudeControl' */
  /* End of Outputs for SubSystem: '<S1>/Controller' */

  /* Outputs for Atomic SubSystem: '<S1>/CommandHandling' */
  /* Saturate: '<S3>/Saturation1' incorporates:
   *  Inport: '<Root>/Commands'
   */
  /* Unit Conversion - from: deg to: rad
     Expression: output = (0.0174533*input) + (0) */
  /* Unit Conversion - from: deg to: rad
     Expression: output = (0.0174533*input) + (0) */
  if (rtU_Commands->control_input.pitch_cmd > 45.0F) {
    rtb_preprocessing_accel_out_i_0 = 45.0F;
  } else if (rtU_Commands->control_input.pitch_cmd < -45.0F) {
    rtb_preprocessing_accel_out_i_0 = -45.0F;
  } else {
    rtb_preprocessing_accel_out_i_0 = rtU_Commands->control_input.pitch_cmd;
  }

  /* End of Saturate: '<S3>/Saturation1' */
  /* End of Outputs for SubSystem: '<S1>/CommandHandling' */

  /* Outputs for Atomic SubSystem: '<S1>/Controller' */
  /* Outputs for Atomic SubSystem: '<S4>/PitchRollControl' */
  /* Sum: '<S7>/Sum19' incorporates:
   *  UnitConversion: '<S9>/Unit Conversion'
   */
  rtb_preprocessing_alt_ned = 0.0174532924F * rtb_preprocessing_accel_out_i_0 -
    Subtract1[1];

  /* End of Outputs for SubSystem: '<S4>/PitchRollControl' */
  /* End of Outputs for SubSystem: '<S1>/Controller' */

  /* Outputs for Atomic SubSystem: '<S1>/CommandHandling' */
  /* Saturate: '<S3>/Saturation2' incorporates:
   *  Inport: '<Root>/Commands'
   */
  if (rtU_Commands->control_input.roll_cmd > 45.0F) {
    rtb_preprocessing_accel_out_i_0 = 45.0F;
  } else if (rtU_Commands->control_input.roll_cmd < -45.0F) {
    rtb_preprocessing_accel_out_i_0 = -45.0F;
  } else {
    rtb_preprocessing_accel_out_i_0 = rtU_Commands->control_input.roll_cmd;
  }

  /* End of Saturate: '<S3>/Saturation2' */
  /* End of Outputs for SubSystem: '<S1>/CommandHandling' */

  /* Outputs for Atomic SubSystem: '<S1>/Controller' */
  /* Outputs for Atomic SubSystem: '<S4>/PitchRollControl' */
  /* Sum: '<S7>/Sum19' incorporates:
   *  UnitConversion: '<S10>/Unit Conversion'
   */
  rtb_preprocessing_accel_out_i_0 = 0.0174532924F *
    rtb_preprocessing_accel_out_i_0 - Subtract1[2];

  /* Outputs for Atomic SubSystem: '<Root>/StatesEstimator' */
  /* Outputs for Atomic SubSystem: '<S2>/OrientationEstimator' */
  /* Sum: '<S7>/Sum16' incorporates:
   *  Concatenate: '<S14>/preprocessing_pqr_concat'
   *  DiscreteIntegrator: '<S7>/Discrete-Time Integrator'
   *  Gain: '<S7>/D_pr'
   *  Gain: '<S7>/I_pr'
   *  Gain: '<S7>/P_pr'
   *  Inport: '<S13>/<gyro_data_SI>'
   */
  rtb_preprocessing_accel_out_idx = (PID_pitch_P * rtb_preprocessing_alt_ned +
    PID_pitch_roll_I * rtDW->DiscreteTimeIntegrator_DSTATE[0]) - PID_pitch_D *
    rtb_preprocessing_gyro_out_id_0;
  Subtract2_idx_0 = (PID_pitch_P * rtb_preprocessing_accel_out_i_0 +
                     PID_pitch_roll_I * rtDW->DiscreteTimeIntegrator_DSTATE[1])
    - PID_pitch_D * rtb_preprocessing_gyro_out_id_1;

  /* End of Outputs for SubSystem: '<S2>/OrientationEstimator' */
  /* End of Outputs for SubSystem: '<Root>/StatesEstimator' */

  /* Sum: '<S7>/Add' incorporates:
   *  Delay: '<S7>/Delay'
   *  Gain: '<S7>/antiWU_Gain'
   */
  rtb_preprocessing_alt_ned -= 0.001F * rtDW->Delay_DSTATE[0];

  /* Update for Delay: '<S7>/Delay' incorporates:
   *  DiscreteIntegrator: '<S7>/Discrete-Time Integrator'
   */
  rtDW->Delay_DSTATE[0] = rtDW->DiscreteTimeIntegrator_DSTATE[0];

  /* Update for DiscreteIntegrator: '<S7>/Discrete-Time Integrator' */
  rtDW->DiscreteTimeIntegrator_DSTATE[0] += 0.01F * rtb_preprocessing_alt_ned;
  if (rtDW->DiscreteTimeIntegrator_DSTATE[0] >= 2.0F) {
    rtDW->DiscreteTimeIntegrator_DSTATE[0] = 2.0F;
  } else {
    if (rtDW->DiscreteTimeIntegrator_DSTATE[0] <= -2.0F) {
      rtDW->DiscreteTimeIntegrator_DSTATE[0] = -2.0F;
    }
  }

  /* Sum: '<S7>/Add' incorporates:
   *  Delay: '<S7>/Delay'
   *  Gain: '<S7>/antiWU_Gain'
   */
  rtb_preprocessing_alt_ned = rtb_preprocessing_accel_out_i_0 - 0.001F *
    rtDW->Delay_DSTATE[1];

  /* Update for Delay: '<S7>/Delay' incorporates:
   *  DiscreteIntegrator: '<S7>/Discrete-Time Integrator'
   */
  rtDW->Delay_DSTATE[1] = rtDW->DiscreteTimeIntegrator_DSTATE[1];

  /* Update for DiscreteIntegrator: '<S7>/Discrete-Time Integrator' */
  rtDW->DiscreteTimeIntegrator_DSTATE[1] += 0.01F * rtb_preprocessing_alt_ned;
  if (rtDW->DiscreteTimeIntegrator_DSTATE[1] >= 2.0F) {
    rtDW->DiscreteTimeIntegrator_DSTATE[1] = 2.0F;
  } else {
    if (rtDW->DiscreteTimeIntegrator_DSTATE[1] <= -2.0F) {
      rtDW->DiscreteTimeIntegrator_DSTATE[1] = -2.0F;
    }
  }

  /* End of Outputs for SubSystem: '<S4>/PitchRollControl' */
  /* End of Outputs for SubSystem: '<S1>/Controller' */

  /* Outputs for Atomic SubSystem: '<S1>/CommandHandling' */
  /* Saturate: '<S3>/Saturation' incorporates:
   *  Inport: '<Root>/Commands'
   */
  /* Unit Conversion - from: deg to: rad
     Expression: output = (0.0174533*input) + (0) */
  if (rtU_Commands->control_input.yaw_cmd > 180.0F) {
    rtb_preprocessing_accel_out_i_0 = 180.0F;
  } else if (rtU_Commands->control_input.yaw_cmd < -180.0F) {
    rtb_preprocessing_accel_out_i_0 = -180.0F;
  } else {
    rtb_preprocessing_accel_out_i_0 = rtU_Commands->control_input.yaw_cmd;
  }

  /* End of Saturate: '<S3>/Saturation' */
  /* End of Outputs for SubSystem: '<S1>/CommandHandling' */

  /* Outputs for Atomic SubSystem: '<S1>/Controller' */
  /* Outputs for Atomic SubSystem: '<S4>/YawControl' */
  /* Outputs for Atomic SubSystem: '<Root>/StatesEstimator' */
  /* Outputs for Atomic SubSystem: '<S2>/OrientationEstimator' */
  /* Gain: '<S8>/Gain' incorporates:
   *  Concatenate: '<S14>/preprocessing_pqr_concat'
   *  Gain: '<S8>/Yaw_D_Gain'
   *  Gain: '<S8>/Yaw_P_Gain'
   *  Inport: '<S13>/<gyro_data_SI>'
   *  Sum: '<S8>/Sum'
   *  Sum: '<S8>/Sum1'
   *  UnitConversion: '<S11>/Unit Conversion'
   */
  rtb_preprocessing_alt_ned = -((0.0174532924F * rtb_preprocessing_accel_out_i_0
    - Subtract1[0]) * PID_yaw_P - PID_yaw_D * rtb_preprocessing_gyro_out_idx_);

  /* End of Outputs for SubSystem: '<S2>/OrientationEstimator' */
  /* End of Outputs for SubSystem: '<Root>/StatesEstimator' */
  /* End of Outputs for SubSystem: '<S4>/YawControl' */
  /* End of Outputs for SubSystem: '<S1>/Controller' */

  /* Outputs for Atomic SubSystem: '<S1>/MotorMixing' */
  /* Sum: '<S5>/MotorFR' incorporates:
   *  Sum: '<S5>/MotorBL'
   */
  rtb_fcn2 = rtb_MotorBR + rtb_preprocessing_alt_ned;

  /* Polyval: '<S5>/Polynomial' incorporates:
   *  Sum: '<S5>/MotorFR'
   */
  rtb_MotorBL = ((rtb_fcn2 + rtb_preprocessing_accel_out_idx) - Subtract2_idx_0)
    * 86.036 + 113.82;

  /* Saturate: '<S5>/Saturation' */
  if (rtb_MotorBL > 1000.0) {
    rtb_MotorBL = 1000.0;
  } else {
    if (rtb_MotorBL < 100.0) {
      rtb_MotorBL = 100.0;
    }
  }

  /* Outport: '<Root>/Throttle' incorporates:
   *  DataTypeConversion: '<S5>/Data Type Conversion'
   */
  rtY_Throttle[0] = (uint16_T)rtb_MotorBL;

  /* Sum: '<S5>/MotorFL' incorporates:
   *  Sum: '<S5>/MotorBR'
   */
  rtb_MotorBR -= rtb_preprocessing_alt_ned;

  /* Polyval: '<S5>/Polynomial' incorporates:
   *  Sum: '<S5>/MotorFL'
   */
  rtb_MotorBL = ((rtb_MotorBR + rtb_preprocessing_accel_out_idx) +
                 Subtract2_idx_0) * 86.036 + 113.82;

  /* Saturate: '<S5>/Saturation' */
  if (rtb_MotorBL > 1000.0) {
    rtb_MotorBL = 1000.0;
  } else {
    if (rtb_MotorBL < 100.0) {
      rtb_MotorBL = 100.0;
    }
  }

  /* Outport: '<Root>/Throttle' incorporates:
   *  DataTypeConversion: '<S5>/Data Type Conversion'
   */
  rtY_Throttle[1] = (uint16_T)rtb_MotorBL;

  /* Polyval: '<S5>/Polynomial' incorporates:
   *  Sum: '<S5>/MotorBR'
   */
  rtb_MotorBL = ((rtb_MotorBR - rtb_preprocessing_accel_out_idx) -
                 Subtract2_idx_0) * 86.036 + 113.82;

  /* Saturate: '<S5>/Saturation' */
  if (rtb_MotorBL > 1000.0) {
    rtb_MotorBL = 1000.0;
  } else {
    if (rtb_MotorBL < 100.0) {
      rtb_MotorBL = 100.0;
    }
  }

  /* Outport: '<Root>/Throttle' incorporates:
   *  DataTypeConversion: '<S5>/Data Type Conversion'
   */
  rtY_Throttle[2] = (uint16_T)rtb_MotorBL;

  /* Polyval: '<S5>/Polynomial' incorporates:
   *  Sum: '<S5>/MotorBL'
   */
  rtb_MotorBL = ((rtb_fcn2 - rtb_preprocessing_accel_out_idx) + Subtract2_idx_0)
    * 86.036 + 113.82;

  /* Saturate: '<S5>/Saturation' */
  if (rtb_MotorBL > 1000.0) {
    rtb_MotorBL = 1000.0;
  } else {
    if (rtb_MotorBL < 100.0) {
      rtb_MotorBL = 100.0;
    }
  }

  /* Outport: '<Root>/Throttle' incorporates:
   *  DataTypeConversion: '<S5>/Data Type Conversion'
   */
  rtY_Throttle[3] = (uint16_T)rtb_MotorBL;

  /* End of Outputs for SubSystem: '<S1>/MotorMixing' */
  /* End of Outputs for SubSystem: '<Root>/FCS' */

  /* Outport: '<Root>/State_Estim' incorporates:
   *  BusCreator generated from: '<Root>/State_Estim'
   *  Concatenate: '<S14>/preprocessing_pqr_concat'
   *  Inport: '<S13>/<gyro_data_SI>'
   */
  rtY_State_Estim->x = 0.0F;
  rtY_State_Estim->y = 0.0F;
  rtY_State_Estim->z = rtb_preprocessing_alt_out;
  rtY_State_Estim->dx = 0.0F;
  rtY_State_Estim->dy = 0.0F;
  rtY_State_Estim->dz = IIR_IMUgyro_r_tmp;
  rtY_State_Estim->yaw = Subtract1[0];
  rtY_State_Estim->pitch = Subtract1[1];
  rtY_State_Estim->roll = Subtract1[2];

  /* Outputs for Atomic SubSystem: '<Root>/StatesEstimator' */
  /* Outputs for Atomic SubSystem: '<S2>/OrientationEstimator' */
  rtY_State_Estim->p = rtb_preprocessing_gyro_out_id_1;
  rtY_State_Estim->q = rtb_preprocessing_gyro_out_id_0;
  rtY_State_Estim->r = rtb_preprocessing_gyro_out_idx_;

  /* End of Outputs for SubSystem: '<S2>/OrientationEstimator' */
  /* End of Outputs for SubSystem: '<Root>/StatesEstimator' */
}

/* Model initialize function */
void flightController_initialize(RT_MODEL *const rtM, FCS_command_t
  *rtU_Commands, sensor_bias_t *rtU_Bias, sensor_data_t *rtU_Sensors,
  states_estimate_t *rtY_State_Estim, uint16_T rtY_Throttle[4])
{
  DW *rtDW = rtM->dwork;

  /* Registration code */

  /* states (dwork) */
  (void) memset((void *)rtDW, 0,
                sizeof(DW));

  /* external inputs */
  *rtU_Commands = flightController_rtZFCS_command;
  *rtU_Bias = flightController_rtZsensor_bias;
  *rtU_Sensors = flightController_rtZsensor_data;

  /* external outputs */
  (*rtY_State_Estim) = flightController_rtZstates_estimate_t;
  (void) memset(&rtY_Throttle[0], 0,
                4U*sizeof(uint16_T));

  {
    int32_T i;

    /* SystemInitialize for Atomic SubSystem: '<Root>/StatesEstimator' */
    /* SystemInitialize for Atomic SubSystem: '<S2>/SensorPreprocessing' */
    /* InitializeConditions for DiscreteFir: '<S14>/FIR_IMUaccel' */
    rtDW->FIR_IMUaccel_circBuf = 0;
    for (i = 0; i < 15; i++) {
      rtDW->FIR_IMUaccel_states[i] = 0.0F;
    }

    /* End of InitializeConditions for DiscreteFir: '<S14>/FIR_IMUaccel' */

    /* InitializeConditions for DiscreteFir: '<S14>/FIR_IMUgyro' */
    rtDW->FIR_IMUgyro_circBuf = 0;
    for (i = 0; i < 10; i++) {
      rtDW->FIR_IMUgyro_states[i] = 0.0F;
    }

    /* End of InitializeConditions for DiscreteFir: '<S14>/FIR_IMUgyro' */
    /* End of SystemInitialize for SubSystem: '<S2>/SensorPreprocessing' */
    /* End of SystemInitialize for SubSystem: '<Root>/StatesEstimator' */
  }
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
