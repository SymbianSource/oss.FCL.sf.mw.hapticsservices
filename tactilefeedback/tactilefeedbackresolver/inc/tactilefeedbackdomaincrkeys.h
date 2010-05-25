/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef TACTILEFEEDBACKDOMAINCRKEYS_H
#define TACTILEFEEDBACKDOMAINCRKEYS_H

#include <e32std.h>


// =============================================================================
// General Tactile feedback -related configuration options
//
// These settings are customizable but not changeable by end user
// =============================================================================
const TUid KCRUidTactileFeedback = {0x2000B494};


/**
* Base level of vibra feedback strength
*
* Possible values are:
* 0-100
*
* Default value: 50
**/
const TUint32 KTactileFeedbackHapticsStrength       = 0x00000013;

/**
* Base level of audio feedback volume
*
* Possible values are:
* 0-100
*
* Default value: 2
**/
const TUint32 KTactileFeedbackAudioVolume           = 0x00000014;

/**
* Full path of the IVT file, which is used to play haptics effects. 
*
* Possible values are:
* Valid path to IVT file. 
*
* Default value: "z:\\private\\10003B20\\10.1_TactileFeedbackLevel*_A.ivt"
**/
const TUint32 KTactileHapticsLevel1IVTFile          = 0x00000015;
const TUint32 KTactileHapticsLevel2IVTFile          = 0x00000016;
const TUint32 KTactileHapticsLevel3IVTFile          = 0x00000017;
const TUint32 KTactileHapticsLevel4IVTFile          = 0x00000018;
const TUint32 KTactileHapticsLevel5IVTFile          = 0x00000019;

/**
* Number of steps used to adjust vibra feedback strength
*
* Possible values are:
* 1-100
*
* Default value: 10
**/
const TUint32 KTactileFeedbackVibraLevels           = 0x0000001a;

/**
* Number of steps used to adjust audio feedback volume
*
* Possible values are:
* 1-100
*
* Default value: 3
**/
const TUint32 KTactileFeedbackAudioLevels           = 0x0000001b;

#endif  // TACTILEFEEDBACKDOMAINCRKEYS_H

// End of File
