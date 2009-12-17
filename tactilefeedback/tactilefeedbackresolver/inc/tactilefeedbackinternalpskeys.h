/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Private Publish & Subscribe keys of Tactile Feedback
*                subsystem
* Part of:      Tactile Feedback.
*
*/



#ifndef TACTILEFEEDBACKINTERNALPSKEYS_H
#define TACTILEFEEDBACKINTERNALPSKEYS_H

#include <e32property.h>

/**
 * We are using the system category value, because it is below
 * KUidSecurityThresholdCategoryValue, and thus the property can be defined
 * from any process with WriteDeviceData -capability, regardless of 
 * whether the process' SID matches this category.
 */
const TUid KPSUidTactileFeedback = { KUidSystemCategoryValue }; 

/**
 * KTactileFeedbackEnabled
 * Can be used for temporarily disabling tactile feedback in whole
 * system. Should be used in situations such as a telephone call,
 * where tactile feedback can cause disturbing noice on the other
 * end of the telephone call.
 *
 * Writing to this property is protected by WriteDeviceData
 * capability.
 *
 * Audio- and vibra feedback can be disabled separately.
 * Vibra is controlled by bit 0 and audio by bit 1 in the key.
 *
 * The Id used is the UID of Tactile Feedback Resolver
 *
 * Possible values:
 * 0x00: Tactile feedback is currently disabled
 * 0x01: Tactile vibra feedback is enabled, audio disabled
 * 0x02: Tactile vibra feedback is disabled and audio enabled.
 * 0x03: Both Tactile vibra- and audio feedback are enabled. 
 */
const TUint32 KTactileFeedbackEnabled = 0x2000B494;


#endif // TACTILEFEEDBACKINTERNALPSKEYS_H
