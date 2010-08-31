/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  CR Keys for the configurable parameters related to physical 
*                vibrator and audio feedback effects played by 
*                Tactile Feedback Resolver
* Part of:      Tactile Feedback.
*
*/

#ifndef TACTILEFEEDBACKPRIVATECRKEYS_H
#define TACTILEFEEDBACKPRIVATECRKEYS_H

#include <e32std.h>
#include "tactilefeedbackdomaincrkeys.h"


/**
* Minimun allowed interval of physical feedback effects
*
* This key can be used for filtering the feedback requests in case that is
* required by device HW. For example: When painting text in editors, it is
* possible to generate dozens of feedback requests in a second. This may feel
* very unpleasant to end user.
*
* This key defines the minimum time that must have passed since previous
* feedback request, or otherwise feedback request is ignored.
* 
* If this key value is defined to zero, then this feature is disabled (i.e.
* all feedbacks are played no matter how fast they come).
*
* Possible values are:
* Integer value that represents the minimum interval in milliseconds, or
* zero for disabling the feature.
*
* Default value: 50ms
**/
const TUint32 KTactileFeedbackMinimumInterval       = 0x0000001F;

// =============================================================================
// Configuration options related to vibrator parameters
//
// These settings are customizable but not changeable by end user
// =============================================================================

/**
* Vibra intensity setting for logical feedback type ETouchFeedbackBasic
* and vibra effect level ETactileFeedbackVibraLevel1.
*
* Possible values are:
* Integers from 0 to 100. See HWRMVibra.h for more information.
* Zero intensity will disable this logical feedback type.
*
* Default value: 10
**/
const TUint32 KTactileVibraIntensityBasicLevel1      = 0x00000020;

/**
* Vibra intensity setting for logical feedback type ETouchFeedbackBasic
* and vibra effect level ETactileFeedbackVibraLevel2.
*
* Possible values are:
* Integers from 0 to 100. See HWRMVibra.h for more information.
* Zero intensity will disable this logical feedback type.
*
* Default value: 40
**/
const TUint32 KTactileVibraIntensityBasicLevel2      = 0x00000021;

/**
* Vibra intensity setting for logical feedback type ETouchFeedbackBasic
* and vibra effect level ETactileFeedbackVibraLevel3.
*
* Possible values are:
* Integers from 0 to 100. See HWRMVibra.h for more information.
* Zero intensity will disable this logical feedback type.
*
* Default value: 40
**/
const TUint32 KTactileVibraIntensityBasicLevel3      = 0x00000022;

/**
* Vibra intensity setting for logical feedback type ETouchFeedbackBasic
* and vibra effect level ETactileFeedbackVibraLevel4.
*
* Possible values are:
* Integers from 0 to 100. See HWRMVibra.h for more information.
* Zero intensity will disable this logical feedback type.
*
* Default value: 60
**/
const TUint32 KTactileVibraIntensityBasicLevel4      = 0x00000023;

/**
* Vibra intensity setting for logical feedback type ETouchFeedbackBasic
* and vibra effect level ETactileFeedbackVibraLevel5.
*
* Possible values are:
* Integers from 0 to 100. See HWRMVibra.h for more information.
* Zero intensity will disable this logical feedback type.
*
* Default value: 80
**/
const TUint32 KTactileVibraIntensityBasicLevel5      = 0x00000024;

/**
* Vibra duration setting for logical feedback type ETouchFeedbackBasic
* and vibra effect level ETactileFeedbackVibraLevel1.
*
* Duration is given in milliseconds, and zero value will disable this
* logical feedback type. 
*
* Possible values are:
* Integer values from zero upwards.
*
* Default value: 15
**/
const TUint32 KTactileVibraDurationBasicLevel1       = 0x00000025;

/**
* Vibra duration setting for logical feedback type ETouchFeedbackBasic
* and vibra effect level ETactileFeedbackVibraLevel2.
*
* Duration is given in milliseconds, and zero value will disable this
* logical feedback type. 
*
* Possible values are:
* Integer values from zero upwards.
*
* Default value: 20
**/
const TUint32 KTactileVibraDurationBasicLevel2       = 0x00000026;

