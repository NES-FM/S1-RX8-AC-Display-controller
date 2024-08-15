/*
 * File:           D:\Documents\SigmaStudio\Car\CarRadioSub\CarRadioSub\export\CarRadioSub_IC_1_PARAM.h
 *
 * Created:        Thursday, August 15, 2024 7:15:12 PM
 * Description:    CarRadioSub:IC 1 parameter RAM definitions.
 *
 * This software is distributed in the hope that it will be useful,
 * but is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 * CONDITIONS OF ANY KIND, without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * This software may only be used to program products purchased from
 * Analog Devices for incorporation by you into audio products that
 * are intended for resale to audio product end users. This software
 * may not be distributed whole or in any part to third parties.
 *
 * Copyright ©2024 Analog Devices, Inc. All rights reserved.
 */
#ifndef __CARRADIOSUB_IC_1_PARAM_H__
#define __CARRADIOSUB_IC_1_PARAM_H__


/* Module Gen Filter1 - General (2nd order)*/
#define MOD_GENFILTER1_COUNT                           5
#define MOD_GENFILTER1_DEVICE                          "IC1"
#define MOD_GENFILTER1_ALG0_STAGE0_B0_ADDR             0
#define MOD_GENFILTER1_ALG0_STAGE0_B0_FIXPT            0x007ED219
#define MOD_GENFILTER1_ALG0_STAGE0_B0_VALUE            SIGMASTUDIOTYPE_FIXPOINT_CONVERT(0.990786697940427)
#define MOD_GENFILTER1_ALG0_STAGE0_B0_TYPE             SIGMASTUDIOTYPE_FIXPOINT
#define MOD_GENFILTER1_ALG0_STAGE0_B1_ADDR             1
#define MOD_GENFILTER1_ALG0_STAGE0_B1_FIXPT            0xFF025BCE
#define MOD_GENFILTER1_ALG0_STAGE0_B1_VALUE            SIGMASTUDIOTYPE_FIXPOINT_CONVERT(-1.98157339588085)
#define MOD_GENFILTER1_ALG0_STAGE0_B1_TYPE             SIGMASTUDIOTYPE_FIXPOINT
#define MOD_GENFILTER1_ALG0_STAGE0_B2_ADDR             2
#define MOD_GENFILTER1_ALG0_STAGE0_B2_FIXPT            0x007ED219
#define MOD_GENFILTER1_ALG0_STAGE0_B2_VALUE            SIGMASTUDIOTYPE_FIXPOINT_CONVERT(0.990786697940427)
#define MOD_GENFILTER1_ALG0_STAGE0_B2_TYPE             SIGMASTUDIOTYPE_FIXPOINT
#define MOD_GENFILTER1_ALG0_STAGE0_A1_ADDR             3
#define MOD_GENFILTER1_ALG0_STAGE0_A1_FIXPT            0x00FDA16A
#define MOD_GENFILTER1_ALG0_STAGE0_A1_VALUE            SIGMASTUDIOTYPE_FIXPOINT_CONVERT(1.98148850914457)
#define MOD_GENFILTER1_ALG0_STAGE0_A1_TYPE             SIGMASTUDIOTYPE_FIXPOINT
#define MOD_GENFILTER1_ALG0_STAGE0_A2_ADDR             4
#define MOD_GENFILTER1_ALG0_STAGE0_A2_FIXPT            0xFF825906
#define MOD_GENFILTER1_ALG0_STAGE0_A2_VALUE            SIGMASTUDIOTYPE_FIXPOINT_CONVERT(-0.981658282617134)
#define MOD_GENFILTER1_ALG0_STAGE0_A2_TYPE             SIGMASTUDIOTYPE_FIXPOINT

/* Module Mg1 - Signal Merger*/
#define MOD_MG1_COUNT                                  1
#define MOD_MG1_DEVICE                                 "IC1"
#define MOD_MG1_SINGLECTRLMIXER19401_ADDR              5
#define MOD_MG1_SINGLECTRLMIXER19401_FIXPT             0x00400000
#define MOD_MG1_SINGLECTRLMIXER19401_VALUE             SIGMASTUDIOTYPE_FIXPOINT_CONVERT(0.5)
#define MOD_MG1_SINGLECTRLMIXER19401_TYPE              SIGMASTUDIOTYPE_FIXPOINT

/* Module DELAY_CH_1 - Delay*/
#define MOD_DELAY_CH_1_COUNT                           1
#define MOD_DELAY_CH_1_DEVICE                          "IC1"
#define MOD_DELAY_CH_1_DELAYAMT_ADDR                   6
#define MOD_DELAY_CH_1_DELAYAMT_FIXPT                  0x00000001
#define MOD_DELAY_CH_1_DELAYAMT_VALUE                  SIGMASTUDIOTYPE_INTEGER_CONVERT(1)
#define MOD_DELAY_CH_1_DELAYAMT_TYPE                   SIGMASTUDIOTYPE_INTEGER

/* Module DELAY_CH_2 - Delay*/
#define MOD_DELAY_CH_2_COUNT                           1
#define MOD_DELAY_CH_2_DEVICE                          "IC1"
#define MOD_DELAY_CH_2_DELAYAMT_ADDR                   7
#define MOD_DELAY_CH_2_DELAYAMT_FIXPT                  0x00000008
#define MOD_DELAY_CH_2_DELAYAMT_VALUE                  SIGMASTUDIOTYPE_INTEGER_CONVERT(8)
#define MOD_DELAY_CH_2_DELAYAMT_TYPE                   SIGMASTUDIOTYPE_INTEGER

