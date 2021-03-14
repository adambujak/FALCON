/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: flightController.c
 *
 * Code generated for Simulink model 'flightController'.
 *
 * Model version                  : 1.113
 * Simulink Coder version         : 9.4 (R2020b) 29-Jul-2020
 * C/C++ source code generated on : Sat Mar 13 16:54:18 2021
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: ARM Compatible->ARM Cortex-M
 * Code generation objectives:
 *    1. Execution efficiency
 *    2. RAM efficiency
 * Validation result: Not run
 */

#include "flightController.h"
#include "motors.h"

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

const FCS_command_t flightController_rtZFCS_command = { 0U,/* control_type */
  { 0.0F,                              /* yaw_cmd */
    0.0F,                              /* roll_cmd */
    0.0F,                              /* pitch_cmd */
    0.0F                               /* alt_cmd */
  }                                    /* control_input */
};

const sensor_data_t flightController_rtZsensor_data = { { 0.0F, 0.0F, 0.0F },/* gyro_data_SI */
  { 0.0F, 0.0F, 0.0F },                /* accel_data_SI */
  { 1.0F, 0.0F, 0.0F, 0.0F },          /* quat_data */
  0.0F                                 /* alt_data_SI */
};

/* Model step function */
void flightController_step(RT_MODEL *const rtM, FCS_command_t *rtU_Commands,
  sensor_data_t *rtU_Sensors, states_estimate_t *rtY_State_Estim)
{
  DW *rtDW = rtM->dwork;
  real_T rtb_VectorConcatenate_b[9];
  real_T rtb_VectorConcatenate[3];
  real_T rtb_MotorBL;
  real_T rtb_MotorBR;
  real_T rtb_Product2_m;
  real_T rtb_Product3_b;
  real_T rtb_Product3_g_tmp;
  real_T rtb_VectorConcatenate_i_tmp;
  real_T rtb_VectorConcatenate_i_tmp_0;
  real_T rtb_VectorConcatenate_tmp;
  real_T rtb_VectorConcatenate_tmp_0;
  real_T rtb_VectorConcatenate_tmp_1;
  real_T rtb_VectorConcatenate_tmp_2;
  real_T rtb_fcn3;
  int32_T cff;
  int32_T i;
  real32_T rtb_VectorConcatenate_i[3];
  real32_T rtb_orientation_RPY_out[3];
  real32_T pressureFilter_IIR_tmp;
  real32_T rtb_Sum_k3;
  real32_T rtb_preprocessing_accel_out_i_0;
  real32_T rtb_preprocessing_accel_out_i_1;
  real32_T rtb_preprocessing_accel_out_idx;
  real32_T rtb_preprocessing_alt_ned;
  real32_T rtb_preprocessing_alt_out;
  real32_T rtb_preprocessing_gyro_out_idx_;

  /* Outputs for Atomic SubSystem: '<Root>/StatesEstimator' */
  /* Outputs for Atomic SubSystem: '<S2>/SensorPreprocessing' */
  /* DiscreteFir: '<S212>/FIR_IMUaccel' incorporates:
   *  Inport: '<Root>/Sensors'
   */
  rtb_preprocessing_alt_ned = rtU_Sensors->accel_data_SI[0] * 0.0264077242F;
  cff = 1;
  for (i = rtDW->FIR_IMUaccel_circBuf; i < 5; i++) {
    rtb_preprocessing_alt_ned += rtDW->FIR_IMUaccel_states[i] *
      rtConstP.FIR_IMUaccel_Coefficients[cff];
    cff++;
  }

  for (i = 0; i < rtDW->FIR_IMUaccel_circBuf; i++) {
    rtb_preprocessing_alt_ned += rtDW->FIR_IMUaccel_states[i] *
      rtConstP.FIR_IMUaccel_Coefficients[cff];
    cff++;
  }

  rtb_preprocessing_accel_out_idx = rtb_preprocessing_alt_ned;
  rtb_preprocessing_alt_ned = rtU_Sensors->accel_data_SI[1] * 0.0264077242F;
  cff = 1;
  for (i = rtDW->FIR_IMUaccel_circBuf; i < 5; i++) {
    rtb_preprocessing_alt_ned += rtDW->FIR_IMUaccel_states[i + 5] *
      rtConstP.FIR_IMUaccel_Coefficients[cff];
    cff++;
  }

  for (i = 0; i < rtDW->FIR_IMUaccel_circBuf; i++) {
    rtb_preprocessing_alt_ned += rtDW->FIR_IMUaccel_states[i + 5] *
      rtConstP.FIR_IMUaccel_Coefficients[cff];
    cff++;
  }

  rtb_preprocessing_accel_out_i_0 = rtb_preprocessing_alt_ned;
  rtb_preprocessing_alt_ned = rtU_Sensors->accel_data_SI[2] * 0.0264077242F;
  cff = 1;
  for (i = rtDW->FIR_IMUaccel_circBuf; i < 5; i++) {
    rtb_preprocessing_alt_ned += rtDW->FIR_IMUaccel_states[i + 10] *
      rtConstP.FIR_IMUaccel_Coefficients[cff];
    cff++;
  }

  for (i = 0; i < rtDW->FIR_IMUaccel_circBuf; i++) {
    rtb_preprocessing_alt_ned += rtDW->FIR_IMUaccel_states[i + 10] *
      rtConstP.FIR_IMUaccel_Coefficients[cff];
    cff++;
  }

  rtb_preprocessing_accel_out_i_1 = rtb_preprocessing_alt_ned;

  /* End of DiscreteFir: '<S212>/FIR_IMUaccel' */

  /* DiscreteFilter: '<S212>/pressureFilter_IIR' incorporates:
   *  Gain: '<S212>/Alt2NED'
   *  Inport: '<Root>/Sensors'
   */
  pressureFilter_IIR_tmp = ((-rtU_Sensors->alt_data_SI - -2.93717074F *
    rtDW->pressureFilter_IIR_states[0]) - 2.87629962F *
    rtDW->pressureFilter_IIR_states[1]) - -0.939098954F *
    rtDW->pressureFilter_IIR_states[2];
  rtb_preprocessing_alt_out = ((3.75683794E-6F * pressureFilter_IIR_tmp +
    1.12705138E-5F * rtDW->pressureFilter_IIR_states[0]) + 1.12705138E-5F *
    rtDW->pressureFilter_IIR_states[1]) + 3.75683794E-6F *
    rtDW->pressureFilter_IIR_states[2];

  /* DiscreteFir: '<S212>/FIR_IMUgyro' incorporates:
   *  Inport: '<Root>/Sensors'
   */
  rtb_preprocessing_alt_ned = rtU_Sensors->gyro_data_SI[0] * -2.48169962E-18F;
  cff = 1;
  for (i = rtDW->FIR_IMUgyro_circBuf; i < 5; i++) {
    rtb_preprocessing_alt_ned += rtDW->FIR_IMUgyro_states[i] *
      rtConstP.FIR_IMUgyro_Coefficients[cff];
    cff++;
  }

  for (i = 0; i < rtDW->FIR_IMUgyro_circBuf; i++) {
    rtb_preprocessing_alt_ned += rtDW->FIR_IMUgyro_states[i] *
      rtConstP.FIR_IMUgyro_Coefficients[cff];
    cff++;
  }

  /* Outport: '<Root>/State_Estim' incorporates:
   *  DiscreteFir: '<S212>/FIR_IMUgyro'
   */
  rtY_State_Estim->p = rtb_preprocessing_alt_ned;

  /* DiscreteFir: '<S212>/FIR_IMUgyro' incorporates:
   *  Inport: '<Root>/Sensors'
   */
  rtb_preprocessing_alt_ned = rtU_Sensors->gyro_data_SI[1] * -2.48169962E-18F;
  cff = 1;
  for (i = rtDW->FIR_IMUgyro_circBuf; i < 5; i++) {
    rtb_preprocessing_alt_ned += rtDW->FIR_IMUgyro_states[i + 5] *
      rtConstP.FIR_IMUgyro_Coefficients[cff];
    cff++;
  }

  for (i = 0; i < rtDW->FIR_IMUgyro_circBuf; i++) {
    rtb_preprocessing_alt_ned += rtDW->FIR_IMUgyro_states[i + 5] *
      rtConstP.FIR_IMUgyro_Coefficients[cff];
    cff++;
  }

  /* Outport: '<Root>/State_Estim' incorporates:
   *  DiscreteFir: '<S212>/FIR_IMUgyro'
   */
  rtY_State_Estim->q = rtb_preprocessing_alt_ned;

  /* DiscreteFilter: '<S212>/IIR_IMUgyro_r' incorporates:
   *  Inport: '<Root>/Sensors'
   */
  rtb_preprocessing_alt_ned = rtU_Sensors->gyro_data_SI[2];
  cff = 1;
  for (i = 0; i < 5; i++) {
    rtb_preprocessing_alt_ned -= rtConstP.IIR_IMUgyro_r_DenCoef[cff] *
      rtDW->IIR_IMUgyro_r_states[i];
    cff++;
  }

  rtb_Sum_k3 = 0.282124132F * rtb_preprocessing_alt_ned;
  cff = 1;
  for (i = 0; i < 5; i++) {
    rtb_Sum_k3 += rtConstP.IIR_IMUgyro_r_NumCoef[cff] *
      rtDW->IIR_IMUgyro_r_states[i];
    cff++;
  }

  rtb_preprocessing_gyro_out_idx_ = rtb_Sum_k3;

  /* End of DiscreteFilter: '<S212>/IIR_IMUgyro_r' */

  /* Update for DiscreteFir: '<S212>/FIR_IMUaccel' incorporates:
   *  Inport: '<Root>/Sensors'
   */
  /* Update circular buffer index */
  rtDW->FIR_IMUaccel_circBuf--;
  if (rtDW->FIR_IMUaccel_circBuf < 0) {
    rtDW->FIR_IMUaccel_circBuf = 4;
  }

  /* Update circular buffer */
  rtDW->FIR_IMUaccel_states[rtDW->FIR_IMUaccel_circBuf] =
    rtU_Sensors->accel_data_SI[0];
  rtDW->FIR_IMUaccel_states[rtDW->FIR_IMUaccel_circBuf + 5] =
    rtU_Sensors->accel_data_SI[1];
  rtDW->FIR_IMUaccel_states[rtDW->FIR_IMUaccel_circBuf + 10] =
    rtU_Sensors->accel_data_SI[2];

  /* End of Update for DiscreteFir: '<S212>/FIR_IMUaccel' */

  /* Update for DiscreteFilter: '<S212>/pressureFilter_IIR' */
  rtDW->pressureFilter_IIR_states[2] = rtDW->pressureFilter_IIR_states[1];
  rtDW->pressureFilter_IIR_states[1] = rtDW->pressureFilter_IIR_states[0];
  rtDW->pressureFilter_IIR_states[0] = pressureFilter_IIR_tmp;

  /* Update for DiscreteFir: '<S212>/FIR_IMUgyro' incorporates:
   *  Inport: '<Root>/Sensors'
   */
  /* Update circular buffer index */
  rtDW->FIR_IMUgyro_circBuf--;
  if (rtDW->FIR_IMUgyro_circBuf < 0) {
    rtDW->FIR_IMUgyro_circBuf = 4;
  }

  /* Update circular buffer */
  rtDW->FIR_IMUgyro_states[rtDW->FIR_IMUgyro_circBuf] =
    rtU_Sensors->gyro_data_SI[0];
  rtDW->FIR_IMUgyro_states[rtDW->FIR_IMUgyro_circBuf + 5] =
    rtU_Sensors->gyro_data_SI[1];

  /* End of Update for DiscreteFir: '<S212>/FIR_IMUgyro' */

  /* Update for DiscreteFilter: '<S212>/IIR_IMUgyro_r' */
  rtDW->IIR_IMUgyro_r_states[4] = rtDW->IIR_IMUgyro_r_states[3];
  rtDW->IIR_IMUgyro_r_states[3] = rtDW->IIR_IMUgyro_r_states[2];
  rtDW->IIR_IMUgyro_r_states[2] = rtDW->IIR_IMUgyro_r_states[1];
  rtDW->IIR_IMUgyro_r_states[1] = rtDW->IIR_IMUgyro_r_states[0];
  rtDW->IIR_IMUgyro_r_states[0] = rtb_preprocessing_alt_ned;

  /* End of Outputs for SubSystem: '<S2>/SensorPreprocessing' */

  /* Outputs for Atomic SubSystem: '<S2>/AltitudeEstimator' */
  /* Outputs for Atomic SubSystem: '<S2>/OrientationEstimator' */
  /* Sqrt: '<S235>/sqrt' incorporates:
   *  DataTypeConversion: '<S211>/orientation_quat2double'
   *  Inport: '<Root>/Sensors'
   *  Inport: '<S211>/<quat_data>'
   *  Product: '<S236>/Product'
   *  Product: '<S236>/Product1'
   *  Product: '<S236>/Product2'
   *  Product: '<S236>/Product3'
   *  Sqrt: '<S226>/sqrt'
   *  Sum: '<S236>/Sum'
   */
  rtb_Product3_g_tmp = sqrt((((real_T)rtU_Sensors->quat_data[0] *
    rtU_Sensors->quat_data[0] + (real_T)rtU_Sensors->quat_data[1] *
    rtU_Sensors->quat_data[1]) + (real_T)rtU_Sensors->quat_data[2] *
    rtU_Sensors->quat_data[2]) + (real_T)rtU_Sensors->quat_data[3] *
    rtU_Sensors->quat_data[3]);

  /* End of Outputs for SubSystem: '<S2>/AltitudeEstimator' */

  /* Product: '<S230>/Product' incorporates:
   *  DataTypeConversion: '<S211>/orientation_quat2double'
   *  Inport: '<Root>/Sensors'
   *  Inport: '<S211>/<quat_data>'
   *  Sqrt: '<S235>/sqrt'
   */
  rtb_MotorBR = rtU_Sensors->quat_data[0] / rtb_Product3_g_tmp;

  /* Product: '<S230>/Product1' incorporates:
   *  DataTypeConversion: '<S211>/orientation_quat2double'
   *  Inport: '<Root>/Sensors'
   *  Inport: '<S211>/<quat_data>'
   *  Sqrt: '<S235>/sqrt'
   */
  rtb_MotorBL = rtU_Sensors->quat_data[1] / rtb_Product3_g_tmp;

  /* Product: '<S230>/Product2' incorporates:
   *  DataTypeConversion: '<S211>/orientation_quat2double'
   *  Inport: '<Root>/Sensors'
   *  Inport: '<S211>/<quat_data>'
   *  Sqrt: '<S235>/sqrt'
   */
  rtb_Product2_m = rtU_Sensors->quat_data[2] / rtb_Product3_g_tmp;

  /* Product: '<S230>/Product3' incorporates:
   *  DataTypeConversion: '<S211>/orientation_quat2double'
   *  Inport: '<Root>/Sensors'
   *  Inport: '<S211>/<quat_data>'
   *  Sqrt: '<S235>/sqrt'
   */
  rtb_Product3_b = rtU_Sensors->quat_data[3] / rtb_Product3_g_tmp;

  /* Fcn: '<S228>/fcn2' incorporates:
   *  Fcn: '<S228>/fcn5'
   */
  rtb_VectorConcatenate_tmp = rtb_MotorBR * rtb_MotorBR;
  rtb_VectorConcatenate_tmp_0 = rtb_MotorBL * rtb_MotorBL;
  rtb_VectorConcatenate_tmp_1 = rtb_Product2_m * rtb_Product2_m;
  rtb_VectorConcatenate_tmp_2 = rtb_Product3_b * rtb_Product3_b;

  /* Trigonometry: '<S229>/Trigonometric Function1' incorporates:
   *  Fcn: '<S228>/fcn1'
   *  Fcn: '<S228>/fcn2'
   */
  rtb_VectorConcatenate[0] = atan2((rtb_MotorBL * rtb_Product2_m + rtb_MotorBR *
    rtb_Product3_b) * 2.0, ((rtb_VectorConcatenate_tmp +
    rtb_VectorConcatenate_tmp_0) - rtb_VectorConcatenate_tmp_1) -
    rtb_VectorConcatenate_tmp_2);

  /* Fcn: '<S228>/fcn3' */
  rtb_fcn3 = (rtb_MotorBL * rtb_Product3_b - rtb_MotorBR * rtb_Product2_m) *
    -2.0;

  /* If: '<S231>/If' incorporates:
   *  Constant: '<S232>/Constant'
   *  Constant: '<S233>/Constant'
   */
  if (rtb_fcn3 > 1.0) {
    /* Outputs for IfAction SubSystem: '<S231>/If Action Subsystem' incorporates:
     *  ActionPort: '<S232>/Action Port'
     */
    rtb_fcn3 = 1.0;

    /* End of Outputs for SubSystem: '<S231>/If Action Subsystem' */
  } else {
    if (rtb_fcn3 < -1.0) {
      /* Outputs for IfAction SubSystem: '<S231>/If Action Subsystem1' incorporates:
       *  ActionPort: '<S233>/Action Port'
       */
      rtb_fcn3 = 1.0;

      /* End of Outputs for SubSystem: '<S231>/If Action Subsystem1' */
    }
  }

  /* End of If: '<S231>/If' */

  /* Trigonometry: '<S229>/trigFcn' */
  rtb_VectorConcatenate[1] = asin(rtb_fcn3);

  /* Trigonometry: '<S229>/Trigonometric Function3' incorporates:
   *  Fcn: '<S228>/fcn4'
   *  Fcn: '<S228>/fcn5'
   */
  rtb_VectorConcatenate[2] = atan2((rtb_Product2_m * rtb_Product3_b +
    rtb_MotorBR * rtb_MotorBL) * 2.0, ((rtb_VectorConcatenate_tmp -
    rtb_VectorConcatenate_tmp_0) - rtb_VectorConcatenate_tmp_1) +
    rtb_VectorConcatenate_tmp_2);

  /* Outputs for Atomic SubSystem: '<S2>/AltitudeEstimator' */
  /* Product: '<S225>/Product3' incorporates:
   *  DataTypeConversion: '<S211>/orientation_quat2double'
   *  Inport: '<Root>/Sensors'
   *  Inport: '<S211>/<quat_data>'
   */
  rtb_Product3_b = rtU_Sensors->quat_data[3] / rtb_Product3_g_tmp;

  /* Product: '<S225>/Product2' incorporates:
   *  DataTypeConversion: '<S211>/orientation_quat2double'
   *  Inport: '<Root>/Sensors'
   *  Inport: '<S211>/<quat_data>'
   */
  rtb_MotorBR = rtU_Sensors->quat_data[2] / rtb_Product3_g_tmp;

  /* Product: '<S225>/Product1' incorporates:
   *  DataTypeConversion: '<S211>/orientation_quat2double'
   *  Inport: '<Root>/Sensors'
   *  Inport: '<S211>/<quat_data>'
   */
  rtb_MotorBL = rtU_Sensors->quat_data[1] / rtb_Product3_g_tmp;

  /* Product: '<S225>/Product' incorporates:
   *  DataTypeConversion: '<S211>/orientation_quat2double'
   *  Inport: '<Root>/Sensors'
   *  Inport: '<S211>/<quat_data>'
   */
  rtb_fcn3 = rtU_Sensors->quat_data[0] / rtb_Product3_g_tmp;

  /* End of Outputs for SubSystem: '<S2>/OrientationEstimator' */

  /* Product: '<S215>/Product3' incorporates:
   *  Product: '<S219>/Product3'
   */
  rtb_Product3_g_tmp = rtb_fcn3 * rtb_fcn3;

  /* Product: '<S215>/Product2' incorporates:
   *  Product: '<S219>/Product2'
   */
  rtb_Product2_m = rtb_MotorBL * rtb_MotorBL;

  /* Product: '<S215>/Product1' incorporates:
   *  Product: '<S219>/Product1'
   *  Product: '<S223>/Product1'
   */
  rtb_VectorConcatenate_tmp = rtb_MotorBR * rtb_MotorBR;

  /* Product: '<S215>/Product' incorporates:
   *  Product: '<S219>/Product'
   *  Product: '<S223>/Product'
   */
  rtb_VectorConcatenate_tmp_0 = rtb_Product3_b * rtb_Product3_b;

  /* Sum: '<S215>/Sum' incorporates:
   *  Product: '<S215>/Product'
   *  Product: '<S215>/Product1'
   *  Product: '<S215>/Product2'
   *  Product: '<S215>/Product3'
   */
  rtb_VectorConcatenate_b[0] = ((rtb_Product3_g_tmp + rtb_Product2_m) -
    rtb_VectorConcatenate_tmp) - rtb_VectorConcatenate_tmp_0;

  /* Product: '<S216>/Product2' incorporates:
   *  Product: '<S218>/Product2'
   */
  rtb_VectorConcatenate_tmp_1 = rtb_MotorBL * rtb_MotorBR;

  /* Product: '<S216>/Product3' incorporates:
   *  Product: '<S218>/Product3'
   */
  rtb_VectorConcatenate_tmp_2 = rtb_Product3_b * rtb_fcn3;

  /* Gain: '<S216>/Gain' incorporates:
   *  Product: '<S216>/Product2'
   *  Product: '<S216>/Product3'
   *  Sum: '<S216>/Sum'
   */
  rtb_VectorConcatenate_b[3] = (rtb_VectorConcatenate_tmp_2 +
    rtb_VectorConcatenate_tmp_1) * 2.0;

  /* Product: '<S217>/Product1' incorporates:
   *  Product: '<S221>/Product1'
   */
  rtb_VectorConcatenate_i_tmp = rtb_fcn3 * rtb_MotorBR;

  /* Product: '<S217>/Product2' incorporates:
   *  Product: '<S221>/Product2'
   */
  rtb_VectorConcatenate_i_tmp_0 = rtb_MotorBL * rtb_Product3_b;

  /* Gain: '<S217>/Gain' incorporates:
   *  Product: '<S217>/Product1'
   *  Product: '<S217>/Product2'
   *  Sum: '<S217>/Sum'
   */
  rtb_VectorConcatenate_b[6] = (rtb_VectorConcatenate_i_tmp_0 -
    rtb_VectorConcatenate_i_tmp) * 2.0;

  /* Gain: '<S218>/Gain' incorporates:
   *  Sum: '<S218>/Sum'
   */
  rtb_VectorConcatenate_b[1] = (rtb_VectorConcatenate_tmp_1 -
    rtb_VectorConcatenate_tmp_2) * 2.0;

  /* Sum: '<S219>/Sum' incorporates:
   *  Sum: '<S223>/Sum'
   */
  rtb_Product3_g_tmp -= rtb_Product2_m;
  rtb_VectorConcatenate_b[4] = (rtb_Product3_g_tmp + rtb_VectorConcatenate_tmp)
    - rtb_VectorConcatenate_tmp_0;

  /* Product: '<S220>/Product2' incorporates:
   *  Product: '<S222>/Product2'
   */
  rtb_Product2_m = rtb_MotorBR * rtb_Product3_b;

  /* Product: '<S220>/Product1' incorporates:
   *  Product: '<S222>/Product1'
   */
  rtb_VectorConcatenate_tmp_1 = rtb_fcn3 * rtb_MotorBL;

  /* Gain: '<S220>/Gain' incorporates:
   *  Product: '<S220>/Product1'
   *  Product: '<S220>/Product2'
   *  Sum: '<S220>/Sum'
   */
  rtb_VectorConcatenate_b[7] = (rtb_VectorConcatenate_tmp_1 + rtb_Product2_m) *
    2.0;

  /* Gain: '<S221>/Gain' incorporates:
   *  Sum: '<S221>/Sum'
   */
  rtb_VectorConcatenate_b[2] = (rtb_VectorConcatenate_i_tmp +
    rtb_VectorConcatenate_i_tmp_0) * 2.0;

  /* Gain: '<S222>/Gain' incorporates:
   *  Sum: '<S222>/Sum'
   */
  rtb_VectorConcatenate_b[5] = (rtb_Product2_m - rtb_VectorConcatenate_tmp_1) *
    2.0;

  /* Sum: '<S223>/Sum' */
  rtb_VectorConcatenate_b[8] = (rtb_Product3_g_tmp - rtb_VectorConcatenate_tmp)
    + rtb_VectorConcatenate_tmp_0;

  /* SampleTimeMath: '<S213>/TSamp'
   *
   * About '<S213>/TSamp':
   *  y = u * K where K = 1 / ( w * Ts )
   */
  rtb_preprocessing_alt_ned = rtb_preprocessing_alt_out * 100.0F;

  /* Outputs for Atomic SubSystem: '<S2>/OrientationEstimator' */
  for (i = 0; i < 3; i++) {
    /* DataTypeConversion: '<S211>/orientation_RPY2single' */
    rtb_orientation_RPY_out[i] = (real32_T)rtb_VectorConcatenate[i];

    /* Product: '<S210>/altitude_matrix_multiply' incorporates:
     *  Concatenate: '<S224>/Vector Concatenate'
     *  DataTypeConversion: '<S210>/altitude_DCM2single'
     *  DiscreteFir: '<S212>/FIR_IMUaccel'
     */
    rtb_VectorConcatenate_i[i] = (real32_T)rtb_VectorConcatenate_b[3 * i + 2] *
      rtb_preprocessing_accel_out_i_1 + ((real32_T)rtb_VectorConcatenate_b[3 * i
      + 1] * rtb_preprocessing_accel_out_i_0 + (real32_T)
      rtb_VectorConcatenate_b[3 * i] * rtb_preprocessing_accel_out_idx);
  }

  /* End of Outputs for SubSystem: '<S2>/OrientationEstimator' */

  /* Sum: '<S210>/altitude_velocity_baroCorrection' incorporates:
   *  Delay: '<S210>/altitude_delay0'
   *  Gain: '<S210>/altitude_Ts_Gain0'
   *  Gain: '<S210>/altitude_compFilter_accel_Gain0'
   *  Gain: '<S210>/altitude_compFilter_baro_Gain0'
   *  Product: '<S210>/altitude_matrix_multiply'
   *  Sum: '<S210>/altitude_velCorrection'
   *  Sum: '<S213>/Diff'
   *  UnitDelay: '<S213>/UD'
   *
   * Block description for '<S213>/Diff':
   *
   *  Add in CPU
   *
   * Block description for '<S213>/UD':
   *
   *  Store in Global RAM
   */
  pressureFilter_IIR_tmp = (0.01F * rtb_VectorConcatenate_i[2] +
    rtDW->altitude_delay0_DSTATE) * 0.999F + (rtb_preprocessing_alt_ned -
    rtDW->UD_DSTATE) * 0.001F;

  /* Sum: '<S210>/altitude_baroCorrection' incorporates:
   *  Delay: '<S210>/altitude_delay'
   *  Gain: '<S210>/altitude_Ts_Gain'
   *  Gain: '<S210>/altitude_compFilter_accel_Gain'
   *  Gain: '<S210>/altitude_compFilter_baro_Gain'
   *  Sum: '<S210>/altitude_accelCorrection'
   */
  rtb_preprocessing_alt_out = (0.01F * pressureFilter_IIR_tmp +
    rtDW->altitude_delay_DSTATE) * 0.99F + 0.01F * rtb_preprocessing_alt_out;

  /* Update for Delay: '<S210>/altitude_delay0' */
  rtDW->altitude_delay0_DSTATE = pressureFilter_IIR_tmp;

  /* Update for UnitDelay: '<S213>/UD'
   *
   * Block description for '<S213>/UD':
   *
   *  Store in Global RAM
   */
  rtDW->UD_DSTATE = rtb_preprocessing_alt_ned;

  /* Update for Delay: '<S210>/altitude_delay' */
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
  rtb_preprocessing_alt_ned -= -pressureFilter_IIR_tmp;

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
    rtb_orientation_RPY_out[1];

  /* Gain: '<S96>/Filter Coefficient' incorporates:
   *  DiscreteIntegrator: '<S88>/Filter'
   *  Gain: '<S87>/Derivative Gain'
   *  Sum: '<S88>/SumD'
   */
  rtb_MotorBL = (1.19990897F * rtb_preprocessing_alt_ned - rtDW->Filter_DSTATE) *
    194.459278359077;

  /* Sum: '<S102>/Sum' incorporates:
   *  DiscreteIntegrator: '<S93>/Integrator'
   *  Gain: '<S98>/Proportional Gain'
   */
  rtb_Product3_b = ((real_T)(0.10433244F * rtb_preprocessing_alt_ned) +
                    rtDW->Integrator_DSTATE_n) + rtb_MotorBL;

  /* Gain: '<S90>/Integral Gain' */
  rtb_preprocessing_accel_out_idx = 0.000503859483F * rtb_preprocessing_alt_ned;

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
    rtb_orientation_RPY_out[2];

  /* SampleTimeMath: '<S138>/Tsamp' incorporates:
   *  Gain: '<S135>/Derivative Gain'
   *
   * About '<S138>/Tsamp':
   *  y = u * K where K = 1 / ( w * Ts )
   */
  rtb_preprocessing_accel_out_i_0 = 1.19990897F * rtb_preprocessing_alt_ned *
    100.0F;

  /* Sum: '<S152>/Sum' incorporates:
   *  Delay: '<S136>/UD'
   *  DiscreteIntegrator: '<S143>/Integrator'
   *  Gain: '<S148>/Proportional Gain'
   *  Sum: '<S136>/Diff'
   */
  rtb_Sum_k3 = (0.10433244F * rtb_preprocessing_alt_ned +
                rtDW->Integrator_DSTATE_j) + (rtb_preprocessing_accel_out_i_0 -
    rtDW->UD_DSTATE_k);

  /* Update for DiscreteIntegrator: '<S88>/Filter' */
  rtDW->Filter_DSTATE += 0.01 * rtb_MotorBL;

  /* Update for DiscreteIntegrator: '<S93>/Integrator' */
  rtDW->Integrator_DSTATE_n += 0.01F * rtb_preprocessing_accel_out_idx;

  /* Update for Delay: '<S136>/UD' */
  rtDW->UD_DSTATE_k = rtb_preprocessing_accel_out_i_0;

  /* Update for DiscreteIntegrator: '<S143>/Integrator' incorporates:
   *  Gain: '<S140>/Integral Gain'
   */
  rtDW->Integrator_DSTATE_j += 0.000503859483F * rtb_preprocessing_alt_ned *
    0.01F;

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
   *  Concatenate: '<S212>/preprocessing_pqr_concat'
   *  Inport: '<S211>/<gyro_data_SI>'
   *  UnitConversion: '<S160>/Unit Conversion'
   */
  rtb_preprocessing_alt_ned = 0.0174532924F * rtb_preprocessing_alt_ned -
    rtb_preprocessing_gyro_out_idx_;

  /* End of Outputs for SubSystem: '<S2>/OrientationEstimator' */
  /* End of Outputs for SubSystem: '<Root>/StatesEstimator' */

  /* Sum: '<S202>/Sum' incorporates:
   *  DiscreteIntegrator: '<S193>/Integrator'
   *  Gain: '<S198>/Proportional Gain'
   */
  rtb_preprocessing_accel_out_idx = -35.9833298F * rtb_preprocessing_alt_ned +
    rtDW->Integrator_DSTATE;

  /* Update for DiscreteIntegrator: '<S193>/Integrator' incorporates:
   *  Gain: '<S190>/Integral Gain'
   */
  rtDW->Integrator_DSTATE += -10.7215109F * rtb_preprocessing_alt_ned * 0.01F;

  /* End of Outputs for SubSystem: '<S4>/YawControl' */
  /* End of Outputs for SubSystem: '<S1>/Controller' */

  /* Outputs for Atomic SubSystem: '<S1>/MotorMixing' */
  /* Sum: '<S5>/MotorFR' incorporates:
   *  Sum: '<S5>/MotorBL'
   */
  rtb_MotorBL = rtb_MotorBR + rtb_preprocessing_accel_out_idx;
  rtb_fcn3 = (rtb_MotorBL + rtb_Product3_b) - rtb_Sum_k3;

  /* Saturate: '<S5>/Saturation' */
  if (rtb_fcn3 > 1000.0) {
    /* Outport: '<Root>/Throttle' incorporates:
     *  DataTypeConversion: '<S5>/Data Type Conversion'
     */
    motors_set_motor_us(0, 1000U);
  } else if (rtb_fcn3 < 100.0) {
    /* Outport: '<Root>/Throttle' incorporates:
     *  DataTypeConversion: '<S5>/Data Type Conversion'
     */
    motors_set_motor_us(0, 100U);
  } else {
    /* Outport: '<Root>/Throttle' incorporates:
     *  DataTypeConversion: '<S5>/Data Type Conversion'
     */
    motors_set_motor_us(0, (uint16_T)rtb_fcn3);
  }

  /* Sum: '<S5>/MotorFL' incorporates:
   *  Sum: '<S5>/MotorBR'
   */
  rtb_MotorBR -= rtb_preprocessing_accel_out_idx;
  rtb_fcn3 = (rtb_MotorBR + rtb_Product3_b) + rtb_Sum_k3;

  /* Saturate: '<S5>/Saturation' */
  if (rtb_fcn3 > 1000.0) {
    /* Outport: '<Root>/Throttle' incorporates:
     *  DataTypeConversion: '<S5>/Data Type Conversion'
     */
    motors_set_motor_us(1, 1000U);
  } else if (rtb_fcn3 < 100.0) {
    /* Outport: '<Root>/Throttle' incorporates:
     *  DataTypeConversion: '<S5>/Data Type Conversion'
     */
    motors_set_motor_us(1, 100U);
  } else {
    /* Outport: '<Root>/Throttle' incorporates:
     *  DataTypeConversion: '<S5>/Data Type Conversion'
     */
    motors_set_motor_us(1, (uint16_T)rtb_fcn3);
  }

  /* Sum: '<S5>/MotorBR' */
  rtb_fcn3 = (rtb_MotorBR - rtb_Product3_b) - rtb_Sum_k3;

  /* Saturate: '<S5>/Saturation' */
  if (rtb_fcn3 > 1000.0) {
    /* Outport: '<Root>/Throttle' incorporates:
     *  DataTypeConversion: '<S5>/Data Type Conversion'
     */
    motors_set_motor_us(2, 1000U);
  } else if (rtb_fcn3 < 100.0) {
    /* Outport: '<Root>/Throttle' incorporates:
     *  DataTypeConversion: '<S5>/Data Type Conversion'
     */
    motors_set_motor_us(2, 100U);
  } else {
    /* Outport: '<Root>/Throttle' incorporates:
     *  DataTypeConversion: '<S5>/Data Type Conversion'
     */
    motors_set_motor_us(2, (uint16_T)rtb_fcn3);
  }

  /* Sum: '<S5>/MotorBL' */
  rtb_fcn3 = (rtb_MotorBL - rtb_Product3_b) + rtb_Sum_k3;

  /* Saturate: '<S5>/Saturation' */
  if (rtb_fcn3 > 1000.0) {
    /* Outport: '<Root>/Throttle' incorporates:
     *  DataTypeConversion: '<S5>/Data Type Conversion'
     */
    motors_set_motor_us(3, 1000U);
  } else if (rtb_fcn3 < 100.0) {
    /* Outport: '<Root>/Throttle' incorporates:
     *  DataTypeConversion: '<S5>/Data Type Conversion'
     */
    motors_set_motor_us(3, 100U);
  } else {
    /* Outport: '<Root>/Throttle' incorporates:
     *  DataTypeConversion: '<S5>/Data Type Conversion'
     */
    motors_set_motor_us(3, (uint16_T)rtb_fcn3);
  }

  /* End of Outputs for SubSystem: '<S1>/MotorMixing' */
  /* End of Outputs for SubSystem: '<Root>/FCS' */

  /* Outport: '<Root>/State_Estim' incorporates:
   *  BusCreator generated from: '<Root>/State_Estim'
   *  Concatenate: '<S212>/preprocessing_pqr_concat'
   *  Inport: '<S211>/<gyro_data_SI>'
   */
  rtY_State_Estim->x = 0.0F;
  rtY_State_Estim->y = 0.0F;
  rtY_State_Estim->z = rtb_preprocessing_alt_out;
  rtY_State_Estim->dx = 0.0F;
  rtY_State_Estim->dy = 0.0F;
  rtY_State_Estim->dz = pressureFilter_IIR_tmp;
  rtY_State_Estim->yaw = rtb_orientation_RPY_out[0];
  rtY_State_Estim->pitch = rtb_orientation_RPY_out[1];
  rtY_State_Estim->roll = rtb_orientation_RPY_out[2];

  /* Outputs for Atomic SubSystem: '<Root>/StatesEstimator' */
  /* Outputs for Atomic SubSystem: '<S2>/OrientationEstimator' */
  rtY_State_Estim->r = rtb_preprocessing_gyro_out_idx_;

  /* End of Outputs for SubSystem: '<S2>/OrientationEstimator' */
  /* End of Outputs for SubSystem: '<Root>/StatesEstimator' */
}