/**
* Vibra duration setting for logical feedback type ETouchFeedbackBasic
* and vibra effect level ETactileFeedbackVibraLevel3.
*
* Duration is given in milliseconds, and zero value will disable this
* logical feedback type. 
*
* Possible values are:
* Integer values from zero upwards.
*
* Default value: 30
**/
const TUint32 KTactileVibraDurationBasicLevel3       = 0x00000027;

/**
* Vibra duration setting for logical feedback type ETouchFeedbackBasic
* and vibra effect level ETactileFeedbackVibraLevel4.
*
* Duration is given in milliseconds, and zero value will disable this
* logical feedback type. 
*
* Possible values are:
* Integer values from zero upwards.
*
* Default value: 40
**/
const TUint32 KTactileVibraDurationBasicLevel4       = 0x00000028;

/**
* Vibra duration setting for logical feedback type ETouchFeedbackBasic
* and vibra effect level ETactileFeedbackVibraLevel5.
*
* Duration is given in milliseconds, and zero value will disable this
* logical feedback type. 
*
* Possible values are:
* Integer values from zero upwards.
*
* Default value: 50
**/
const TUint32 KTactileVibraDurationBasicLevel5       = 0x00000029;


/**
* Vibra intensity setting for logical feedback type ETouchFeedbackSensitive
* and vibra effect level ETactileFeedbackVibraLevel1.
*
* Possible values are:
* Integers from 0 to 100. See HWRMVibra.h for more information.
* Zero intensity will disable this logical feedback type.
*
* Default value: 50
**/
const TUint32 KTactileVibraIntensitySensitiveLevel1  = 0x0000002a;

/**
* Vibra intensity setting for logical feedback type ETouchFeedbackSensitive
* and vibra effect level ETactileFeedbackVibraLevel2.
*
* Possible values are:
* Integers from 0 to 100. See HWRMVibra.h for more information.
* Zero intensity will disable this logical feedback type.
*
* Default value: 100
**/
const TUint32 KTactileVibraIntensitySensitiveLevel2  = 0x0000002b;

/**
* Vibra intensity setting for logical feedback type ETouchFeedbackSensitive
* and vibra effect level ETactileFeedbackVibraLevel3.
*
* Possible values are:
* Integers from 0 to 100. See HWRMVibra.h for more information.
* Zero intensity will disable this logical feedback type.
*
* Default value: 10
**/
const TUint32 KTactileVibraIntensitySensitiveLevel3  = 0x0000002c;

/**
* Vibra intensity setting for logical feedback type ETouchFeedbackSensitive
* and vibra effect level ETactileFeedbackVibraLevel4.
*
* Possible values are:
* Integers from 0 to 100. See HWRMVibra.h for more information.
* Zero intensity will disable this logical feedback type.
*
* Default value: 10
**/
const TUint32 KTactileVibraIntensitySensitiveLevel4  = 0x0000002d;

/**
* Vibra intensity setting for logical feedback type ETouchFeedbackSensitive
* and vibra effect level ETactileFeedbackVibraLevel5.
*
* Possible values are:
* Integers from 0 to 100. See HWRMVibra.h for more information.
* Zero intensity will disable this logical feedback type.
*
* Default value: 10
**/
const TUint32 KTactileVibraIntensitySensitiveLevel5  = 0x0000002e;


/**
* Vibra duration setting for logical feedback type ETouchFeedbackSensitive
* and vibra effect level ETactileFeedbackVibraLevel1.
*
* Duration is given in milliseconds, and zero value will disable this
* logical feedback type. 
*
* Possible values are:
* Integer values from zero upwards.
*
* Default value: 5
**/
const TUint32 KTactileVibraDurationSensitiveLevel1  = 0x0000002f;

/**
* Vibra duration setting for logical feedback type ETouchFeedbackSensitive
* and vibra effect level ETactileFeedbackVibraLevel2.
*
* Duration is given in milliseconds, and zero value will disable this
* logical feedback type. 
*
* Possible values are:
* Integer values from zero upwards.
*
* Default value: 10
**/
const TUint32 KTactileVibraDurationSensitiveLevel2  = 0x00000030;

