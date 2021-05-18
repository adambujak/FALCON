/*
 * Academic License - for use in teaching, academic research, and meeting
 * course requirements at degree granting institutions only.  Not for
 * government, commercial, or other organizational use.
 *
 * File: flightController_data.c
 *
 * Code generated for Simulink model 'flightController'.
 *
 * Model version                  : 1.143
 * Simulink Coder version         : 9.4 (R2020b) 29-Jul-2020
 * C/C++ source code generated on : Mon May 17 19:59:13 2021
 *
 * Target selection: ert.tlc
 * Embedded hardware selection: ARM Compatible->ARM Cortex-M
 * Code generation objectives:
 *    1. Execution efficiency
 *    2. RAM efficiency
 * Validation result: Not run
 */

#include "flightController.h"

/* Constant parameters (default storage) */
const ConstP rtConstP = {
  /* Computed Parameter: FIR_IMUaccel_Coefficients
   * Referenced by: '<S14>/FIR_IMUaccel'
   */
  { 0.0264077242F, 0.140531361F, 0.33306092F, 0.33306092F, 0.140531361F,
    0.0264077242F },

  /* Computed Parameter: FIR_IMUgyro_Coefficients
   * Referenced by: '<S14>/FIR_IMUgyro'
   */
  { -2.48169962E-18F, -0.0493636839F, 0.549363673F, 0.549363673F, -0.0493636839F,
    -2.48169962E-18F },

  /* Computed Parameter: IIR_IMUgyro_r_NumCoef
   * Referenced by: '<S14>/IIR_IMUgyro_r'
   */
  { 0.282124132F, 1.27253926F, 2.42084408F, 2.42084408F, 1.27253926F,
    0.282124132F },

  /* Computed Parameter: IIR_IMUgyro_r_DenCoef
   * Referenced by: '<S14>/IIR_IMUgyro_r'
   */
  { 1.0F, 2.22871494F, 2.52446198F, 1.57725322F, 0.54102242F, 0.0795623958F }
};

/*
 * File trailer for generated code.
 *
 * [EOF]
 */