/* Model initialize function */
void flightController_initialize(RT_MODEL *const rtM, FCS_command_t
  *rtU_Commands, sensor_data_t *rtU_Sensors, states_estimate_t *rtY_State_Estim)
{
  DW *rtDW = rtM->dwork;

  /* Registration code */

  /* states (dwork) */
  (void) memset((void *)rtDW, 0,
                sizeof(DW));

  /* external inputs */
  *rtU_Commands = flightController_rtZFCS_command;
  *rtU_Sensors = flightController_rtZsensor_data;

  /* external outputs */
  (*rtY_State_Estim) = flightController_rtZstates_estimate_t;

  {
    int32_T i;

    /* SystemInitialize for Atomic SubSystem: '<Root>/StatesEstimator' */
    /* SystemInitialize for Atomic SubSystem: '<S2>/SensorPreprocessing' */
    /* InitializeConditions for DiscreteFir: '<S212>/FIR_IMUaccel' */
    rtDW->FIR_IMUaccel_circBuf = 0;
    for (i = 0; i < 15; i++) {
      rtDW->FIR_IMUaccel_states[i] = 0.0F;
    }

    /* End of InitializeConditions for DiscreteFir: '<S212>/FIR_IMUaccel' */

    /* InitializeConditions for DiscreteFir: '<S212>/FIR_IMUgyro' */
    rtDW->FIR_IMUgyro_circBuf = 0;
    for (i = 0; i < 10; i++) {
      rtDW->FIR_IMUgyro_states[i] = 0.0F;
    }

    /* End of InitializeConditions for DiscreteFir: '<S212>/FIR_IMUgyro' */
    /* End of SystemInitialize for SubSystem: '<S2>/SensorPreprocessing' */
    /* End of SystemInitialize for SubSystem: '<Root>/StatesEstimator' */
  }
}

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