/**
* Vibra duration setting for logical feedback type ETouchFeedbackSensitive
* and vibra effect level ETactileFeedbackVibraLevel3.
*
* Duration is given in milliseconds, and zero value will disable this
* logical feedback type. 
*
* Possible values are:
* Integer values from zero upwards.
*
* Default value: 15
**/
const TUint32 KTactileVibraDurationSensitiveLevel3  = 0x00000031;

/**
* Vibra duration setting for logical feedback type ETouchFeedbackSensitive
* and vibra effect level ETactileFeedbackVibraLevel4.
*
* Duration is given in milliseconds, and zero value will disable this
* logical feedback type. 
*
* Possible values are:
* Integer values from zero upwards.
*
* Default value: 20
**/
const TUint32 KTactileVibraDurationSensitiveLevel4  = 0x00000032;

/**
* Vibra duration setting for logical feedback type ETouchFeedbackSensitive
* and vibra effect level ETactileFeedbackVibraLevel5.
*
* Duration is given in milliseconds, and zero value will disable this
* logical feedback type. 
*
* Possible values are:
* Integer values from zero upwards.
*
* Default value: 25
**/
const TUint32 KTactileVibraDurationSensitiveLevel5  = 0x00000033;



// =============================================================================
// Configuration options related to audio tone parameters
//
// These settings are customizable but not changeable by end user
// =============================================================================

/**
* Audio frequency setting for logical feedback type ETouchFeedbackBasic
* and audio effect level ETactileFeedbackAudioLevel1.
*
* Frequency is given in Hz.
*
* Possible values are:
* Positive integer values.
*
* Default value: 1100
**/
const TUint32 KTactileAudioToneFreqBasicLevel1      = 0x00000040;

/**
* Audio frequency setting for logical feedback type ETouchFeedbackBasic
* and audio effect level ETactileFeedbackAudioLevel2.
*
* Frequency is given in Hz.
*
* Possible values are:
* Positive integer values.
*
* Default value: 1100
**/
const TUint32 KTactileAudioToneFreqBasicLevel2      = 0x00000041;

/**
* Audio frequency setting for logical feedback type ETouchFeedbackBasic
* and audio effect level ETactileFeedbackAudioLevel3.
*
* Frequency is given in Hz.
*
* Possible values are:
* Positive integer values.
*
* Default value: 1100
**/
const TUint32 KTactileAudioToneFreqBasicLevel3      = 0x00000042;

/**
* Audio frequency setting for logical feedback type ETouchFeedbackBasic
* and audio effect level ETactileFeedbackAudioLevel4.
*
* Frequency is given in Hz.
*
* Possible values are:
* Positive integer values.
*
* Default value: 1100
**/
const TUint32 KTactileAudioToneFreqBasicLevel4      = 0x00000043;

/**
* Audio frequency setting for logical feedback type ETouchFeedbackBasic
* and audio effect level ETactileFeedbackAudioLevel5.
*
* Frequency is given in Hz.
*
* Possible values are:
* Positive integer values.
*
* Default value: 1100
**/
const TUint32 KTactileAudioToneFreqBasicLevel5      = 0x00000044;

/**
* Audio duration setting for logical feedback type ETouchFeedbackBasic
* and audio effect level ETactileFeedbackAudioLevel1.
*
* Duration is given in milliseconds.
*
* Possible values are:
* Zero (disables this feedback type) or positive integers
*
* Default value: 10
**/
const TUint32 KTactileAudioToneDurationBasicLevel1  = 0x00000045;

/**
* Audio duration setting for logical feedback type ETouchFeedbackBasic
* and audio effect level ETactileFeedbackAudioLevel2.
*
* Duration is given in milliseconds.
*
* Possible values are:
* Zero (disables this feedback type) or positive integers
*
* Default value: 10
**/
const TUint32 KTactileAudioToneDurationBasicLevel2  = 0x00000046;

/**
* Audio duration setting for logical feedback type ETouchFeedbackBasic
* and audio effect level ETactileFeedbackAudioLevel3.
*
* Duration is given in milliseconds.
*
* Possible values are:
* Zero (disables this feedback type) or positive integers
*
* Default value: 10
**/
const TUint32 KTactileAudioToneDurationBasicLevel3  = 0x00000047;