/* Module Gen Filter2 - General (2nd order)*/
#define MOD_GENFILTER2_COUNT                           5
#define MOD_GENFILTER2_DEVICE                          "IC1"
#define MOD_GENFILTER2_ALG0_STAGE0_B0_ADDR             8
#define MOD_GENFILTER2_ALG0_STAGE0_B0_FIXPT            0x00000164
#define MOD_GENFILTER2_ALG0_STAGE0_B0_VALUE            SIGMASTUDIOTYPE_FIXPOINT_CONVERT(4.2443368140217E-05)
#define MOD_GENFILTER2_ALG0_STAGE0_B0_TYPE             SIGMASTUDIOTYPE_FIXPOINT
#define MOD_GENFILTER2_ALG0_STAGE0_B1_ADDR             9
#define MOD_GENFILTER2_ALG0_STAGE0_B1_FIXPT            0x000002C8
#define MOD_GENFILTER2_ALG0_STAGE0_B1_VALUE            SIGMASTUDIOTYPE_FIXPOINT_CONVERT(8.4886736280434E-05)
#define MOD_GENFILTER2_ALG0_STAGE0_B1_TYPE             SIGMASTUDIOTYPE_FIXPOINT
#define MOD_GENFILTER2_ALG0_STAGE0_B2_ADDR             10
#define MOD_GENFILTER2_ALG0_STAGE0_B2_FIXPT            0x00000164
#define MOD_GENFILTER2_ALG0_STAGE0_B2_VALUE            SIGMASTUDIOTYPE_FIXPOINT_CONVERT(4.2443368140217E-05)
#define MOD_GENFILTER2_ALG0_STAGE0_B2_TYPE             SIGMASTUDIOTYPE_FIXPOINT
#define MOD_GENFILTER2_ALG0_STAGE0_A1_ADDR             11
#define MOD_GENFILTER2_ALG0_STAGE0_A1_FIXPT            0x00FDA16A
#define MOD_GENFILTER2_ALG0_STAGE0_A1_VALUE            SIGMASTUDIOTYPE_FIXPOINT_CONVERT(1.98148850914457)
#define MOD_GENFILTER2_ALG0_STAGE0_A1_TYPE             SIGMASTUDIOTYPE_FIXPOINT
#define MOD_GENFILTER2_ALG0_STAGE0_A2_ADDR             12
#define MOD_GENFILTER2_ALG0_STAGE0_A2_FIXPT            0xFF825906
#define MOD_GENFILTER2_ALG0_STAGE0_A2_VALUE            SIGMASTUDIOTYPE_FIXPOINT_CONVERT(-0.981658282617134)
#define MOD_GENFILTER2_ALG0_STAGE0_A2_TYPE             SIGMASTUDIOTYPE_FIXPOINT

/* Module GAIN_SUB - Gain*/
#define MOD_GAIN_SUB_COUNT                             1
#define MOD_GAIN_SUB_DEVICE                            "IC1"
#define MOD_GAIN_SUB_GAIN1940ALGNS2_ADDR               13
#define MOD_GAIN_SUB_GAIN1940ALGNS2_FIXPT              0x00800000
#define MOD_GAIN_SUB_GAIN1940ALGNS2_VALUE              SIGMASTUDIOTYPE_FIXPOINT_CONVERT(1)
#define MOD_GAIN_SUB_GAIN1940ALGNS2_TYPE               SIGMASTUDIOTYPE_FIXPOINT

/* Module VOL_SUB - Single SW slew vol (adjustable)*/
#define MOD_VOL_SUB_COUNT                              2
#define MOD_VOL_SUB_DEVICE                             "IC1"
#define MOD_VOL_SUB_ALG0_TARGET_ADDR                   14
#define MOD_VOL_SUB_ALG0_TARGET_FIXPT                  0x00800000
#define MOD_VOL_SUB_ALG0_TARGET_VALUE                  SIGMASTUDIOTYPE_FIXPOINT_CONVERT(1)
#define MOD_VOL_SUB_ALG0_TARGET_TYPE                   SIGMASTUDIOTYPE_FIXPOINT
#define MOD_VOL_SUB_ALG0_STEP_ADDR                     15
#define MOD_VOL_SUB_ALG0_STEP_FIXPT                    0x00000800
#define MOD_VOL_SUB_ALG0_STEP_VALUE                    SIGMASTUDIOTYPE_FIXPOINT_CONVERT(0.000244140625)
#define MOD_VOL_SUB_ALG0_STEP_TYPE                     SIGMASTUDIOTYPE_FIXPOINT

/* Module DELAY_SUB - Delay*/
#define MOD_DELAY_SUB_COUNT                            1
#define MOD_DELAY_SUB_DEVICE                           "IC1"
#define MOD_DELAY_SUB_DELAYAMT_ADDR                    16
#define MOD_DELAY_SUB_DELAYAMT_FIXPT                   0x00000001
#define MOD_DELAY_SUB_DELAYAMT_VALUE                   SIGMASTUDIOTYPE_INTEGER_CONVERT(1)
#define MOD_DELAY_SUB_DELAYAMT_TYPE                    SIGMASTUDIOTYPE_INTEGER

#endif
