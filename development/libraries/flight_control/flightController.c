/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: flightController.c
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
real32_T PID_pitch_D = 0.599954486F;   /* Variable: PID_pitch_D
                                        * Referenced by:
                                        *   '<S87>/Derivative Gain'
                                        *   '<S135>/Derivative Gain'
                                        */
real32_T PID_pitch_I = 0.00100771897F; /* Variable: PID_pitch_I
                                        * Referenced by:
                                        *   '<S90>/Integral Gain'
                                        *   '<S138>/Integral Gain'
                                        */
real32_T PID_pitch_N = 194.459274F;    /* Variable: PID_pitch_N
                                        * Referenced by:
                                        *   '<S96>/Filter Coefficient'
                                        *   '<S144>/Filter Coefficient'
                                        */
real32_T PID_pitch_P = 0.0521662198F;  /* Variable: PID_pitch_P
                                        * Referenced by:
                                        *   '<S98>/Proportional Gain'
                                        *   '<S146>/Proportional Gain'
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
  { 0.7071F, 0.0F, 0.0F, -0.7071F},          /* quat_bias */
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
  real32_T pressureFilter_IIR_tmp;
  real32_T rtb_preprocessing_accel_out_i_0;
  real32_T rtb_preprocessing_accel_out_i_1;
  real32_T rtb_preprocessing_accel_out_idx;
  real32_T rtb_preprocessing_alt_ned;
  real32_T rtb_preprocessing_alt_out;
  real32_T rtb_preprocessing_gyro_out_idx_;

  /* Outputs for Atomic SubSystem: '<Root>/StatesEstimator' */
  /* Outputs for Atomic SubSystem: '<S2>/SensorPreprocessing' */
  /* Sqrt: '<S252>/sqrt' incorporates:
   *  DataTypeConversion: '<S210>/preprocessing_quat2doubleSens'
   *  Inport: '<Root>/Sensors'
   *  Product: '<S253>/Product'
   *  Product: '<S253>/Product1'
   *  Product: '<S253>/Product2'
   *  Product: '<S253>/Product3'
   *  Sum: '<S253>/Sum'
   */
  rtb_fcn2 = sqrt((((real_T)rtU_Sensors->quat_data[0] * rtU_Sensors->quat_data[0]
                    + (real_T)rtU_Sensors->quat_data[1] * rtU_Sensors->
                    quat_data[1]) + (real_T)rtU_Sensors->quat_data[2] *
                   rtU_Sensors->quat_data[2]) + (real_T)rtU_Sensors->quat_data[3]
                  * rtU_Sensors->quat_data[3]);

  /* Product: '<S247>/Product' incorporates:
   *  DataTypeConversion: '<S210>/preprocessing_quat2doubleSens'
   *  Inport: '<Root>/Sensors'
   */
  rtb_MotorBR = rtU_Sensors->quat_data[0] / rtb_fcn2;

  /* Product: '<S247>/Product1' incorporates:
   *  DataTypeConversion: '<S210>/preprocessing_quat2doubleSens'
   *  Inport: '<Root>/Sensors'
   */
  rtb_MotorBL = rtU_Sensors->quat_data[1] / rtb_fcn2;

  /* Product: '<S247>/Product2' incorporates:
   *  DataTypeConversion: '<S210>/preprocessing_quat2doubleSens'
   *  Inport: '<Root>/Sensors'
   */
  rtb_fcn4 = rtU_Sensors->quat_data[2] / rtb_fcn2;

  /* Product: '<S247>/Product3' incorporates:
   *  DataTypeConversion: '<S210>/preprocessing_quat2doubleSens'
   *  Inport: '<Root>/Sensors'
   */
  rtb_fcn2 = rtU_Sensors->quat_data[3] / rtb_fcn2;

  /* Fcn: '<S237>/fcn2' incorporates:
   *  Fcn: '<S237>/fcn5'
   */
  rtb_Saturation_idx_1 = rtb_MotorBR * rtb_MotorBR;
  rtb_sincos_o2_idx_2 = rtb_MotorBL * rtb_MotorBL;
  rtb_sincos_o2_idx_0_tmp = rtb_fcn4 * rtb_fcn4;
  rtb_sincos_o2_idx_0_tmp_0 = rtb_fcn2 * rtb_fcn2;

  /* Trigonometry: '<S246>/Trigonometric Function1' incorporates:
   *  Fcn: '<S237>/fcn1'
   *  Fcn: '<S237>/fcn2'
   */
  rtb_sincos_o2_idx_0 = atan2((rtb_MotorBL * rtb_fcn4 + rtb_MotorBR * rtb_fcn2) *
    2.0, ((rtb_Saturation_idx_1 + rtb_sincos_o2_idx_2) - rtb_sincos_o2_idx_0_tmp)
    - rtb_sincos_o2_idx_0_tmp_0);

  /* Fcn: '<S237>/fcn3' */
  rtb_fcn3 = (rtb_MotorBL * rtb_fcn2 - rtb_MotorBR * rtb_fcn4) * -2.0;

  /* If: '<S248>/If' incorporates:
   *  Constant: '<S249>/Constant'
   *  Constant: '<S250>/Constant'
   *  Inport: '<S251>/In'
   */
  if (rtb_fcn3 > 1.0) {
    /* Outputs for IfAction SubSystem: '<S248>/If Action Subsystem' incorporates:
     *  ActionPort: '<S249>/Action Port'
     */
    rtb_Saturation_idx_0 = 1.0;

    /* End of Outputs for SubSystem: '<S248>/If Action Subsystem' */
  } else if (rtb_fcn3 < -1.0) {
    /* Outputs for IfAction SubSystem: '<S248>/If Action Subsystem1' incorporates:
     *  ActionPort: '<S250>/Action Port'
     */
    rtb_Saturation_idx_0 = 1.0;

    /* End of Outputs for SubSystem: '<S248>/If Action Subsystem1' */
  } else {
    /* Outputs for IfAction SubSystem: '<S248>/If Action Subsystem2' incorporates:
     *  ActionPort: '<S251>/Action Port'
     */
    rtb_Saturation_idx_0 = rtb_fcn3;

    /* End of Outputs for SubSystem: '<S248>/If Action Subsystem2' */
  }

  /* End of If: '<S248>/If' */

  /* Trigonometry: '<S246>/Trigonometric Function3' incorporates:
   *  Fcn: '<S237>/fcn4'
   *  Fcn: '<S237>/fcn5'
   */
  rtb_sincos_o2_idx_2 = atan2((rtb_fcn4 * rtb_fcn2 + rtb_MotorBR * rtb_MotorBL) *
    2.0, ((rtb_Saturation_idx_1 - rtb_sincos_o2_idx_2) - rtb_sincos_o2_idx_0_tmp)
    + rtb_sincos_o2_idx_0_tmp_0);

  /* Sqrt: '<S244>/sqrt' incorporates:
   *  DataTypeConversion: '<S210>/preprocessing_quat2doubleBias'
   *  Inport: '<Root>/Bias'
   *  Product: '<S245>/Product'
   *  Product: '<S245>/Product1'
   *  Product: '<S245>/Product2'
   *  Product: '<S245>/Product3'
   *  Sum: '<S245>/Sum'
   */
  rtb_fcn3 = sqrt((((real_T)rtU_Bias->quat_bias[0] * rtU_Bias->quat_bias[0] +
                    (real_T)rtU_Bias->quat_bias[1] * rtU_Bias->quat_bias[1]) +
                   (real_T)rtU_Bias->quat_bias[2] * rtU_Bias->quat_bias[2]) +
                  (real_T)rtU_Bias->quat_bias[3] * rtU_Bias->quat_bias[3]);

  /* Product: '<S239>/Product' incorporates:
   *  DataTypeConversion: '<S210>/preprocessing_quat2doubleBias'
   *  Inport: '<Root>/Bias'
   */
  rtb_fcn2 = rtU_Bias->quat_bias[0] / rtb_fcn3;

  /* Product: '<S239>/Product1' incorporates:
   *  DataTypeConversion: '<S210>/preprocessing_quat2doubleBias'
   *  Inport: '<Root>/Bias'
   */
  rtb_MotorBR = rtU_Bias->quat_bias[1] / rtb_fcn3;

  /* Product: '<S239>/Product2' incorporates:
   *  DataTypeConversion: '<S210>/preprocessing_quat2doubleBias'
   *  Inport: '<Root>/Bias'
   */
  rtb_MotorBL = rtU_Bias->quat_bias[2] / rtb_fcn3;

  /* Product: '<S239>/Product3' incorporates:
   *  DataTypeConversion: '<S210>/preprocessing_quat2doubleBias'
   *  Inport: '<Root>/Bias'
   */
  rtb_fcn3 = rtU_Bias->quat_bias[3] / rtb_fcn3;

  /* Fcn: '<S236>/fcn2' incorporates:
   *  Fcn: '<S236>/fcn5'
   */
  rtb_sincos_o2_idx_0_tmp = rtb_fcn2 * rtb_fcn2;
  rtb_sincos_o2_idx_0_tmp_0 = rtb_MotorBR * rtb_MotorBR;
  u1_tmp = rtb_MotorBL * rtb_MotorBL;
  u1_tmp_0 = rtb_fcn3 * rtb_fcn3;

  /* Fcn: '<S236>/fcn3' */
  rtb_fcn4 = (rtb_MotorBR * rtb_fcn3 - rtb_fcn2 * rtb_MotorBL) * -2.0;

  /* If: '<S240>/If' incorporates:
   *  Constant: '<S241>/Constant'
   *  Constant: '<S242>/Constant'
   */
  if (rtb_fcn4 > 1.0) {
    /* Outputs for IfAction SubSystem: '<S240>/If Action Subsystem' incorporates:
     *  ActionPort: '<S241>/Action Port'
     */
    rtb_fcn4 = 1.0;

    /* End of Outputs for SubSystem: '<S240>/If Action Subsystem' */
  } else {
    if (rtb_fcn4 < -1.0) {
      /* Outputs for IfAction SubSystem: '<S240>/If Action Subsystem1' incorporates:
       *  ActionPort: '<S242>/Action Port'
       */
      rtb_fcn4 = 1.0;

      /* End of Outputs for SubSystem: '<S240>/If Action Subsystem1' */
    }
  }

  /* End of If: '<S240>/If' */

  /* Gain: '<S235>/1//2' incorporates:
   *  Fcn: '<S236>/fcn1'
   *  Fcn: '<S236>/fcn2'
   *  Sum: '<S210>/Subtract'
   *  Trigonometry: '<S238>/Trigonometric Function1'
   */
  rtb_Saturation_idx_1 = (rtb_sincos_o2_idx_0 - atan2((rtb_MotorBR * rtb_MotorBL
    + rtb_fcn2 * rtb_fcn3) * 2.0, ((rtb_sincos_o2_idx_0_tmp +
    rtb_sincos_o2_idx_0_tmp_0) - u1_tmp) - u1_tmp_0)) * 0.5;

  /* Sum: '<S210>/Subtract1' incorporates:
   *  Inport: '<Root>/Bias'
   *  Inport: '<Root>/Sensors'
   */
  Subtract1[0] = rtU_Sensors->accel_data_SI[0] - rtU_Bias->accel_bias[0];

  /* Trigonometry: '<S235>/sincos' */
  rtb_sincos_o2_idx_0 = cos(rtb_Saturation_idx_1);
  rtb_VectorConcatenate_p[0] = sin(rtb_Saturation_idx_1);

  /* Gain: '<S235>/1//2' incorporates:
   *  Sum: '<S210>/Subtract'
   *  Trigonometry: '<S238>/trigFcn'
   *  Trigonometry: '<S246>/trigFcn'
   */
  rtb_Saturation_idx_1 = (asin(rtb_Saturation_idx_0) - asin(rtb_fcn4)) * 0.5;

  /* Sum: '<S210>/Subtract1' incorporates:
   *  Inport: '<Root>/Bias'
   *  Inport: '<Root>/Sensors'
   */
  Subtract1[1] = rtU_Sensors->accel_data_SI[1] - rtU_Bias->accel_bias[1];

  /* Trigonometry: '<S235>/sincos' */
  rtb_fcn4 = cos(rtb_Saturation_idx_1);
  rtb_VectorConcatenate_p[1] = sin(rtb_Saturation_idx_1);

  /* Gain: '<S235>/1//2' incorporates:
   *  Fcn: '<S236>/fcn4'
   *  Fcn: '<S236>/fcn5'
   *  Sum: '<S210>/Subtract'
   *  Trigonometry: '<S238>/Trigonometric Function3'
   */
  rtb_Saturation_idx_1 = (rtb_sincos_o2_idx_2 - atan2((rtb_MotorBL * rtb_fcn3 +
    rtb_fcn2 * rtb_MotorBR) * 2.0, ((rtb_sincos_o2_idx_0_tmp -
    rtb_sincos_o2_idx_0_tmp_0) - u1_tmp) + u1_tmp_0)) * 0.5;

  /* Trigonometry: '<S235>/sincos' */
  rtb_fcn3 = cos(rtb_Saturation_idx_1);

  /* Sum: '<S210>/Subtract1' incorporates:
   *  Inport: '<Root>/Bias'
   *  Inport: '<Root>/Sensors'
   */
  Subtract1[2] = rtU_Sensors->accel_data_SI[2] - rtU_Bias->accel_bias[2];

  /* Trigonometry: '<S235>/sincos' */
  rtb_VectorConcatenate_p[2] = sin(rtb_Saturation_idx_1);

  /* DiscreteFir: '<S210>/FIR_IMUaccel' incorporates:
   *  Sum: '<S210>/Subtract1'
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

  /* Sum: '<S210>/Subtract2' incorporates:
   *  Inport: '<Root>/Bias'
   *  Inport: '<Root>/Sensors'
   */
  Subtract2_idx_0 = rtU_Sensors->gyro_data_SI[0] - rtU_Bias->gyro_bias[0];

  /* DiscreteFir: '<S210>/FIR_IMUaccel' incorporates:
   *  Sum: '<S210>/Subtract1'
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

  /* Sum: '<S210>/Subtract2' incorporates:
   *  Inport: '<Root>/Bias'
   *  Inport: '<Root>/Sensors'
   */
  Subtract2_idx_1 = rtU_Sensors->gyro_data_SI[1] - rtU_Bias->gyro_bias[1];

  /* DiscreteFir: '<S210>/FIR_IMUaccel' incorporates:
   *  Sum: '<S210>/Subtract1'
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

  /* DiscreteFir: '<S210>/FIR_IMUgyro' */
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

  /* Outport: '<Root>/State_Estim' incorporates:
   *  DiscreteFir: '<S210>/FIR_IMUgyro'
   */
  rtY_State_Estim->p = IIR_IMUgyro_r_tmp;

  /* DiscreteFir: '<S210>/FIR_IMUgyro' */
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

  /* Outport: '<Root>/State_Estim' incorporates:
   *  DiscreteFir: '<S210>/FIR_IMUgyro'
   */
  rtY_State_Estim->q = IIR_IMUgyro_r_tmp;

  /* DiscreteFilter: '<S210>/IIR_IMUgyro_r' incorporates:
   *  Inport: '<Root>/Bias'
   *  Inport: '<Root>/Sensors'
   *  Sum: '<S210>/Subtract2'
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

  /* End of DiscreteFilter: '<S210>/IIR_IMUgyro_r' */

  /* DiscreteFilter: '<S210>/pressureFilter_IIR' incorporates:
   *  Gain: '<S210>/Alt2NED'
   *  Inport: '<Root>/Bias'
   *  Inport: '<Root>/Sensors'
   *  Sum: '<S210>/Subtract3'
   */
  pressureFilter_IIR_tmp = ((-(rtU_Sensors->alt_data_SI - rtU_Bias->alt_bias) -
    -2.93717074F * rtDW->pressureFilter_IIR_states[0]) - 2.87629962F *
    rtDW->pressureFilter_IIR_states[1]) - -0.939098954F *
    rtDW->pressureFilter_IIR_states[2];
  rtb_preprocessing_alt_out = ((3.75683794E-6F * pressureFilter_IIR_tmp +
    1.12705138E-5F * rtDW->pressureFilter_IIR_states[0]) + 1.12705138E-5F *
    rtDW->pressureFilter_IIR_states[1]) + 3.75683794E-6F *
    rtDW->pressureFilter_IIR_states[2];

  /* Update for DiscreteFir: '<S210>/FIR_IMUaccel' incorporates:
   *  Sum: '<S210>/Subtract1'
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

  /* End of Update for DiscreteFir: '<S210>/FIR_IMUaccel' */

  /* Update for DiscreteFir: '<S210>/FIR_IMUgyro' */
  /* Update circular buffer index */
  rtDW->FIR_IMUgyro_circBuf--;
  if (rtDW->FIR_IMUgyro_circBuf < 0) {
    rtDW->FIR_IMUgyro_circBuf = 4;
  }

  /* Update circular buffer */
  rtDW->FIR_IMUgyro_states[rtDW->FIR_IMUgyro_circBuf] = Subtract2_idx_0;
  rtDW->FIR_IMUgyro_states[rtDW->FIR_IMUgyro_circBuf + 5] = Subtract2_idx_1;

  /* End of Update for DiscreteFir: '<S210>/FIR_IMUgyro' */

  /* Update for DiscreteFilter: '<S210>/IIR_IMUgyro_r' */
  rtDW->IIR_IMUgyro_r_states[4] = rtDW->IIR_IMUgyro_r_states[3];
  rtDW->IIR_IMUgyro_r_states[3] = rtDW->IIR_IMUgyro_r_states[2];
  rtDW->IIR_IMUgyro_r_states[2] = rtDW->IIR_IMUgyro_r_states[1];
  rtDW->IIR_IMUgyro_r_states[1] = rtDW->IIR_IMUgyro_r_states[0];
  rtDW->IIR_IMUgyro_r_states[0] = IIR_IMUgyro_r_tmp;

  /* Update for DiscreteFilter: '<S210>/pressureFilter_IIR' */
  rtDW->pressureFilter_IIR_states[2] = rtDW->pressureFilter_IIR_states[1];
  rtDW->pressureFilter_IIR_states[1] = rtDW->pressureFilter_IIR_states[0];
  rtDW->pressureFilter_IIR_states[0] = pressureFilter_IIR_tmp;

  /* Fcn: '<S235>/q0' incorporates:
   *  Fcn: '<S235>/q1'
   */
  rtb_MotorBR = rtb_sincos_o2_idx_0 * rtb_fcn4;
  rtb_fcn2 = rtb_VectorConcatenate_p[0] * rtb_VectorConcatenate_p[1];

  /* Outputs for Atomic SubSystem: '<S2>/AltitudeEstimator' */
  /* Outputs for Atomic SubSystem: '<S2>/OrientationEstimator' */
  /* DataTypeConversion: '<S209>/orientation_quat2double' incorporates:
   *  DataTypeConversion: '<S208>/altitude_quat2double'
   *  DataTypeConversion: '<S210>/preprocessing_quat2single'
   *  Fcn: '<S235>/q0'
   *  Fcn: '<S235>/q1'
   */
  rtb_Saturation_idx_0 = (real32_T)(rtb_MotorBR * rtb_fcn3 + rtb_fcn2 *
    rtb_VectorConcatenate_p[2]);
  rtb_Saturation_idx_1 = (real32_T)(rtb_MotorBR * rtb_VectorConcatenate_p[2] -
    rtb_fcn2 * rtb_fcn3);

  /* End of Outputs for SubSystem: '<S2>/OrientationEstimator' */
  /* End of Outputs for SubSystem: '<S2>/AltitudeEstimator' */

  /* Fcn: '<S235>/q2' incorporates:
   *  Fcn: '<S235>/q0'
   *  Fcn: '<S235>/q3'
   */
  rtb_MotorBR = rtb_VectorConcatenate_p[0] * rtb_fcn4;
  rtb_fcn2 = rtb_sincos_o2_idx_0 * rtb_VectorConcatenate_p[1];

  /* Outputs for Atomic SubSystem: '<S2>/AltitudeEstimator' */
  /* Outputs for Atomic SubSystem: '<S2>/OrientationEstimator' */
  /* DataTypeConversion: '<S209>/orientation_quat2double' incorporates:
   *  DataTypeConversion: '<S208>/altitude_quat2double'
   *  DataTypeConversion: '<S210>/preprocessing_quat2single'
   *  Fcn: '<S235>/q0'
   *  Fcn: '<S235>/q2'
   *  Fcn: '<S235>/q3'
   */
  rtb_sincos_o2_idx_0 = (real32_T)(rtb_fcn2 * rtb_fcn3 + rtb_MotorBR *
    rtb_VectorConcatenate_p[2]);
  rtb_sincos_o2_idx_2 = (real32_T)(rtb_MotorBR * rtb_fcn3 - rtb_fcn2 *
    rtb_VectorConcatenate_p[2]);

  /* End of Outputs for SubSystem: '<S2>/SensorPreprocessing' */

  /* Sqrt: '<S233>/sqrt' incorporates:
   *  DataTypeConversion: '<S209>/orientation_quat2double'
   *  Product: '<S234>/Product'
   *  Product: '<S234>/Product1'
   *  Product: '<S234>/Product2'
   *  Product: '<S234>/Product3'
   *  Sqrt: '<S224>/sqrt'
   *  Sum: '<S234>/Sum'
   */
  rtb_sincos_o2_idx_0_tmp = sqrt(((rtb_Saturation_idx_0 * rtb_Saturation_idx_0 +
    rtb_Saturation_idx_1 * rtb_Saturation_idx_1) + rtb_sincos_o2_idx_0 *
    rtb_sincos_o2_idx_0) + rtb_sincos_o2_idx_2 * rtb_sincos_o2_idx_2);

  /* End of Outputs for SubSystem: '<S2>/AltitudeEstimator' */

  /* Product: '<S228>/Product' incorporates:
   *  DataTypeConversion: '<S209>/orientation_quat2double'
   *  Sqrt: '<S233>/sqrt'
   */
  rtb_fcn2 = rtb_Saturation_idx_0 / rtb_sincos_o2_idx_0_tmp;

  /* Product: '<S228>/Product1' incorporates:
   *  DataTypeConversion: '<S209>/orientation_quat2double'
   *  Sqrt: '<S233>/sqrt'
   */
  rtb_MotorBR = rtb_Saturation_idx_1 / rtb_sincos_o2_idx_0_tmp;

  /* Product: '<S228>/Product2' incorporates:
   *  DataTypeConversion: '<S209>/orientation_quat2double'
   *  Sqrt: '<S233>/sqrt'
   */
  rtb_MotorBL = rtb_sincos_o2_idx_0 / rtb_sincos_o2_idx_0_tmp;

  /* Product: '<S228>/Product3' incorporates:
   *  DataTypeConversion: '<S209>/orientation_quat2double'
   *  Sqrt: '<S233>/sqrt'
   */
  rtb_fcn3 = rtb_sincos_o2_idx_2 / rtb_sincos_o2_idx_0_tmp;

  /* Fcn: '<S226>/fcn2' incorporates:
   *  Fcn: '<S226>/fcn5'
   */
  rtb_sincos_o2_idx_0_tmp_0 = rtb_fcn2 * rtb_fcn2;
  u1_tmp = rtb_MotorBR * rtb_MotorBR;
  u1_tmp_0 = rtb_MotorBL * rtb_MotorBL;
  rtb_VectorConcatenate_h_tmp = rtb_fcn3 * rtb_fcn3;

  /* Trigonometry: '<S227>/Trigonometric Function1' incorporates:
   *  Fcn: '<S226>/fcn1'
   *  Fcn: '<S226>/fcn2'
   */
  rtb_VectorConcatenate_p[0] = atan2((rtb_MotorBR * rtb_MotorBL + rtb_fcn2 *
    rtb_fcn3) * 2.0, ((rtb_sincos_o2_idx_0_tmp_0 + u1_tmp) - u1_tmp_0) -
    rtb_VectorConcatenate_h_tmp);

  /* Fcn: '<S226>/fcn3' */
  rtb_fcn4 = (rtb_MotorBR * rtb_fcn3 - rtb_fcn2 * rtb_MotorBL) * -2.0;

  /* If: '<S229>/If' incorporates:
   *  Constant: '<S230>/Constant'
   *  Constant: '<S231>/Constant'
   */
  if (rtb_fcn4 > 1.0) {
    /* Outputs for IfAction SubSystem: '<S229>/If Action Subsystem' incorporates:
     *  ActionPort: '<S230>/Action Port'
     */
    rtb_fcn4 = 1.0;

    /* End of Outputs for SubSystem: '<S229>/If Action Subsystem' */
  } else {
    if (rtb_fcn4 < -1.0) {
      /* Outputs for IfAction SubSystem: '<S229>/If Action Subsystem1' incorporates:
       *  ActionPort: '<S231>/Action Port'
       */
      rtb_fcn4 = 1.0;

      /* End of Outputs for SubSystem: '<S229>/If Action Subsystem1' */
    }
  }

  /* End of If: '<S229>/If' */

  /* Trigonometry: '<S227>/trigFcn' */
  rtb_VectorConcatenate_p[1] = asin(rtb_fcn4);

  /* Trigonometry: '<S227>/Trigonometric Function3' incorporates:
   *  Fcn: '<S226>/fcn4'
   *  Fcn: '<S226>/fcn5'
   */
  rtb_VectorConcatenate_p[2] = atan2((rtb_MotorBL * rtb_fcn3 + rtb_fcn2 *
    rtb_MotorBR) * 2.0, ((rtb_sincos_o2_idx_0_tmp_0 - u1_tmp) - u1_tmp_0) +
    rtb_VectorConcatenate_h_tmp);

  /* End of Outputs for SubSystem: '<S2>/OrientationEstimator' */

  /* Outputs for Atomic SubSystem: '<S2>/AltitudeEstimator' */
  /* Product: '<S223>/Product3' incorporates:
   *  DataTypeConversion: '<S208>/altitude_quat2double'
   */
  rtb_fcn2 = rtb_sincos_o2_idx_2 / rtb_sincos_o2_idx_0_tmp;

  /* Product: '<S223>/Product2' incorporates:
   *  DataTypeConversion: '<S208>/altitude_quat2double'
   */
  rtb_MotorBR = rtb_sincos_o2_idx_0 / rtb_sincos_o2_idx_0_tmp;

  /* Product: '<S223>/Product1' incorporates:
   *  DataTypeConversion: '<S208>/altitude_quat2double'
   */
  rtb_MotorBL = rtb_Saturation_idx_1 / rtb_sincos_o2_idx_0_tmp;

  /* Product: '<S223>/Product' incorporates:
   *  DataTypeConversion: '<S208>/altitude_quat2double'
   */
  rtb_fcn3 = rtb_Saturation_idx_0 / rtb_sincos_o2_idx_0_tmp;

  /* Product: '<S213>/Product3' incorporates:
   *  Product: '<S217>/Product3'
   */
  rtb_fcn4 = rtb_fcn3 * rtb_fcn3;

  /* Product: '<S213>/Product2' incorporates:
   *  Product: '<S217>/Product2'
   */
  rtb_sincos_o2_idx_0 = rtb_MotorBL * rtb_MotorBL;

  /* Product: '<S213>/Product1' incorporates:
   *  Product: '<S217>/Product1'
   *  Product: '<S221>/Product1'
   */
  rtb_Saturation_idx_0 = rtb_MotorBR * rtb_MotorBR;

  /* Product: '<S213>/Product' incorporates:
   *  Product: '<S217>/Product'
   *  Product: '<S221>/Product'
   */
  rtb_Saturation_idx_1 = rtb_fcn2 * rtb_fcn2;

  /* Sum: '<S213>/Sum' incorporates:
   *  Product: '<S213>/Product'
   *  Product: '<S213>/Product1'
   *  Product: '<S213>/Product2'
   *  Product: '<S213>/Product3'
   */
  rtb_VectorConcatenate[0] = ((rtb_fcn4 + rtb_sincos_o2_idx_0) -
    rtb_Saturation_idx_0) - rtb_Saturation_idx_1;

  /* Product: '<S214>/Product2' incorporates:
   *  Product: '<S216>/Product2'
   */
  rtb_sincos_o2_idx_2 = rtb_MotorBL * rtb_MotorBR;

  /* Product: '<S214>/Product3' incorporates:
   *  Product: '<S216>/Product3'
   */
  rtb_sincos_o2_idx_0_tmp = rtb_fcn2 * rtb_fcn3;

  /* Gain: '<S214>/Gain' incorporates:
   *  Product: '<S214>/Product2'
   *  Product: '<S214>/Product3'
   *  Sum: '<S214>/Sum'
   */
  rtb_VectorConcatenate[3] = (rtb_sincos_o2_idx_0_tmp + rtb_sincos_o2_idx_2) *
    2.0;

  /* Product: '<S215>/Product1' incorporates:
   *  Product: '<S219>/Product1'
   */
  rtb_sincos_o2_idx_0_tmp_0 = rtb_fcn3 * rtb_MotorBR;

  /* Product: '<S215>/Product2' incorporates:
   *  Product: '<S219>/Product2'
   */
  u1_tmp = rtb_MotorBL * rtb_fcn2;

  /* Gain: '<S215>/Gain' incorporates:
   *  Product: '<S215>/Product1'
   *  Product: '<S215>/Product2'
   *  Sum: '<S215>/Sum'
   */
  rtb_VectorConcatenate[6] = (u1_tmp - rtb_sincos_o2_idx_0_tmp_0) * 2.0;

  /* Gain: '<S216>/Gain' incorporates:
   *  Sum: '<S216>/Sum'
   */
  rtb_VectorConcatenate[1] = (rtb_sincos_o2_idx_2 - rtb_sincos_o2_idx_0_tmp) *
    2.0;

  /* Sum: '<S217>/Sum' incorporates:
   *  Sum: '<S221>/Sum'
   */
  rtb_fcn4 -= rtb_sincos_o2_idx_0;
  rtb_VectorConcatenate[4] = (rtb_fcn4 + rtb_Saturation_idx_0) -
    rtb_Saturation_idx_1;

  /* Product: '<S218>/Product2' incorporates:
   *  Product: '<S220>/Product2'
   */
  rtb_sincos_o2_idx_0 = rtb_MotorBR * rtb_fcn2;

  /* Product: '<S218>/Product1' incorporates:
   *  Product: '<S220>/Product1'
   */
  rtb_sincos_o2_idx_2 = rtb_fcn3 * rtb_MotorBL;

  /* Gain: '<S218>/Gain' incorporates:
   *  Product: '<S218>/Product1'
   *  Product: '<S218>/Product2'
   *  Sum: '<S218>/Sum'
   */
  rtb_VectorConcatenate[7] = (rtb_sincos_o2_idx_2 + rtb_sincos_o2_idx_0) * 2.0;

  /* Gain: '<S219>/Gain' incorporates:
   *  Sum: '<S219>/Sum'
   */
  rtb_VectorConcatenate[2] = (rtb_sincos_o2_idx_0_tmp_0 + u1_tmp) * 2.0;

  /* Gain: '<S220>/Gain' incorporates:
   *  Sum: '<S220>/Sum'
   */
  rtb_VectorConcatenate[5] = (rtb_sincos_o2_idx_0 - rtb_sincos_o2_idx_2) * 2.0;

  /* Sum: '<S221>/Sum' */
  rtb_VectorConcatenate[8] = (rtb_fcn4 - rtb_Saturation_idx_0) +
    rtb_Saturation_idx_1;

  /* SampleTimeMath: '<S211>/TSamp'
   *
   * About '<S211>/TSamp':
   *  y = u * K where K = 1 / ( w * Ts )
   */
  rtb_preprocessing_alt_ned = rtb_preprocessing_alt_out * 100.0F;

  /* Outputs for Atomic SubSystem: '<S2>/OrientationEstimator' */
  for (i = 0; i < 3; i++) {
    /* DataTypeConversion: '<S209>/orientation_RPY2single' */
    Subtract1[i] = (real32_T)rtb_VectorConcatenate_p[i];

    /* Product: '<S208>/altitude_matrix_multiply' incorporates:
     *  Concatenate: '<S222>/Vector Concatenate'
     *  DataTypeConversion: '<S208>/altitude_DCM2single'
     *  DiscreteFir: '<S210>/FIR_IMUaccel'
     */
    rtb_VectorConcatenate_0[i] = (real32_T)rtb_VectorConcatenate[3 * i + 2] *
      rtb_preprocessing_accel_out_i_1 + ((real32_T)rtb_VectorConcatenate[3 * i +
      1] * rtb_preprocessing_accel_out_i_0 + (real32_T)rtb_VectorConcatenate[3 *
      i] * rtb_preprocessing_accel_out_idx);
  }

  /* End of Outputs for SubSystem: '<S2>/OrientationEstimator' */

  /* Sum: '<S208>/altitude_velocity_baroCorrection' incorporates:
   *  Delay: '<S208>/altitude_delay0'
   *  Gain: '<S208>/altitude_Ts_Gain0'
   *  Gain: '<S208>/altitude_compFilter_accel_Gain0'
   *  Gain: '<S208>/altitude_compFilter_baro_Gain0'
   *  Product: '<S208>/altitude_matrix_multiply'
   *  Sum: '<S208>/altitude_velCorrection'
   *  Sum: '<S211>/Diff'
   *  UnitDelay: '<S211>/UD'
   *
   * Block description for '<S211>/Diff':
   *
   *  Add in CPU
   *
   * Block description for '<S211>/UD':
   *
   *  Store in Global RAM
   */
  IIR_IMUgyro_r_tmp = (0.01F * rtb_VectorConcatenate_0[2] +
                       rtDW->altitude_delay0_DSTATE) * 0.999F +
    (rtb_preprocessing_alt_ned - rtDW->UD_DSTATE) * 0.001F;

  /* Sum: '<S208>/altitude_baroCorrection' incorporates:
   *  Delay: '<S208>/altitude_delay'
   *  Gain: '<S208>/altitude_Ts_Gain'
   *  Gain: '<S208>/altitude_compFilter_accel_Gain'
   *  Gain: '<S208>/altitude_compFilter_baro_Gain'
   *  Sum: '<S208>/altitude_accelCorrection'
   */
  rtb_preprocessing_alt_out = (0.01F * IIR_IMUgyro_r_tmp +
    rtDW->altitude_delay_DSTATE) * 0.99F + 0.01F * rtb_preprocessing_alt_out;

  /* Update for Delay: '<S208>/altitude_delay0' */
  rtDW->altitude_delay0_DSTATE = IIR_IMUgyro_r_tmp;

  /* Update for UnitDelay: '<S211>/UD'
   *
   * Block description for '<S211>/UD':
   *
   *  Store in Global RAM
   */
  rtDW->UD_DSTATE = rtb_preprocessing_alt_ned;

  /* Update for Delay: '<S208>/altitude_delay' */
  rtDW->altitude_delay_DSTATE = rtb_preprocessing_alt_out;

  /* End of Outputs for SubSystem: '<S2>/AltitudeEstimator' */
  /* End of Outputs for SubSystem: '<Root>/StatesEstimator' */

  /* Outputs for Atomic SubSystem: '<Root>/FCS' */
  /* Outputs for Atomic SubSystem: '<S1>/CommandHandling' */
  /* Saturate: '<S3>/Saturation3' incorporates:
   *  Inport: '<Root>/Commands'
   */
  if (rtU_Commands->control_input.alt_cmd > 10.0F) {
    rtb_preprocessing_alt_ned = 10.0F;
  } else if (rtU_Commands->control_input.alt_cmd < -10.0F) {
    rtb_preprocessing_alt_ned = -10.0F;
  } else {
    rtb_preprocessing_alt_ned = rtU_Commands->control_input.alt_cmd;
  }

  /* End of Saturate: '<S3>/Saturation3' */
  /* End of Outputs for SubSystem: '<S1>/CommandHandling' */

  /* Outputs for Atomic SubSystem: '<S1>/Controller' */
  /* Outputs for Atomic SubSystem: '<S4>/AltitudeControl' */
  /* Sum: '<S6>/Sum' incorporates:
   *  Gain: '<S6>/Gain2'
   */
  IIR_IMUgyro_r_tmp = 0;
  rtb_preprocessing_alt_ned -= -IIR_IMUgyro_r_tmp;

  /* Sum: '<S6>/Sum2' incorporates:
   *  Constant: '<S6>/Constant'
   *  DiscreteIntegrator: '<S41>/Integrator'
   *  Gain: '<S46>/Proportional Gain'
   *  Sum: '<S50>/Sum'
   */
  rtb_MotorBR = (475.622192F * rtb_preprocessing_alt_ned +
                 rtDW->Integrator_DSTATE_m) + 244.2193;

  /* Update for DiscreteIntegrator: '<S41>/Integrator' incorporates:
   *  Gain: '<S38>/Integral Gain'
   */
  rtDW->Integrator_DSTATE_m += 234.461655F * rtb_preprocessing_alt_ned * 0.01F;

  /* End of Outputs for SubSystem: '<S4>/AltitudeControl' */
  /* End of Outputs for SubSystem: '<S1>/Controller' */

  /* Outputs for Atomic SubSystem: '<S1>/CommandHandling' */
  /* Saturate: '<S3>/Saturation1' incorporates:
   *  Inport: '<Root>/Commands'
   */
  /* Unit Conversion - from: deg to: rad
     Expression: output = (0.0174533*input) + (0) */
  if (rtU_Commands->control_input.pitch_cmd > 45.0F) {
    rtb_preprocessing_alt_ned = 45.0F;
  } else if (rtU_Commands->control_input.pitch_cmd < -45.0F) {
    rtb_preprocessing_alt_ned = -45.0F;
  } else {
    rtb_preprocessing_alt_ned = rtU_Commands->control_input.pitch_cmd;
  }

  /* End of Saturate: '<S3>/Saturation1' */
  /* End of Outputs for SubSystem: '<S1>/CommandHandling' */

  /* Outputs for Atomic SubSystem: '<S1>/Controller' */
  /* Outputs for Atomic SubSystem: '<S4>/PitchRollControl' */
  /* Sum: '<S7>/Sum2' incorporates:
   *  UnitConversion: '<S58>/Unit Conversion'
   */
  rtb_preprocessing_alt_ned = 0.0174532924F * rtb_preprocessing_alt_ned -
    Subtract1[1];

  /* Gain: '<S96>/Filter Coefficient' incorporates:
   *  DiscreteIntegrator: '<S88>/Filter'
   *  Gain: '<S87>/Derivative Gain'
   *  Sum: '<S88>/SumD'
   */
  rtb_preprocessing_accel_out_idx = (PID_pitch_D * rtb_preprocessing_alt_ned -
    rtDW->Filter_DSTATE_k) * PID_pitch_N;

  /* Sum: '<S102>/Sum' incorporates:
   *  DiscreteIntegrator: '<S93>/Integrator'
   *  Gain: '<S98>/Proportional Gain'
   */
  pressureFilter_IIR_tmp = (PID_pitch_P * rtb_preprocessing_alt_ned +
    rtDW->Integrator_DSTATE_n) + rtb_preprocessing_accel_out_idx;

  /* Gain: '<S90>/Integral Gain' */
  Subtract2_idx_0 = PID_pitch_I * rtb_preprocessing_alt_ned;

  /* End of Outputs for SubSystem: '<S4>/PitchRollControl' */
  /* End of Outputs for SubSystem: '<S1>/Controller' */

  /* Outputs for Atomic SubSystem: '<S1>/CommandHandling' */
  /* Saturate: '<S3>/Saturation2' incorporates:
   *  Inport: '<Root>/Commands'
   */
  /* Unit Conversion - from: deg to: rad
     Expression: output = (0.0174533*input) + (0) */
  if (rtU_Commands->control_input.roll_cmd > 45.0F) {
    rtb_preprocessing_alt_ned = 45.0F;
  } else if (rtU_Commands->control_input.roll_cmd < -45.0F) {
    rtb_preprocessing_alt_ned = -45.0F;
  } else {
    rtb_preprocessing_alt_ned = rtU_Commands->control_input.roll_cmd;
  }

  /* End of Saturate: '<S3>/Saturation2' */
  /* End of Outputs for SubSystem: '<S1>/CommandHandling' */

  /* Outputs for Atomic SubSystem: '<S1>/Controller' */
  /* Outputs for Atomic SubSystem: '<S4>/PitchRollControl' */
  /* Sum: '<S7>/Sum3' incorporates:
   *  UnitConversion: '<S59>/Unit Conversion'
   */
  rtb_preprocessing_alt_ned = 0.0174532924F * rtb_preprocessing_alt_ned -
    Subtract1[2];

  /* Gain: '<S144>/Filter Coefficient' incorporates:
   *  DiscreteIntegrator: '<S136>/Filter'
   *  Gain: '<S135>/Derivative Gain'
   *  Sum: '<S136>/SumD'
   */
  rtb_MotorBL = (PID_pitch_D * rtb_preprocessing_alt_ned - rtDW->Filter_DSTATE) *
    PID_pitch_N;

  /* Sum: '<S150>/Sum' incorporates:
   *  DiscreteIntegrator: '<S141>/Integrator'
   *  Gain: '<S146>/Proportional Gain'
   */
  rtb_fcn3 = ((real_T)(PID_pitch_P * rtb_preprocessing_alt_ned) +
              rtDW->Integrator_DSTATE_j) + rtb_MotorBL;

  /* Update for DiscreteIntegrator: '<S88>/Filter' */
  rtDW->Filter_DSTATE_k += 0.01F * rtb_preprocessing_accel_out_idx;

  /* Update for DiscreteIntegrator: '<S93>/Integrator' */
  rtDW->Integrator_DSTATE_n += 0.01F * Subtract2_idx_0;

  /* Update for DiscreteIntegrator: '<S136>/Filter' */
  rtDW->Filter_DSTATE += 0.01 * rtb_MotorBL;

  /* Update for DiscreteIntegrator: '<S141>/Integrator' incorporates:
   *  Gain: '<S138>/Integral Gain'
   */
  rtDW->Integrator_DSTATE_j += PID_pitch_I * rtb_preprocessing_alt_ned * 0.01F;

  /* End of Outputs for SubSystem: '<S4>/PitchRollControl' */
  /* End of Outputs for SubSystem: '<S1>/Controller' */

  /* Outputs for Atomic SubSystem: '<S1>/CommandHandling' */
  /* Saturate: '<S3>/Saturation' incorporates:
   *  Inport: '<Root>/Commands'
   */
  /* Unit Conversion - from: deg to: rad
     Expression: output = (0.0174533*input) + (0) */
  if (rtU_Commands->control_input.yaw_cmd > 180.0F) {
    rtb_preprocessing_alt_ned = 180.0F;
  } else if (rtU_Commands->control_input.yaw_cmd < -180.0F) {
    rtb_preprocessing_alt_ned = -180.0F;
  } else {
    rtb_preprocessing_alt_ned = rtU_Commands->control_input.yaw_cmd;
  }

  /* End of Saturate: '<S3>/Saturation' */
  /* End of Outputs for SubSystem: '<S1>/CommandHandling' */

  /* Outputs for Atomic SubSystem: '<S1>/Controller' */
  /* Outputs for Atomic SubSystem: '<S4>/YawControl' */
  /* Outputs for Atomic SubSystem: '<Root>/StatesEstimator' */
  /* Outputs for Atomic SubSystem: '<S2>/OrientationEstimator' */
  /* Sum: '<S8>/Sum' incorporates:
   *  Concatenate: '<S210>/preprocessing_pqr_concat'
   *  Inport: '<S209>/<gyro_data_SI>'
   *  UnitConversion: '<S158>/Unit Conversion'
   */
  rtb_preprocessing_alt_ned = 0.0174532924F * rtb_preprocessing_alt_ned -
    rtb_preprocessing_gyro_out_idx_;

  /* End of Outputs for SubSystem: '<S2>/OrientationEstimator' */
  /* End of Outputs for SubSystem: '<Root>/StatesEstimator' */

  /* Sum: '<S200>/Sum' incorporates:
   *  DiscreteIntegrator: '<S191>/Integrator'
   *  Gain: '<S196>/Proportional Gain'
   */
  rtb_preprocessing_accel_out_idx = -35.9833298F * rtb_preprocessing_alt_ned +
    rtDW->Integrator_DSTATE;

  /* Update for DiscreteIntegrator: '<S191>/Integrator' incorporates:
   *  Gain: '<S188>/Integral Gain'
   */
  rtDW->Integrator_DSTATE += -10.7215109F * rtb_preprocessing_alt_ned * 0.01F;

  /* End of Outputs for SubSystem: '<S4>/YawControl' */
  /* End of Outputs for SubSystem: '<S1>/Controller' */

  /* Outputs for Atomic SubSystem: '<S1>/MotorMixing' */
  /* Sum: '<S5>/MotorFR' incorporates:
   *  Sum: '<S5>/MotorBL'
   */
  rtb_fcn2 = rtb_MotorBR + rtb_preprocessing_accel_out_idx;
  rtb_MotorBL = (rtb_fcn2 + pressureFilter_IIR_tmp) - rtb_fcn3;

  /* Saturate: '<S5>/Saturation' */
  if (rtb_MotorBL > 1000.0) {
    /* Outport: '<Root>/Throttle' incorporates:
     *  DataTypeConversion: '<S5>/Data Type Conversion'
     */
    rtY_Throttle[0] = 1000U;
  } else if (rtb_MotorBL < 100.0) {
    /* Outport: '<Root>/Throttle' incorporates:
     *  DataTypeConversion: '<S5>/Data Type Conversion'
     */
    rtY_Throttle[0] = 100U;
  } else {
    /* Outport: '<Root>/Throttle' incorporates:
     *  DataTypeConversion: '<S5>/Data Type Conversion'
     */
    rtY_Throttle[0] = (uint16_T)rtb_MotorBL;
  }

  /* Sum: '<S5>/MotorFL' incorporates:
   *  Sum: '<S5>/MotorBR'
   */
  rtb_MotorBR -= rtb_preprocessing_accel_out_idx;
  rtb_MotorBL = (rtb_MotorBR + pressureFilter_IIR_tmp) + rtb_fcn3;

  /* Saturate: '<S5>/Saturation' */
  if (rtb_MotorBL > 1000.0) {
    /* Outport: '<Root>/Throttle' incorporates:
     *  DataTypeConversion: '<S5>/Data Type Conversion'
     */
    rtY_Throttle[1] = 1000U;
  } else if (rtb_MotorBL < 100.0) {
    /* Outport: '<Root>/Throttle' incorporates:
     *  DataTypeConversion: '<S5>/Data Type Conversion'
     */
    rtY_Throttle[1] = 100U;
  } else {
    /* Outport: '<Root>/Throttle' incorporates:
     *  DataTypeConversion: '<S5>/Data Type Conversion'
     */
    rtY_Throttle[1] = (uint16_T)rtb_MotorBL;
  }

  /* Sum: '<S5>/MotorBR' */
  rtb_MotorBL = (rtb_MotorBR - pressureFilter_IIR_tmp) - rtb_fcn3;

  /* Saturate: '<S5>/Saturation' */
  if (rtb_MotorBL > 1000.0) {
    /* Outport: '<Root>/Throttle' incorporates:
     *  DataTypeConversion: '<S5>/Data Type Conversion'
     */
    rtY_Throttle[2] = 1000U;
  } else if (rtb_MotorBL < 100.0) {
    /* Outport: '<Root>/Throttle' incorporates:
     *  DataTypeConversion: '<S5>/Data Type Conversion'
     */
    rtY_Throttle[2] = 100U;
  } else {
    /* Outport: '<Root>/Throttle' incorporates:
     *  DataTypeConversion: '<S5>/Data Type Conversion'
     */
    rtY_Throttle[2] = (uint16_T)rtb_MotorBL;
  }

  /* Sum: '<S5>/MotorBL' */
  rtb_MotorBL = (rtb_fcn2 - pressureFilter_IIR_tmp) + rtb_fcn3;

  /* Saturate: '<S5>/Saturation' */
  if (rtb_MotorBL > 1000.0) {
    /* Outport: '<Root>/Throttle' incorporates:
     *  DataTypeConversion: '<S5>/Data Type Conversion'
     */
    rtY_Throttle[3] = 1000U;
  } else if (rtb_MotorBL < 100.0) {
    /* Outport: '<Root>/Throttle' incorporates:
     *  DataTypeConversion: '<S5>/Data Type Conversion'
     */
    rtY_Throttle[3] = 100U;
  } else {
    /* Outport: '<Root>/Throttle' incorporates:
     *  DataTypeConversion: '<S5>/Data Type Conversion'
     */
    rtY_Throttle[3] = (uint16_T)rtb_MotorBL;
  }

  /* End of Outputs for SubSystem: '<S1>/MotorMixing' */
  /* End of Outputs for SubSystem: '<Root>/FCS' */

  /* Outport: '<Root>/State_Estim' incorporates:
   *  BusCreator generated from: '<Root>/State_Estim'
   *  Concatenate: '<S210>/preprocessing_pqr_concat'
   *  Inport: '<S209>/<gyro_data_SI>'
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
    /* InitializeConditions for DiscreteFir: '<S210>/FIR_IMUaccel' */
    rtDW->FIR_IMUaccel_circBuf = 0;
    for (i = 0; i < 15; i++) {
      rtDW->FIR_IMUaccel_states[i] = 0.0F;
    }

    /* End of InitializeConditions for DiscreteFir: '<S210>/FIR_IMUaccel' */

    /* InitializeConditions for DiscreteFir: '<S210>/FIR_IMUgyro' */
    rtDW->FIR_IMUgyro_circBuf = 0;
    for (i = 0; i < 10; i++) {
      rtDW->FIR_IMUgyro_states[i] = 0.0F;
    }

    /* End of InitializeConditions for DiscreteFir: '<S210>/FIR_IMUgyro' */
    /* End of SystemInitialize for SubSystem: '<S2>/SensorPreprocessing' */
    /* End of SystemInitialize for SubSystem: '<Root>/StatesEstimator' */
  }
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