/**
* Audio duration setting for logical feedback type ETouchFeedbackBasic
* and audio effect level ETactileFeedbackAudioLevel4.
*
* Duration is given in milliseconds.
*
* Possible values are:
* Zero (disables this feedback type) or positive integers
*
* Default value: 10
**/
const TUint32 KTactileAudioToneDurationBasicLevel4  = 0x00000048;

/**
* Audio duration setting for logical feedback type ETouchFeedbackBasic
* and audio effect level ETactileFeedbackAudioLevel5.
*
* Duration is given in milliseconds.
*
* Possible values are:
* Zero (disables this feedback type) or positive integers
*
* Default value: 10
**/
const TUint32 KTactileAudioToneDurationBasicLevel5  = 0x00000049;

/**
* Audio volume setting for logical feedback type ETouchFeedbackBasic
* and audio effect level ETactileFeedbackAudioLevel1.
*
* Volume level is given in percentages. 100% equals to 
* CMdaAudioToneUtility::MaxVolume, and 0% will disable this feedback type.
*
* Possible values are:
* 0 - 100 
*
* Default value: 20
**/
const TUint32 KTactileAudioToneVolumeBasicLevel1    = 0x0000004a;

/**
* Audio volume setting for logical feedback type ETouchFeedbackBasic
* and audio effect level ETactileFeedbackAudioLevel2.
*
* Volume level is given in percentages. 100% equals to 
* CMdaAudioToneUtility::MaxVolume, and 0% will disable this feedback type.
*
* Possible values are:
* 0 - 100 
*
* Default value: 40
**/
const TUint32 KTactileAudioToneVolumeBasicLevel2    = 0x0000004b;

/**
* Audio volume setting for logical feedback type ETouchFeedbackBasic
* and audio effect level ETactileFeedbackAudioLevel3.
*
* Volume level is given in percentages. 100% equals to 
* CMdaAudioToneUtility::MaxVolume, and 0% will disable this feedback type.
*
* Possible values are:
* 0 - 100 
*
* Default value: 60
**/
const TUint32 KTactileAudioToneVolumeBasicLevel3    = 0x0000004c;

/**
* Audio volume setting for logical feedback type ETouchFeedbackBasic
* and audio effect level ETactileFeedbackAudioLevel4.
*
* Volume level is given in percentages. 100% equals to 
* CMdaAudioToneUtility::MaxVolume, and 0% will disable this feedback type.
*
* Possible values are:
* 0 - 100 
*
* Default value: 80
**/
const TUint32 KTactileAudioToneVolumeBasicLevel4    = 0x0000004d;

/**
* Audio volume setting for logical feedback type ETouchFeedbackBasic
* and audio effect level ETactileFeedbackAudioLevel5.
*
* Volume level is given in percentages. 100% equals to 
* CMdaAudioToneUtility::MaxVolume, and 0% will disable this feedback type.
*
* Possible values are:
* 0 - 100 
*
* Default value: 100
**/
const TUint32 KTactileAudioToneVolumeBasicLevel5    = 0x0000004e;


/**
* Audio frequency setting for logical feedback type ETouchFeedbackSensitive
* and audio effect level ETactileFeedbackAudioLevel1.
*
* Frequency is given in Hz.
*
* Possible values are:
* Positive integer values.
*
* Default value: 1100
**/
const TUint32 KTactileAudioToneFreqSensitiveLevel1  = 0x0000004f;

/**
* Audio frequency setting for logical feedback type ETouchFeedbackSensitive
* and audio effect level ETactileFeedbackAudioLevel2.
*
* Frequency is given in Hz.
*
* Possible values are:
* Positive integer values.
*
* Default value: 1100
**/
const TUint32 KTactileAudioToneFreqSensitiveLevel2  = 0x00000050;

/**
* Audio frequency setting for logical feedback type ETouchFeedbackSensitive
* and audio effect level ETactileFeedbackAudioLevel3.
*
* Frequency is given in Hz.
*
* Possible values are:
* Positive integer values.
*
* Default value: 1100
**/
const TUint32 KTactileAudioToneFreqSensitiveLevel3  = 0x00000051;

/**
* Audio frequency setting for logical feedback type ETouchFeedbackSensitive
* and audio effect level ETactileFeedbackAudioLevel4.
*
* Frequency is given in Hz.
*
* Possible values are:
* Positive integer values.
*
* Default value: 1100
**/
const TUint32 KTactileAudioToneFreqSensitiveLevel4  = 0x00000052;

/**
* Audio frequency setting for logical feedback type ETouchFeedbackSensitive
* and audio effect level ETactileFeedbackAudioLevel5.
*
* Frequency is given in Hz.
*
* Possible values are:
* Positive integer values.
*
* Default value: 1100
**/
const TUint32 KTactileAudioToneFreqSensitiveLevel5  = 0x00000053;


/**
* Audio duration setting for logical feedback type ETouchFeedbackSensitive
* and audio effect level ETactileFeedbackAudioLevel1.
*
* Duration is given in milliseconds.
*
* Possible values are:
* Zero (disables this feedback type) or positive integers
*
* Default value: 10
**/
const TUint32 KTactileAudioToneDurationSensitiveLevel1 = 0x00000054;

/**
* Audio duration setting for logical feedback type ETouchFeedbackSensitive
* and audio effect level ETactileFeedbackAudioLevel2.
*
* Duration is given in milliseconds.
*
* Possible values are:
* Zero (disables this feedback type) or positive integers
*
* Default value: 10
**/
const TUint32 KTactileAudioToneDurationSensitiveLevel2 = 0x00000055;

/**
* Audio duration setting for logical feedback type ETouchFeedbackSensitive
* and audio effect level ETactileFeedbackAudioLevel3.
*
* Duration is given in milliseconds.
*
* Possible values are:
* Zero (disables this feedback type) or positive integers
*
* Default value: 10
**/
const TUint32 KTactileAudioToneDurationSensitiveLevel3 = 0x00000056;

/**
* Audio duration setting for logical feedback type ETouchFeedbackSensitive
* and audio effect level ETactileFeedbackAudioLevel4.
*
* Duration is given in milliseconds.
*
* Possible values are:
* Zero (disables this feedback type) or positive integers
*
* Default value: 10
**/
const TUint32 KTactileAudioToneDurationSensitiveLevel4 = 0x00000057;

/**
* Audio duration setting for logical feedback type ETouchFeedbackSensitive
* and audio effect level ETactileFeedbackAudioLevel5.
*
* Duration is given in milliseconds.
*
* Possible values are:
* Zero (disables this feedback type) or positive integers
*
* Default value: 10
**/
const TUint32 KTactileAudioToneDurationSensitiveLevel5 = 0x00000058;

/**
* Audio volume setting for logical feedback type ETouchFeedbackSensitive
* and audio effect level ETactileFeedbackAudioLevel1.
*
* Volume level is given in percentages. 100% equals to 
* CMdaAudioToneUtility::MaxVolume, and 0% will disable this feedback type.
*
* Possible values are:
* 0 - 100
*
* Default value: 10
**/
const TUint32 KTactileAudioToneVolumeSensitiveLevel1   = 0x00000059;

/**
* Audio volume setting for logical feedback type ETouchFeedbackSensitive
* and audio effect level ETactileFeedbackAudioLevel2.
*
* Volume level is given in percentages. 100% equals to 
* CMdaAudioToneUtility::MaxVolume, and 0% will disable this feedback type.
*
* Possible values are:
* 0 - 100
*
* Default value: 25
**/
const TUint32 KTactileAudioToneVolumeSensitiveLevel2   = 0x0000005a;

/**
* Audio volume setting for logical feedback type ETouchFeedbackSensitive
* and audio effect level ETactileFeedbackAudioLevel3.
*
* Volume level is given in percentages. 100% equals to 
* CMdaAudioToneUtility::MaxVolume, and 0% will disable this feedback type.
*
* Possible values are:
* 0 - 100
*
* Default value: 40
**/
const TUint32 KTactileAudioToneVolumeSensitiveLevel3   = 0x0000005b;

/**
* Audio volume setting for logical feedback type ETouchFeedbackSensitive
* and audio effect level ETactileFeedbackAudioLevel4.
*
* Volume level is given in percentages. 100% equals to 
* CMdaAudioToneUtility::MaxVolume, and 0% will disable this feedback type.
*
* Possible values are:
* 0 - 100
*
* Default value: 55
**/
const TUint32 KTactileAudioToneVolumeSensitiveLevel4   = 0x0000005c;

/**
* Audio volume setting for logical feedback type ETouchFeedbackSensitive
* and audio effect level ETactileFeedbackAudioLevel5.
*
* Volume level is given in percentages. 100% equals to 
* CMdaAudioToneUtility::MaxVolume, and 0% will disable this feedback type.
*
* Possible values are:
* 0 - 100
*
* Default value: 70
**/
const TUint32 KTactileAudioToneVolumeSensitiveLevel5   = 0x0000005d;

// =============================================================================
// Configuration options related to audio .wav feedback
//
// These settings are customizable but not changeable by end user
// =============================================================================

/**
* Full path of the .wav -file that is going to be used as basic feedback
* and audio effect level ETactileFeedbackAudioLevel1.
*
* Possible values are:
* Valid path to .wav -file. If invalid or empty path is given, then
* no .wav feedback shall be generated for this logical feedback type.
*
* Default value: "z:\\System\\Sounds\\Digital\\ui-pen_down04.wav"
**/
const TUint32 KTactileAudioWavFileBasicLevel1          = 0x00000080;

/**
* Full path of the .wav -file that is going to be used as basic feedback
* and audio effect level ETactileFeedbackAudioLevel2.
*
* Possible values are:
* Valid path to .wav -file. If invalid or empty path is given, then
* no .wav feedback shall be generated for this logical feedback type.
*
* Default value: "z:\\System\\Sounds\\Digital\\ui-pen_down04.wav"
**/
const TUint32 KTactileAudioWavFileBasicLevel2          = 0x00000081;

/**
* Full path of the .wav -file that is going to be used as basic feedback
* and audio effect level ETactileFeedbackAudioLevel3.
*
* Possible values are:
* Valid path to .wav -file. If invalid or empty path is given, then
* no .wav feedback shall be generated for this logical feedback type.
*
* Default value: "z:\\System\\Sounds\\Digital\\ui-pen_down04.wav"
**/
const TUint32 KTactileAudioWavFileBasicLevel3          = 0x00000082;

/**
* Sound volume to be used when playing the basic .wav feedback
* and audio effect level ETactileFeedbackAudioLevel1.
*
* Volume level is given in percentages. 100% equals to 
* CMdaAudioPlayerUtility::MaxVolume, and 0% will disable this feedback type.
*
* Possible values are:
* 0-100
*
* Default value: 10
**/
const TUint32 KTactileAudioWavVolumeBasicLevel1        = 0x00000083;

/**
* Sound volume to be used when playing the basic .wav feedback
* and audio effect level ETactileFeedbackAudioLevel2.
*
* Volume level is given in percentages. 100% equals to 
* CMdaAudioPlayerUtility::MaxVolume, and 0% will disable this feedback type.
*
* Possible values are:
* 0-100
*
* Default value: 30
**/
const TUint32 KTactileAudioWavVolumeBasicLevel2        = 0x00000084;

/**
* Sound volume to be used when playing the basic .wav feedback
* and audio effect level ETactileFeedbackAudioLevel3.
*
* Volume level is given in percentages. 100% equals to 
* CMdaAudioPlayerUtility::MaxVolume, and 0% will disable this feedback type.
*
* Possible values are:
* 0-100
*
* Default value: 50
**/
const TUint32 KTactileAudioWavVolumeBasicLevel3        = 0x00000085;

/**
* Full path of the .wav -file that is going to be used as sensitive feedback
* and audio effect level ETactileFeedbackAudioLevel1.
*
* Possible values are:
* Valid path to .wav -file. If invalid or empty path is given, then
* no .wav feedback shall be generated for this logical feedback type.
*
* Default value: "z:\\System\\Sounds\\Digital\\ui-pen_down04.wav"
**/
const TUint32 KTactileAudioWavFileSensitiveLevel1       = 0x00000086;

/**
* Full path of the .wav -file that is going to be used as sensitive feedback
* and audio effect level ETactileFeedbackAudioLevel2.
*
* Possible values are:
* Valid path to .wav -file. If invalid or empty path is given, then
* no .wav feedback shall be generated for this logical feedback type.
*
* Default value: "z:\\System\\Sounds\\Digital\\ui-pen_down04.wav"
**/
const TUint32 KTactileAudioWavFileSensitiveLevel2       = 0x00000087;

/**
* Full path of the .wav -file that is going to be used as sensitive feedback
* and audio effect level ETactileFeedbackAudioLevel3.
*
* Possible values are:
* Valid path to .wav -file. If invalid or empty path is given, then
* no .wav feedback shall be generated for this logical feedback type.
*
* Default value: "z:\\System\\Sounds\\Digital\\ui-pen_down04.wav"
**/
const TUint32 KTactileAudioWavFileSensitiveLevel3       = 0x00000088;

/**
* Sound volume to be used when playing the sensitive .wav feedback
* and audio effect level ETactileFeedbackAudioLevel1.
*
* Volume level is given in percentages. 100% equals to 
* CMdaAudioPlayerUtility::MaxVolume, and 0% will disable this feedback type.
*
* Possible values are:
* 0-100
*
* Default value: 5
**/
const TUint32 KTactileAudioWavVolumeSensitiveLevel1     = 0x00000089;

/**
* Sound volume to be used when playing the sensitive .wav feedback
* and audio effect level ETactileFeedbackAudioLevel2.
*
* Volume level is given in percentages. 100% equals to 
* CMdaAudioPlayerUtility::MaxVolume, and 0% will disable this feedback type.
*
* Possible values are:
* 0-100
*
* Default value: 10
**/
const TUint32 KTactileAudioWavVolumeSensitiveLevel2     = 0x0000008a;

/**
* Sound volume to be used when playing the sensitive .wav feedback
* and audio effect level ETactileFeedbackAudioLevel3.
*
* Volume level is given in percentages. 100% equals to 
* CMdaAudioPlayerUtility::MaxVolume, and 0% will disable this feedback type.
*
* Possible values are:
* 0-100
*
* Default value: 25
**/
const TUint32 KTactileAudioWavVolumeSensitiveLevel3     = 0x0000008b;

// =============================================================================
// Configuration options related to Tactile player plugins.
//
// These settings are customizable but not changeable by end user
// =============================================================================

/**
* Implementation UID of the plugin, which is used to play haptics feedback.
*
* Possible values are:
* Implementation UID of the plugin.
*
* Default value: 536988569 (0x2001CB99)
**/
const TUint32 KTactileHapticsPlugin                     = 0x00000090;

/**
* Implementation UID of the plugin, which is used to play audio feedback.
*
* Possible values are:
* Implementation UID of the plugin.
*
* Default value: 536988573 (0x2001CB9D)
**/
const TUint32 KTactileAudioPlugin                       = 0x00000091;

/**
* Stores tactile flags bitmask:
* Bit 1: Vibra disabled during call
* Bit 2: Audio disabled during call
* Bit 3: Vibra played first
*/
const TUint KTactileInternalSettings                    = 0x00000092;

/**
* Defines bit 1 in KTactileInternalSettings. 
* If this bit is 1, ongoing call will disable vibra feedback.
* If this bit is 0, ongoing call doesn't affect vibra feedback.
* By default this is 1.
*/
const TInt KTactileInternalSettingsCallDisableVibra = 0x01;  // 2^0

/**
* Defines bit 2 in KTactileInternalSettings. 
* If this bit is 1, ongoing call will disable audio feedback.
* If this bit is 0, ongoing call doesn't affect audio feedback.
* By default this is 1.
*/
const TInt KTactileInternalSettingsCallDisableAudio = 0x02;  // 2^1

/**
* Defines bit 3 in KTactileInternalSettings. 
* If this bit is 1, vibra feedback is played the first and
* audio feedback after that.
* If this bit is 0, audio feedback is played the first and
* vibra feedback after that.
* By default this is 1.
*/
const TInt KTactileInternalSettingsVibraPlayedFirst = 0x04;  // 2^2


/**
* Logical actuator type in Tactile Haptics Player.
*
* Possible values are defined in enumeration THWRMLogicalActuators
* in file hwrmlogicalactuators.h.
* 
* Default value: 1 (EHWRMLogicalActuatorAny)
*/
const TInt KTactileFeedbackHapticsActuator              = 0x00000093;

#endif  // TACTILEFEEDBACKPRIVATECRKEYS_H

// End of File
